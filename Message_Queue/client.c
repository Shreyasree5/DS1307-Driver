#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ds1307.h"

#define MSGQ_KEY 1234

struct message {
    long msg_type; // 1: Set RTC request, 2: Get RTC request, 3: Response
    struct date_time_to_set dtts; // RTC data structure
};

void enterData(struct date_time_to_set *dtts) {
    printf("Enter date, time, and day to set in RTC:\n");
    printf("Seconds: ");
    scanf("%d", &dtts->seconds);
    printf("Minutes: ");
    scanf("%d", &dtts->minutes);
    printf("Hours: ");
    scanf("%d", &dtts->hours);
    printf("Day: ");
    scanf("%d", &dtts->day);
    printf("Date: ");
    scanf("%d", &dtts->date);
    printf("Month: ");
    scanf("%d", &dtts->month);
    printf("Year: ");
    scanf("%d", &dtts->year);
}

void printTime(const struct date_time_to_set *dtts) {
    printf("Current Date and Time:\n");
    printf("Time: %02d:%02d:%02d\n", dtts->hours, dtts->minutes, dtts->seconds);
    printf("Date: %02d/%02d/%02d\n", dtts->date, dtts->month, dtts->year);
    printf("Day: %d\n", dtts->day);
}

int main() {
    int msgid;
    struct message msg;

    // Get the message queue
    if ((msgid = msgget(MSGQ_KEY, 0666)) == -1) {
        perror("Client: Failed to access message queue");
        exit(EXIT_FAILURE);
    }
    while(1){
        //printf("Choose an option:\n1. Set RTC\n2. Get RTC\n");
        int choice=2;
        //scanf("%d", &choice);

        if (choice == 1) {
            msg.msg_type = 1; // Set RTC request
            enterData(&msg.dtts);

            if (msgsnd(msgid, &msg, sizeof(msg.dtts), 0) == -1) {
                perror("Client: Failed to send set RTC request");
                exit(EXIT_FAILURE);
            }

            printf("Client: Set RTC request sent. Waiting for acknowledgment...\n");
        } else if (choice == 2) {
            msg.msg_type = 2; // Get RTC request

            if (msgsnd(msgid, &msg, sizeof(msg.dtts), 0) == -1) {
                perror("Client: Failed to send get RTC request");
                exit(EXIT_FAILURE);
            }

            printf("Client: Get RTC request sent. Waiting for response...\n");
        } else {
            printf("Invalid choice. Exiting.\n");
            exit(EXIT_FAILURE);
        }

        // Wait for the server's response
        if (msgrcv(msgid, &msg, sizeof(msg.dtts), 3, 0) == -1) {
            perror("Client: Error receiving response");
            exit(EXIT_FAILURE);
        }

        if (choice == 1) {
            printf("Client: RTC updated successfully.\n");
        } else if (choice == 2) {
            printf("Client: Current RTC time received:\n");
            printTime(&msg.dtts);
        }
        sleep(1);
    }
    return 0;
}
