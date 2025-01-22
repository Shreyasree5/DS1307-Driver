#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/cdev.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shreya Sree Ravutla");
MODULE_DESCRIPTION("RTC Driver with IOCTL support");

struct date_time_to_set {
    int seconds, hours, minutes, day, date, month, year;
};
int decimal_to_bcd(int n){
	return ( ((n/10)<<4) | (n%10) );
}
int bcd_to_decimal(uint8_t n){
	return ( ((n>>4)*10) + (n & 0x0F));
}

#define IOCTL_MAGIC 'k'
#define IOCTL_GET_TIME _IOR(IOCTL_MAGIC, 1, struct date_time_to_set)
#define IOCTL_SET_TIME _IOW(IOCTL_MAGIC, 2, struct date_time_to_set)

#define DEVICE_NAME "my_ioctl_device"
#define I2C_BUS_NUMBER 1       // I2C bus number (e.g., /dev/i2c-1)
#define I2C_SLAVE_ADDR 0x68    // I2C device address

static struct i2c_client *rtc_client;

//ioctl handler
static long rtc_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    struct date_time_to_set dtts;
    char data[8];
    int ret;

    if (!rtc_client) {
        pr_err("RTC client not initialized\n");
        return -ENODEV;
    }

    switch (cmd) {
        case IOCTL_SET_TIME:
            if (copy_from_user(&dtts, (struct date_time_to_set *)arg, sizeof(dtts))){
                pr_err("Failed to copy data from user\n");
                return -EFAULT;
            }
            pr_info("Setting RTC time: %02d:%02d:%02d %02d/%02d/%02d\n", dtts.hours, dtts.minutes, dtts.seconds, dtts.date, dtts.month, dtts.year);

            // Convert time to RTC register format and write to device
            data[0] = 0x00;
            data[1] = decimal_to_bcd(dtts.seconds);
            data[2] = decimal_to_bcd(dtts.minutes);
            data[3] = decimal_to_bcd(dtts.hours);
            data[4] = decimal_to_bcd(dtts.day);
            data[5] = decimal_to_bcd(dtts.date);
            data[6] = decimal_to_bcd(dtts.month);
            data[7] = decimal_to_bcd(dtts.year);

            ret = i2c_master_send(rtc_client, data, sizeof(data));
            if (ret < 0){
                pr_err("Failed to write to RTC\n");
                return -EIO;
            } 
            pr_info("RTC time set successfully\n");
            break;

        case IOCTL_GET_TIME:
            data[0] = 0x00; 
            if (i2c_master_send(rtc_client, data, 1) < 0) {
                pr_err("Failed to request RTC time\n");
                return -EIO;
            }

            // Read time from RTC
            ret = i2c_master_recv(rtc_client, data, sizeof(data));
            if (ret < 0) {
                pr_err("Failed to read from RTC\n");
                return ret;
            }

            // Convert RTC register data to struct format
            dtts.seconds = bcd_to_decimal(data[0]);
            dtts.minutes = bcd_to_decimal(data[1]);
            dtts.hours = bcd_to_decimal(data[2]);
            dtts.day = bcd_to_decimal(data[3]);
            dtts.date = bcd_to_decimal(data[4]);
            dtts.month = bcd_to_decimal(data[5]);
            dtts.year = bcd_to_decimal(data[6]);

            if (copy_to_user((struct date_time_to_set *)arg, &dtts, sizeof(dtts))) {
                pr_err("Failed to copy data to user\n");
                return -EFAULT;
            }

            pr_info("RTC time read: %02d:%02d:%02d %02d/%02d/%02d\n", dtts.hours, dtts.minutes, dtts.seconds, dtts.date, dtts.month, dtts.year);
            break;

        default:
            pr_err("Invalid IOCTL command\n");
            return -EINVAL;
    }
    return 0;
}

// File operations
static const struct file_operations fops = {
    .unlocked_ioctl = rtc_ioctl,
};

// Module initialization
static int __init rtc_driver_init(void) {
    struct i2c_adapter *adapter;
    struct i2c_board_info info = {
        I2C_BOARD_INFO("rtc-ds1307", I2C_SLAVE_ADDR),
    };

    adapter = i2c_get_adapter(I2C_BUS_NUMBER); 
    if (!adapter) {
        pr_err("Failed to get I²C adapter\n");
        return -ENODEV;
    }

    rtc_client = i2c_new_client_device(adapter, &info);
    i2c_put_adapter(adapter);
    if (!rtc_client) {
        pr_err("Failed to register I²C client\n");
        return -ENODEV;
    }

    int major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    printk("RTC driver loaded with major number %d\n", major);
    return 0;
}

// Module exit
static void __exit rtc_driver_exit(void) {
    unregister_chrdev(0, DEVICE_NAME);
    i2c_unregister_device(rtc_client);
    printk(DEVICE_NAME ": RTC driver unloaded\n");
}

module_init(rtc_driver_init);
module_exit(rtc_driver_exit);

