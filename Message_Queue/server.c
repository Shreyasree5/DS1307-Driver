#include "msg_common.h"

void setRTCAndAck(struct message *msg, int msgid) {
    if (setTime(&msg->dtts) == 0) {
        printf("Server: RTC updated successfully.\n");

        msg->msg_type = MSG_SUCCESS; // Response type
        int ret = SendMsg(msgid, msg);
        if (ret < 0) {
            perror("Server: Failed to send acknowledgment");
        }
    } else {
        perror("Server: Failed to set RTC");
    }
}

void getRTCAndAck(struct message *msg, int msgid) {
    if (getTime(&msg->dtts) == 0) {
        printf("Server: Current RTC time retrieved successfully.\n");

        msg->msg_type = MSG_SUCCESS; // Response type
        int ret = SendMsg(msgid, msg);
        if (ret < 0) {
            perror("Server: Error sending current time to client");
        } 
    } else {
        perror("Server: Error retrieving RTC time");
    }
}

int main() {
    int msgid;

    // Create the message queue
    msgid = CreateMsgQueue(IPC_CREAT|0666);
    if (msgid < 0) {
        perror("Server: Failed to create message queue");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    printf("Server: Waiting for requests...\n");

    while (1) {
        // Receive a request
        int ret = ReceiveMsgAny(msgid, &msg);
        if (ret < 0) {
            perror("Server: Error receiving message");
            continue;
        }
        if (msg.msg_type == MSG_SET_TIME) {
            printf("Server: Set RTC request received.\n");
            setRTCAndAck(&msg, msgid);
        } else if (msg.msg_type == MSG_GET_TIME) {
            printf("Server: Get RTC request received.\n");
            getRTCAndAck(&msg, msgid);
        } else {
            printf("Server: Unknown request type. %d\n", msg.msg_type);
        }
    }

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
