#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>

void traverse_dfs(struct task_struct *task) {
    struct list_head *child_list;
	struct task_struct *child;

	printk(KERN_INFO "%-20s | %5d %5d\n", task->comm, task->pid, task->parent->pid);

	list_for_each (child_list, &task->children) {
		child = list_entry(child_list, struct task_struct, sibling);

		traverse_dfs(child);
	}
}

void traverse_bfs(struct task_struct *task) {
    int is_root_printed = 0;
	struct task_struct *child;
	struct list_head *child_list;

	for_each_process (task) {
		if (is_root_printed == 0) {
			printk(KERN_INFO " %-20s | %5d %5d\n", task->parent->comm, task->parent->pid, 0);
			
			list_for_each(child_list, &task->parent->children) {
				child = list_entry(child_list, struct task_struct, sibling);

				printk(KERN_INFO "%-20s | %5d %5d\n", child->comm, child->pid, child->parent->pid);
			}

			is_root_printed = 1;
		}
		
		list_for_each(child_list, &task->children) {
			child = list_entry(child_list, struct task_struct, sibling);

			printk(KERN_INFO " %-20s | %5d %5d\n", child->comm, child->pid, child->parent->pid);
		}
	}
}

int print_processes_sortedby_dfs(void) {
	printk(KERN_INFO "Loading Module\n");
	printk(KERN_INFO "\n");
	printk(KERN_INFO "Processes sorted by depth-first search\n");
	printk(KERN_INFO " CMD                  | PID   PPID\n");
	printk(KERN_INFO "----------------------+------------\n");
	traverse_dfs(&init_task);
	printk(KERN_INFO "\n");
	
	return 0;
}

void print_processes_sortedby_bfs(void) {
	printk(KERN_INFO "Processes sorted by breadth-first search\n");
	printk(KERN_INFO " CMD                  | PID   PPID\n");
	printk(KERN_INFO "----------------------+------------\n");
	traverse_bfs(&init_task);
	printk(KERN_INFO "\n");
	printk(KERN_INFO "Removing Module\n");
}

module_init(print_processes_sortedby_dfs);
module_exit(print_processes_sortedby_bfs);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Homework 1 - Part 1. Linux Kernel Module for Listing Tasks");
MODULE_AUTHOR("Junyeong Yoon 201723300");
