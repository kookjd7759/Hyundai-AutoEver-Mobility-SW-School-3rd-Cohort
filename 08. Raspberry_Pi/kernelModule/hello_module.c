#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

int __init my_init_module(void){
    printk(KERN_ALERT "Hello module\n");
    return 0;
}

void __exit my_cleanup_module(void){
    printk(KERN_ALERT "Goodbye module");
}

module_init(my_init_module);
module_exit(my_cleanup_module);
MODULE_LICENSE("GPL");