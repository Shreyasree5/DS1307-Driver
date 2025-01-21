#include "msg_common.h"

void setRTCAndAck(CommHandle *hComm, struct message *msg) {
    if (setTime(&msg->dtts) == 0) {
        printf("Server: RTC updated successfully.\n");
        msg->msg_type = MSG_SUCCESS; // Response type
        int ret = SendMsg(hComm, msg);
        if (ret < 0) {
            perror("Server: Failed to send acknowledgment");
        }
    } else {
        perror("Server: Failed to set RTC");
    }
}

void getRTCAndAck(CommHandle *hComm, struct message *msg) {
    if (getTime(&msg->dtts) == 0) {
        printf("Server: Current RTC time retrieved successfully.\n");

        msg->msg_type = MSG_SUCCESS; // Response type
        int ret = SendMsg(hComm, msg);
        if (ret < 0) {
            perror("Server: Error sending current time to client");
        } 
    } else {
        perror("Server: Error retrieving RTC time");
    }
}

int main() {
    CommHandle hComm = {0};
    int flags = IPC_CREAT | 0666;
    int ret = CreateResources(&hComm, COMM_TYPE_SHMSEM, flags);
    if (ret < 0) {
        return -1;
    }

    struct message msg;
    printf("Server: Waiting for requests...\n");

    while (1) {
        // Receive a request
        int ret = ReceiveMsgAny(&hComm, &msg);
        if (ret < 0) {
            perror("Server: Error receiving message");
            continue;
        }
        if (msg.msg_type == MSG_SET_TIME) {
            printf("Server: Set RTC request received.\n");
            setRTCAndAck(&hComm, &msg);
        } else if (msg.msg_type == MSG_GET_TIME) {
            printf("Server: Get RTC request received.\n");
            getRTCAndAck(&hComm, &msg);
        } else {
            printf("Server: Unknown request type. %d\n", msg.msg_type);
        }
    }

    msgctl(hComm.msgid, IPC_RMID, NULL);
    shmdt(hComm.shmptr);
    shmctl(hComm.shmid, IPC_RMID, NULL);
    semctl(hComm.semid, 0, IPC_RMID);
    return 0;
}
