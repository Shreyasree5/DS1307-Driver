#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include"ds1307.h"

void enterData(struct date_time_to_set *dtts){
		printf("Enter date, time, day to set in RTC:\n");
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

void main(){
    struct date_time_to_set dtts;
    enterData(&dtts);
    int fd = open("fifo", O_WRONLY);
    if(fd<0){
        perror("Error opening fifo file");
        return;
    }
    if(write(fd, &dtts, sizeof(dtts))==-1){
        perror("Error writing data to fifo");
        close(fd);
        return;
    }
    printf("Client: Time sent to server\n");
    int ack_fd = open("fifo_ack", O_RDONLY);
    if(ack_fd<0){
        perror("Error opening fifo_ack file");
        close(fd);
        return;
    }
    char ack_msg;
    if(read(ack_fd, &ack_msg, sizeof(char))==-1){
        perror("Error reading ack_msg");
        close(fd);
        close(ack_fd);
        return;
    }
    printf("Client: Acknowledgment received from Server (Successful!!)\n");
    close(fd);
    close(ack_fd);
}