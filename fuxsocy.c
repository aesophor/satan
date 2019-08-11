#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Marco Wang");
MODULE_DESCRIPTION("fuxsocy module");
MODULE_VERSION("0.01");

static int __init fuxsocy_init(void) {
  printk(KERN_INFO "fuxsocy.ko: Hello friend\n");
  return 0;
}
static void __exit fuxsocy_exit(void) {
  printk(KERN_INFO "fuxsocy.ko: Goodbye friend\n");
}

module_init(fuxsocy_init);
module_exit(fuxsocy_exit);
