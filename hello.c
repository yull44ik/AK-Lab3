#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Yulia Mikhaylenko <mikhaylenko.yulia@lll.kpi.ua>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int h_count = 1;
// Access for all users to read-only the parameter h_count
module_param(h_count, uint, 0444);
MODULE_PARM_DESC(h_count, "How many times to display 'Hello, world!'");

struct node
{
	struct list_head list_node;
	ktime_t timestamp;
};

static LIST_HEAD(Head_Node);

static int __init hello_init(void)
{
	int i;

	if (h_count == 0 || (h_count >= 5 && h_count <= 10))
	{
		printk(KERN_WARNING "Risky value of parameter.\n");
	}
	else if (h_count > 10)
	{
		printk(KERN_ERR "Wrong number of hello. This module will not be loaded.\n");
		return -EINVAL;
	}

	for (i = 0; i < h_count; i++)
	{
		struct node *linkedlist = kmalloc(sizeof(*linkedlist), GFP_KERNEL);

		linkedlist->timestamp = ktime_get();
		list_add(&linkedlist->list_node, &Head_Node);
		printk(KERN_EMERG "Hello, world!\n");
	}
	return 0;
}

static void __exit hello_exit(void)
{
	struct node *linkedlist, *tmp;

	// Safe is not really needed here
	list_for_each_entry_safe(linkedlist, tmp, &Head_Node, list_node)
	{
		ktime_t duration = ktime_sub(ktime_get(), linkedlist->timestamp);
        
		printk(KERN_INFO "Elapsed time: %lld\n", ktime_to_ns(duration));
		list_del(&linkedlist->list_node);
		kfree(linkedlist);
	}
}

module_init(hello_init);
module_exit(hello_exit);