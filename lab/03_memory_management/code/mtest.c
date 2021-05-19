/*
 *  mtest.c  --  memory management lab `mtest` module
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/mm_types.h>
#include <linux/mm.h>

#define BUFSIZE 1024

static struct proc_dir_entry *mtest_proc_entry;


/* Print all vma of the current process */
static void mtest_list_vma(void) {
    struct mm_struct *mm = current->mm;
    struct vm_area_struct *vma = mm->mmap;

    down_read(&(mm->mmap_sem)); // lock the read critical section
    
    // traverse list of VMAs
    while (vma) {
        printk("VMA 0x%lx - 0x%lx\n", vma->vm_start, vma->vm_end);
        // permission flags in `mm.h`
        (vma->vm_flags & VM_READ) ? printk("r\n") : printk("-\n");
        (vma->vm_flags & VM_WRITE) ? printk("w\n") : printk("-\n");
        (vma->vm_flags & VM_EXEC) ? printk("x\n") : printk("-\n");
        printk("\n");
        vma = vma->vm_next;
    }

    up_read(&(mm->mmap_sem)); // unlock the read critical section
}


/* find page of va */
static struct page *_find_page(unsigned long vaddr) {
    struct mm_struct *mm = current->mm;
    struct page *curr_page;

    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    // walk the page table
    // 1. get [page global directory, pgd]
    pgd = pgd_offset(mm, vaddr);
        // printk("pgd: %llx\n", pgd_val(*pgd));
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        printk("[pgd] not available\n");
        return NULL;
    }
    // 2. get [page upper directory, pud]
    pud = pud_offset(pgd, vaddr);
        // printk("pud: %llx\n", pud_val(*pud));
    if (pud_none(*pud) || pud_bad(*pud)) {
        printk("[pud] not available\n");
        return NULL;
    }
    // 3. get [page middle directory, pmd]
    pmd = pmd_offset(pud, vaddr);
        // printk("pmd: %llx\n", pmd_val(*pmd));
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        printk("[pmd] not available\n");
        return NULL;
    }
    // 4. get [page table entry, pte]
    pte = pte_offset_kernel(pmd, vaddr);
        // printk("pte: %llx\n", pte_val(*pte));
    if (pte_none(*pte)) {
        printk("[pte] not available\n");
        return NULL;
    }
    
    curr_page = pte_page(*pte);
    return curr_page;
}


/* Find va->pa translation */
static void mtest_find_page(unsigned long vaddr) {
    unsigned long paddr;
    unsigned long page_addr;
    unsigned long page_offset;

    // get current page of vaddr
    struct page *curr_page = _find_page(vaddr);

    if (!curr_page) {
        printk("translation not found\n");
        return;
    }

    page_addr = page_to_phys(curr_page) & PAGE_MASK;
    page_offset = vaddr & (~PAGE_MASK);
    paddr = page_addr | page_offset;

    printk("vma 0x%lx -> pma 0x%lx\n", vaddr, paddr);
}


/* Write val to the specified address */
static void mtest_write_val(unsigned long vaddr, unsigned long val) {
    // look up the first VMA which statisfies vaddr < vm_end, NULL if none
    struct vm_area_struct *vma = find_vma(current->mm, vaddr);
    // get current page of vaddr
    struct page *curr_page = _find_page(vaddr);

    // whether the page is existed
    if (!curr_page) {
        printk("unexisted page\n");
        return;
    }

    // whether the vma is valid
    if (!vma || vma->vm_start > vaddr) {
        printk("invalid vma\n");
        return;
    }

    // whether the page is writable
    if (!(vma->vm_flags & VM_WRITE)) {
        printk("unwritable page\n");
        return;
    }

    // write value
    unsigned long *kernel_addr;
    kernel_addr = (unsigned long*)page_address(curr_page);
    kernel_addr += vaddr & (~PAGE_MASK);
    *kernel_addr = val;
    printk("written 0x%lx to address 0x%lx\n", val, (unsigned long)kernel_addr);
}


/* proc write interface */
static ssize_t mtest_proc_write(struct file *file,
                                const char __user *ubuf,
                                size_t count,
                                loff_t *ppos) {
    char buf[BUFSIZE];
    char data[BUFSIZE];
    unsigned long addr, val;
    unsigned short offset;

    if (*ppos > 0 || count > BUFSIZE)
        return -EFAULT;
    if (copy_from_user(buf, ubuf, count))
        return -EFAULT;
    sscanf(buf, "%s", data);

    if (!strcmp(data, "listvma")) {
        /* listvma */
        mtest_list_vma();
    } else if (!strcmp(data, "findpage")) {
        /* findpage */
        offset = 9;
        sscanf(buf + offset, "%s", data);
        kstrtoul(data, 16, &addr);
        mtest_find_page(addr);
    } else if (!strcmp(data, "writeval")) {
        /* writeval */
        offset = 9;
        sscanf(buf + offset, "%s", data);
        kstrtoul(data, 16, &addr);
        while (*(buf + offset) != ' ') offset ++;
        offset ++;
        sscanf(buf + offset, "%s", data);
        kstrtoul(data, 16, &val);
        mtest_write_val(addr, val);        
    }

    *ppos = strlen(buf);
    return *ppos;
}


/* proc file_operations struct */
static struct file_operations proc_mtest_operations = {
    .owner = THIS_MODULE,
    .write = mtest_proc_write
};


static int __init mtest_init(void) {
    mtest_proc_entry = proc_create("mtest", 0666, NULL, &proc_mtest_operations);
    return 0;
}


static void __exit mtest_exit(void) {
    proc_remove(mtest_proc_entry);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Memory Management Lab Test Module");
MODULE_AUTHOR("Zihan Liu");

module_init(mtest_init);
module_exit(mtest_exit);