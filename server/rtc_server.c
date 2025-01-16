#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include"ds1307.h"
void printTime(struct date_time_to_set *dtts){
    printf("Current Date and Time:\n");
    printf("Time: %d:%d:%d\n", dtts->hours, dtts->minutes, dtts->seconds);
    printf("Date: %d/%d/%d\n", dtts->date, dtts->month, dtts->year);
    printf("Day: %d\n", dtts->day);
}
void main(){
    int fd = open("fifo", O_RDONLY);
    if(fd<0){
        perror("Error opening fifo file");
        return;
    }
    struct date_time_to_set dtts;
    while(1){
        if(read(fd, &dtts, sizeof(dtts))==-1){
        perror("Error reading data from fifo");
        close(fd);
        return;
        }
        printf("Server: Data recieved from client\n");
        printTime(&dtts);

        if(setTime(&dtts)==0){
        printf("Server: Time set successfully in RTC\n");
        int ack_fd = open("fifo_ack", O_WRONLY);
        if(ack_fd<0){
            perror("Error opening fifo_ack");
            close(fd);
            return;
        }
        char ack_msg = 'D';
        if(write(ack_fd, &ack_msg, sizeof(char))==-1){
            perror("Error writing ack_msg");
            close(ack_fd);
            return;
        }
        }else{
        printf("Cosumer: Failed to set time in RTC\n");
        }
    }
    close(fd);
}