#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>

void traverse_depthfirst(struct task_struct *task) {
	struct task_struct *child;
	struct list_head *listChildren;

	printk(KERN_INFO "%-20s | %5d %5d\n", task->comm, task->pid, task->parent->pid);

	list_for_each(listChildren, &task->children) {
		child = list_entry(listChildren, struct task_struct, sibling);
		traverse_depthfirst(child);
	}
}

void traverse_breadthfirst(struct task_struct *task) {
	struct task_struct *child;
	struct list_head *listChildren;
	int printRoot = 1;

	for_each_process(task) {
		if (printRoot == 1) {
			printk(KERN_INFO "%-20s | %5d %5d\n", task->parent->comm, task->parent->pid, 0);
			
			list_for_each(listChildren, &task->parent->children) {
				child = list_entry(listChildren, struct task_struct, sibling);
				printk(KERN_INFO "%-20s | %5d %5d\n", child->comm, child->pid, child->parent->pid);
			}

			printRoot = 0;
		}
		
		list_for_each(listChildren, &task->children) {
			child = list_entry(listChildren, struct task_struct, sibling);
			printk(KERN_INFO "%-20s | %5d %5d\n", child->comm, child->pid, child->parent->pid);
		}
	}
}

int listingtasks_init(void) {
	printk(KERN_INFO "Loading Module\n");
	printk(KERN_INFO "\n");
	printk(KERN_INFO "Process sorted by depth-first search\n");
	printk(KERN_INFO "CMD                  | PID   PPID\n");
	printk(KERN_INFO "---------------------+------------\n");

	traverse_depthfirst(&init_task);
	printk(KERN_INFO "\n");
	
	return 0;
}

void listingtasks_exit(void) {
	printk(KERN_INFO "Process sorted by breadth-first search\n");
	printk(KERN_INFO "CMD                  | PID   PPID\n");
	printk(KERN_INFO "---------------------+------------\n");

	traverse_breadthfirst(&init_task);
	printk(KERN_INFO "\n");

	printk(KERN_INFO "Removing Module\n");
}

module_init(listingtasks_init);
module_exit(listingtasks_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HW1 - Pt.1 Linux kernel module for listing tasks");
MODULE_AUTHOR("Junyeong Yoon 201723300");
