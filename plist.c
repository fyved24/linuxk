#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/sched/task.h>
#include <linux/kallsyms.h>
#include <linux/unistd.h>
#include <linux/time.h>
#include <asm/uaccess.h>

#define __NR_syscall 335 // 251 is the syscall number
// #define sys_call_table_addr = 0xffffffff9fe00300
unsigned long *sys_call_table;

unsigned int clear_and_return_cr0(void);
void setback_cr0(unsigned int val);
static int print_pid(void);

int orig_cr0; // original cr0 value
unsigned long *sys_call_table = 0;
static int (*anything_saved)(void); // this is a function pointer
/*
 * 设置cr0寄存器的第17位为0，即可以写入cr0寄存器
 */

unsigned int clear_and_return_cr0(void)
{
	unsigned int cr0 = 0;
	unsigned int ret;
	/* 前者用在32位系统。后者用在64位系统，本系统64位 */
	// asm volatile ("movl %%cr0, %%eax" : "=a"(cr0));
	asm volatile("movq %%cr0, %%rax"
				 : "=a"(cr0)); /* 将cr0寄存器的值移动到rax寄存器中，同时输出到cr0变量中 */
	ret = cr0;
	cr0 &= 0xfffeffff; /* 将cr0变量值中的第17位清0，将修改后的值写入cr0寄存器 */
	// asm volatile ("movl %%eax, %%cr0" :: "a"(cr0));
	asm volatile("movq %%rax, %%cr0" ::"a"(cr0)); /* 读取cr0的值到rax寄存器，再将rax寄存器的值放入cr0中 */
	return ret;
}

/* 读取val的值到rax寄存器，再将rax寄存器的值放入cr0中 */
void setback_cr0(unsigned int val)
{

	// asm volatile ("movl %%eax, %%cr0" :: "a"(val));
	asm volatile("movq %%rax, %%cr0" ::"a"(val));
}

static void print_process_tree(struct task_struct *p, unsigned int level)
{
	int i = 0;
	for (; i < level; i++)
	{
		printk(KERN_CONT "| ");
	}
	if (level > 0)
	{
		printk(KERN_CONT "|--");
	}
	printk(KERN_CONT "%d\n", p->pid);
	struct list_head *l;

	for (l = p->children.next; l != &(p->children); l = l->next)
	{
		struct task_struct *t = list_entry(l, struct task_struct, sibling);
		print_process_tree(t, level + 1);
	}
}
/**
 * @brief
 *
 * @return int
 */

static int print_pid(void)

{
	struct task_struct *task, *p;
	struct list_head *pos;
	task = &init_task;
	print_process_tree(task, 0);
	return 0;
}

static int __init add_syscall_init(void)
{
	printk("load plist module\n");
	sys_call_table = 0xffffffff9fe00300; /* 获取系统调用服务首地址 */
	// sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table"); /* 获取系统调用服务首地址 */

	printk("sys_call_table: 0x%p\n", sys_call_table);
	anything_saved = (int (*)(void))(sys_call_table[__NR_syscall]); /* 保存原始系统调用 */
	orig_cr0 = clear_and_return_cr0();								/* 设置cr0可更改 */
	sys_call_table[__NR_syscall] = (unsigned long)&print_pid;		/* 更改原始的系统调用服务地址 */
	setback_cr0(orig_cr0);											/* 设置为原始的只读cr0 */
	return 0;
}

static void __exit add_syscall_cleanup(void)
{
	printk("unload plist module\n");
	orig_cr0 = clear_and_return_cr0();							  /* 设置cr0中对sys_call_table的更改权限 */
	sys_call_table[__NR_syscall] = (unsigned long)anything_saved; /* 设置cr0可更改 */
	setback_cr0(orig_cr0);										  /* 恢复原有的中断向量表中的函数指针的值 */
}

module_init(add_syscall_init);
module_exit(add_syscall_cleanup);
MODULE_LICENSE("GPL");