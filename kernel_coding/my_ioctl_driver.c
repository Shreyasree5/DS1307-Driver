#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("IOCTL Example for Raspberry Pi");

struct date_time_to_set {
	int seconds;
	int minutes;
	int hours;
	int day;
	int date;
	int month;
	int year;
};

#define MY_MAGIC 'M'
#define IOCTL_CMD_1 _IOW(MY_MAGIC, 1, struct date_time_to_set)
#define IOCTL_CMD_2 _IOR(MY_MAGIC, 2, struct date_time_to_set)

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct date_time_to_set dtts;

    switch (cmd) {
    case IOCTL_CMD_1:
        printk(KERN_INFO "Processing IOCTL_CMD_1\n");
        if (copy_from_user(&dtts, (struct date_time_to_set *)arg, sizeof(dtts)))
            return -EFAULT;
        printk(KERN_INFO "Received from user: seconds=%d, minutes=%d, hours=%d, day=%d, date=%d, month=%d, year=%d\n",
               dtts.seconds, dtts.minutes, dtts.hours, dtts.day, dtts.date, dtts.month, dtts.year);
        break;

    case IOCTL_CMD_2:
        printk(KERN_INFO "Processing IOCTL_CMD_2\n");
        memset(&dtts, 0, sizeof(dtts));
        dtts.seconds = 7;
        dtts.hours = 7;
        dtts.minutes = 7;
        dtts.day = 7;
        dtts.date = 7;
        dtts.month = 7;
        dtts.year = 7;
        if (copy_to_user((struct date_time_to_set *)arg, &dtts, sizeof(dtts)))
            return -EFAULT;
        printk(KERN_INFO "Sent date/time structure to user space\n");
        break;

    default:
        return -EINVAL;
    }
    return 0;
}

static const struct file_operations fops = {
    .unlocked_ioctl = my_ioctl,
};

static int __init my_module_init(void) {
    int major = register_chrdev(0, "my_ioctl_device", &fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register character device\n");
        return major;
    }
    printk(KERN_INFO "IOCTL Module loaded with major number %d\n", major);
    return 0;
}

static void __exit my_module_exit(void) {
    unregister_chrdev(0, "my_ioctl_device");
    printk(KERN_INFO "IOCTL Module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

