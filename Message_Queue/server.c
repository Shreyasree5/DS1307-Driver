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

void setRTC(struct message *msg, int msgid) {
    if (setTime(&msg->dtts) == 0) {
        printf("Server: RTC updated successfully.\n");

        msg->msg_type = 3; // Response type
        if (msgsnd(msgid, msg, sizeof(msg->dtts), 0) == -1) {
            perror("Server: Failed to send acknowledgment");
        }
    } else {
        perror("Server: Failed to set RTC");
    }
}

void getRTC(struct message *msg, int msgid) {
    if (getTime(&msg->dtts) == 0) {
        printf("Server: Current RTC time retrieved successfully.\n");

        msg->msg_type = 3; // Response type
        if (msgsnd(msgid, msg, sizeof(msg->dtts), 0) == -1) {
            perror("Server: Error sending current time to client");
        }
    } else {
        perror("Server: Error retrieving RTC time");
    }
}

int main() {
    int msgid;

    // Create the message queue
    if ((msgid = msgget(MSGQ_KEY, IPC_CREAT | 0666)) == -1) {
        perror("Server: Failed to create message queue");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    printf("Server: Waiting for requests...\n");

    while (1) {
        // Receive a request
        if (msgrcv(msgid, &msg, sizeof(msg.dtts), 0, 0) == -1) {
            perror("Server: Error receiving message");
            continue;
        }
        if (msg.msg_type == 1) {
            printf("Server: Set RTC request received.\n");
            setRTC(&msg, msgid);
        } else if (msg.msg_type == 2) {
            printf("Server: Get RTC request received.\n");
            getRTC(&msg, msgid);
        } else {
            printf("Server: Unknown request type.\n");
        }
    }
    return 0;
}
