#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

unsigned long **real_sys_call_table;


static unsigned long **get_sys_call_table(void)
{
  unsigned long **entry = (unsigned long **) PAGE_OFFSET;
  for (; (unsigned long)entry < ULONG_MAX; entry += 1) {
    if (entry[__NR_close] == (unsigned long *) ksys_close)
      return entry;
  }
  return NULL;
}

static int __init fuxsocy_init(void) {
  printk(KERN_INFO "fuxsocy.ko: Hello friend\n");
  real_sys_call_table = get_sys_call_table();
  printk("PAGE_OFFSET = %lx\n", PAGE_OFFSET);
  printk("sys_call_table = %p\n", real_sys_call_table);
  return 0;
}

static void __exit fuxsocy_exit(void) {
  printk(KERN_INFO "fuxsocy.ko: Goodbye friend\n");
}



MODULE_LICENSE("MIT");
MODULE_AUTHOR("Marco Wang");
MODULE_DESCRIPTION("fuxsocy module");
MODULE_VERSION("0.01");


module_init(fuxsocy_init);
module_exit(fuxsocy_exit);
