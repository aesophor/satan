#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>

static unsigned long **fxcy_sys_call_table = NULL;

static void fxcy_locate_sys_call_table(void);


static int __init fuxsocy_init(void) {
  pr_info("fuxsocy: Hello friend\n");
  fxcy_locate_sys_call_table();
  return 0;
}

static void __exit fuxsocy_exit(void) {
  pr_info("fuxsocy: Goodbye friend\n");
}


static void fxcy_locate_sys_call_table(void) {
  unsigned long *fxcy_sys_read = NULL;
  fxcy_sys_read = (void *) kallsyms_lookup_name("sys_read");
  pr_info("fuxsocy: sys_read: %p\n", fxcy_sys_read);

  unsigned long ptr;
  for (ptr = 0xc0000000; ptr <= 0xd0000000; ptr++) {
    fxcy_sys_call_table = (unsigned long **) ptr;
    if (fxcy_sys_call_table[__NR_read] == (unsigned long *) fxcy_sys_read) {
      fxcy_sys_call_table = &(fxcy_sys_call_table[0]);
      break;
    }
  }

  pr_info("fuxsocy: sys_call_table: %p\n", fxcy_sys_call_table);
  pr_info("fuxsocy: sys_read: %p\n", fxcy_sys_call_table[__NR_read]);
}

module_init(fuxsocy_init);
module_exit(fuxsocy_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.01");
