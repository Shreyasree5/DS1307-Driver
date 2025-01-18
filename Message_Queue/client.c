#include "msg_common.h"

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

void setDefaultTime(struct date_time_to_set *dtts){
    dtts->seconds = 00;
    dtts->minutes = 30;
    dtts->hours = 8;
    dtts->day = 7;
    dtts->date = 5;
    dtts->month = 9;
    dtts->year = 99;
}
void printTime(const struct date_time_to_set *dtts) {
    printf("Current Date and Time:\n");
    printf("Time: %02d:%02d:%02d\n", dtts->hours, dtts->minutes, dtts->seconds);
    printf("Date: %02d/%02d/%02d\n", dtts->date, dtts->month, dtts->year);
    printf("Day: %d\n", dtts->day);
}

int main(int argc, char *argv[]) {
    int msgid;
    struct message msg;

    // Get the message queue
    msgid = CreateMsgQueue(0666);
    if (msgid < 0) {
        perror("Client: Failed to access message queue");
        exit(EXIT_FAILURE);
    }
    while(1){
        int choice;
        // choice = GetUserInput();
        sscanf(argv[1], "%d", &choice);

        if (choice == 1) {
            msg.msg_type = MSG_SET_TIME;
            // enterData(&msg.dtts);
            setDefaultTime(&msg.dtts);
            int ret = SendMsg(msgid, &msg);
            if (ret < 0) {
                perror("Client: Failed to send set RTC request");
                exit(EXIT_FAILURE);
            }
            printf("Client: Set RTC request sent. Waiting for acknowledgment...\n");
        } else if (choice == 2) {
            msg.msg_type = MSG_GET_TIME;
            int ret = SendMsg(msgid, &msg);
            if (ret < 0) {
                perror("Client: Failed to send get RTC request");
                exit(EXIT_FAILURE);
            }
            printf("Client: Get RTC request sent. Waiting for response...\n");
        } else {
            printf("Invalid choice. Exiting.\n");
            exit(EXIT_FAILURE);
        }

        // Wait for the server's response
        int ret = ReceiveMsgAny(msgid, &msg);
        if (ret < 0) {
            perror("Client: Error receiving response");
            if (msg.msg_type == MSG_SUCCESS) {
                printf("Set time successful!\n");
            } else if (msg.msg_type == MSG_FAILED) {
                printf("Set time failed!\n");
            }
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
