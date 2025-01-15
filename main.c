#include<stdio.h>
#include<linux/i2c-dev.h>
#include<stdint.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include <fcntl.h>

#define DS1307_ADDRESS 0x68

struct date_time_to_set{
	int seconds, minutes, hours, day, date, month, year;
};
void enterData(struct date_time_to_set *dtts){
		printf("Enter date, time, day to set:\n");
		printf("Seconds:");
		scanf("%d", &dtts->seconds);
		printf("Minutes:");
		scanf("%d", &dtts->minutes);
		printf("Hours:");
		scanf("%d", &dtts->hours);
		printf("Day:");
		scanf("%d", &dtts->day);
		printf("Date:");
		scanf("%d", &dtts->date);
		printf("Month:");
		scanf("%d", &dtts->month);
		printf("Year:");
		scanf("%d", &dtts->year);
}
int isValid(struct date_time_to_set dtts){
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
		printf("Date and time set successfully!\n");
		close(fd);
		return 0;
	} else{
		printf("Validation failed\n");
		return -1;
	}
}
int getTime(struct date_time_to_set *dtts){
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
	printf("data at 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	dtts->seconds = bcd_to_decimal(data[2]);
	dtts->minutes = bcd_to_decimal(data[1]);
	dtts->hours = bcd_to_decimal(data[0]);
	dtts->day = bcd_to_decimal(data[3]);
	dtts->date = bcd_to_decimal(data[4]);
	dtts->month = bcd_to_decimal(data[5]);
	dtts->year = bcd_to_decimal(data[6]);
	
	printf("Current Date and Time:\n");
    printf("Time: %d:%d:%d\n", dtts->hours, dtts->minutes, dtts->seconds);
    printf("Date: %d/%d/%d\n", dtts->date, dtts->month, dtts->year);
    printf("Day: %d\n", dtts->day);
	close(fd);
	return 1;
}
void main(){
	struct date_time_to_set dtts;
	int n;
	
	while(1){
		printf("Choose 1.setTime 2.getTime 3.Exit\n");
		scanf("%d", &n);
		switch(n){
			case 1:
				enterData(&dtts);
				if(setTime(&dtts) != 0){
					printf("Failed to set Date and Time\n");
				}
				break;
			case 2: 
				if (!getTime(&dtts)) {
                    printf("Failed to retrieve time.\n");
                }
				break;
			case 3:
				printf("Exiting..");
				return;
			default:
				printf("Invalid choice, Try again!\n");
				break;
		}
	}	
}
