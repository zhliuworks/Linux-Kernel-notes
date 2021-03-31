#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#define BUFSIZE 100

MODULE_LICENSE("GPL");

static struct proc_dir_entry* entry;

static ssize_t myread(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	char buf[BUFSIZE];
	int len = 0;
	if(*ppos > 0 || count < BUFSIZE)
		return 0;
	len += sprintf(buf, "mod3: %s\n", "hello world");
	
	if(copy_to_user(ubuf, buf, len))
		return -EFAULT;
	*ppos = len;
	return len;
}

static struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread
};

static int __init mod3_init(void) {	
	entry = proc_create("mod3_proc", 0444, NULL, &myops);
	return 0;
}

static void __exit mod3_exit(void) {
	proc_remove(entry);
}

module_init(mod3_init);
module_exit(mod3_exit);
