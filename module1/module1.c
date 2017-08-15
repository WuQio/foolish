#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
static int hello_init(void)
{
	struct task_struct *p;
	printk(KERN_ALERT"程序名\t\tPID号\t进程状态  优先级\n");
	for_each_process(p)
	{
		printk(KERN_ALERT"%d\t%d\t    %d\t   %d\n", p->comm, p->pid, p->state, p->prio);
	}
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT"module1exit!\n");
}

MODULE_AUTHOR("Wu Qiong");
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
