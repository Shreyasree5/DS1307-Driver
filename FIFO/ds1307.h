#include<stdio.h>
#include<linux/i2c-dev.h>
#include<stdint.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include <fcntl.h>

#define DS1307_ADDRESS 0x68

struct date_time_to_set {
	int seconds;
	int minutes;
	int hours;
	int day;
	int date;
	int month;
	int year;
	int type;
};

int getTime(struct date_time_to_set *dtts);
int setTime(struct date_time_to_set *dtts);