#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#define BUFSIZE 100
#define MAXSIZE 1024 

MODULE_LICENSE("GPL");

static char str[MAXSIZE];

static struct proc_dir_entry *entry;
static struct proc_dir_entry *base;
 
static ssize_t myread(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	char buf[BUFSIZE];
	int len = 0;
	if(*ppos > 0 || count < BUFSIZE)
		return 0;
	len += sprintf(buf, "mod4: %s\n", str);
	printk(KERN_INFO "read from proc file: %s", str);
	
	if(copy_to_user(ubuf, buf, len))
		return -EFAULT;
	*ppos = len;
	return len;
}

static ssize_t mywrite(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) 
{
	char buf[BUFSIZE];
	if(*ppos > 0 || count > BUFSIZE)
		return -EFAULT;
	if(copy_from_user(buf, ubuf, count))
		return -EFAULT;
	char tmp[MAXSIZE];
	sscanf(buf, "%s", tmp);
	strcpy(str, tmp);
	printk(KERN_INFO "write (%s) to proc file", str);
	int c = strlen(buf);
	*ppos = c;
	return c;
}
 
static struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite
};

static int __init mod4_init(void) {	
	base = proc_mkdir("proc4_folder", NULL);
	entry = proc_create("mod4_proc", 0666, base, &myops);
	return 0;
}

static void __exit mod4_exit(void) {
	proc_remove(entry);
	proc_remove(base);
}

module_init(mod4_init);
module_exit(mod4_exit);
