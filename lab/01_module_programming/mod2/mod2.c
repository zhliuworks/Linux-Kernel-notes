#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

static int a;
static char* b;
static int n_para = 1; // 4 parameters
static int c[4]; // 4 parameters

module_param(a, int, 0644);
module_param(b, charp, 0644);
module_param_array(c, int, &n_para, 0644);

static int __init mod2_init(void) {
	printk(KERN_INFO "Insert Module 2:\na = %d, b = %s, c = [%d, %d, %d, %d]\n", a, b, c[0], c[1], c[2], c[3]);
	return 0;
}

static void __exit mod2_exit(void) {
	printk(KERN_INFO "Remove Module 2\n");
}

module_init(mod2_init);
module_exit(mod2_exit);
