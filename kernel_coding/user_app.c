#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

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

int main() {
    int fd = open("/dev/my_ioctl_device", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    struct date_time_to_set dtts = {10, 12, 9, 3, 9, 6, 96};

    if (ioctl(fd, IOCTL_CMD_1, &dtts) < 0) {
        perror("IOCTL_CMD_1 failed");
        close(fd);
        return 1;
    }

    struct date_time_to_set dtts_rcv;
    if (ioctl(fd, IOCTL_CMD_2, &dtts_rcv) < 0) {
        perror("IOCTL_CMD_2 failed");
        close(fd);
        return 1;
    }

    printf("Message from kernel: \n");
    printf("Current Date and Time:\n");
    printf("Time: %02d:%02d:%02d\n", dtts_rcv.hours, dtts_rcv.minutes, dtts_rcv.seconds);
    printf("Date: %02d/%02d/%02d\n", dtts_rcv.date, dtts_rcv.month, dtts_rcv.year);
    printf("Day: %d\n", dtts_rcv.day);
    close(fd);
    return 0;
}
