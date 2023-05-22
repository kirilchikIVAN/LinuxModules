#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/timer.h>

#define KEYBOARD_IRQ 1
#define TIME_PERIOD 60000

static int tap_count = 0;

irqreturn_t keyboard_handler(int irq, void *dev_id) {
    tap_count++;
    return IRQ_NONE;
}

static void timer_callback(struct timer_list *timer) {
    printk(KERN_INFO "Taps in the last minute: %u\n", tap_count);
    tap_count = 0;
    mod_timer(timer, jiffies + msecs_to_jiffies(TIME_PERIOD));
}

static struct timer_list my_timer;

static int __init my_module_init(void) {
    if (request_irq(
            KEYBOARD_IRQ,
            keyboard_handler,
            IRQF_SHARED,
            "keyboard",
            (void *)(keyboard_handler)
    )) {
        printk(KERN_ALERT "Failed to register keyboard interrupt handler\n");
        return -1;
    }

    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIME_PERIOD));

    printk(KERN_INFO "Module loaded\n");
    return 0;
}

static void __exit my_module_exit(void) {
    del_timer(&my_timer);
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_handler));

    printk(KERN_INFO "Module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Keyboard statistics module");
