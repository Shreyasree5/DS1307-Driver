#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>

struct date_time_to_set {
    int seconds, hours, minutes, day, date, month, year;
};

#define DEVICE "/dev/my_ioctl_device"
#define IOCTL_MAGIC 'k'
#define IOCTL_GET_TIME  _IOR(IOCTL_MAGIC, 1, struct date_time_to_set)
#define IOCTL_SET_TIME _IOW(IOCTL_MAGIC, 2, struct date_time_to_set)

int main() {
    int fd;
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    struct date_time_to_set set_time = {1, 2, 3, 4, 5, 6, 7};
   
    if (ioctl(fd, IOCTL_SET_TIME, &set_time) < 0) {
        perror("Failed to write to register");
        close(fd);
        return -1;
    }
    printf("RTC time set successfully\n");

    struct date_time_to_set get_time;
    if (ioctl(fd, IOCTL_GET_TIME, &get_time) < 0) {
        perror("Failed to read from register");
        close(fd);
        return -1;
    } 
    printf("RTC Current Time: %02d:%02d:%02d, Date: %02d/%02d/%02d\n",get_time.hours, get_time.minutes, get_time.seconds, get_time.date, get_time.month, get_time.year);

    close(fd);
    return 0;
}
