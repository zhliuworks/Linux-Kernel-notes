#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zihan Liu");
MODULE_DESCRIPTION("a LKM to hook sys_clone");

typedef void (*sys_call_ptr_t)(void);
typedef asmlinkage long (*sys_clone_t)(unsigned long, unsigned long, int __user *,
                                       int __user *, unsigned long);

sys_call_ptr_t *syscall_table = NULL;
sys_clone_t orig_clone = NULL;
unsigned int level;
pte_t *pte;


asmlinkage long hooked_sys_clone(unsigned long x1, unsigned long x2, int __user *x3,
                                 int __user *x4, unsigned long x5) {
	long ret_val = orig_clone(x1, x2, x3, x4, x5);
	printk(KERN_INFO "hello, I have hacked this syscall");
	return ret_val;
}


static int __init sys_clone_hook_init(void) {
	syscall_table = (sys_call_ptr_t *)kallsyms_lookup_name("sys_call_table");

	// save the original syscall handler
	orig_clone = (sys_clone_t)syscall_table[__NR_clone];

	// unprotect syscall_table memory page
	pte = lookup_address((unsigned long)syscall_table, &level);

	// change PTE to allow writing
	set_pte_atomic(pte, pte_mkwrite(*pte));

	// overwrite the __NR_clone entry with address to our hook
	syscall_table[__NR_clone] = (sys_call_ptr_t)hooked_sys_clone;

	// reprotect page
	set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));

	printk(KERN_INFO "installed sys_clone_hook");
	return 0;
}


static void __exit sys_clone_hook_exit(void) {
	// change PTE to allow writing
	set_pte_atomic(pte, pte_mkwrite(*pte));

	// restore syscall_table to the original state
	syscall_table[__NR_clone] = (sys_call_ptr_t)orig_clone;

	// reprotect page
	set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));

	printk(KERN_INFO "uninstalled sys_clone_hook");
}


module_init(sys_clone_hook_init);
module_exit(sys_clone_hook_exit);
