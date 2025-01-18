#include "ds1307.h"

static int isValid(struct date_time_to_set dtts){
	if(dtts.seconds<0 || dtts.seconds>59){
		printf("Seconds not in range\n");
		return 0;
	}
	if(dtts.minutes<0 || dtts.minutes>59){
		printf("Minutues not in range");
		return 0;
	}
	if(dtts.hours<1 || dtts.hours>23){
		printf("Hours not in range");
		return 0;
	}
	if(dtts.day<1 || dtts.day>7){ 
		printf("Day not in range");
		return 0;
	}
	if(dtts.date<1 || dtts.date>31){ 
		printf("Date not in range");
		return 0;
	}
	if(dtts.month<1 || dtts.month>12){ 
		printf("Month not in range");
		return 0;
	}
	if(dtts.year<0 || dtts.year>99){ 
		printf("Year not in range");
		return 0;
	}
	return 1;
}
int decimal_to_bcd(int n){
	return ( ((n/10)<<4) | (n%10) );
}
int bcd_to_decimal(uint8_t n){
	return ( ((n>>4)*10) + (n & 0x0F));
}
int setTime(struct date_time_to_set *dtts){
	if(isValid(*dtts)){
		printf("Validation successful!\n");
		int fd = open("/dev/i2c-1", O_RDWR);
		if(fd<0){
			printf("Failed to open i2c device");
			return -1;
		}
		if(ioctl(fd, I2C_SLAVE, DS1307_ADDRESS)<0){
			printf("Couldn't find device on address");
			close(fd);
			return -1;
		}
		uint8_t data[8] = {0};
		data[0] = 0x00;
		data[1] = decimal_to_bcd(dtts->seconds);
		data[2] = decimal_to_bcd(dtts->minutes);
		data[3] = decimal_to_bcd(dtts->hours);
		data[4] = decimal_to_bcd(dtts->day);
		data[5] = decimal_to_bcd(dtts->date);
		data[6] = decimal_to_bcd(dtts->month);
		data[7] = decimal_to_bcd(dtts->year);

		if (write(fd, data, 8) != 8) {
			printf("Failed to write to RTC");
			close(fd);
			return -1;
    	}
		close(fd);
		return 0;
	} else{
		printf("Validation failed\n");
		return -1;
	}
}
int getTime(struct date_time_to_set *dtts){
	printf("in gettime\n");
	int fd = open("/dev/i2c-1", O_RDWR);
	if(fd<0){
		printf("Failed to open i2c device");
		return -1;
	}
	if(ioctl(fd, I2C_SLAVE, DS1307_ADDRESS)<0){
		printf("Couldn't find device on address");
		close(fd);
		return -1;
	}

    uint8_t reg = 0x00;
    if (write(fd, &reg, 1) != 1) {
        printf("Failed to write to DS1307");
        close(fd);
        return -1;
    }

	uint8_t data[7] = {0};
	if(read(fd, data, 7)!=7){
		printf("Failed to read from RTC");
		close(fd);
		return -1;
	}
	dtts->seconds = bcd_to_decimal(data[2]);
	dtts->minutes = bcd_to_decimal(data[1]);
	dtts->hours = bcd_to_decimal(data[0]);
	dtts->day = bcd_to_decimal(data[3]);
	dtts->date = bcd_to_decimal(data[4]);
	dtts->month = bcd_to_decimal(data[5]);
	dtts->year = bcd_to_decimal(data[6]);
	close(fd);
	return 0;
}
