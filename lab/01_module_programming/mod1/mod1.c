#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init mod1_init(void) {
	printk(KERN_INFO "Insert Module 1\n");
	return 0;
}

static void __exit mod1_exit(void) {
	printk(KERN_INFO "Remove Module 1\n");
}

module_init(mod1_init);
module_exit(mod1_exit);
