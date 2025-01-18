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

typedef enum {
    MSG_ANY = 0,
    MSG_SET_TIME = 1,
    MSG_GET_TIME,
    MSG_SUCCESS,
    MSG_FAILED,
} MsgType;

int CreateMsgQueue(int flags) {
    int msgid = msgget(MSGQ_KEY, flags);
    return msgid;
}

int GetUserInput() {
    int choice;
    printf("Choose 1.Set Time 2.Get Time\n");
    scanf("%d", &choice);
    return choice;
}
int SendMsg(int msg_id, struct message* msg) {
    int ret = 0;
    if (msg == NULL) {
        return -1;
    } else {
        ret = msgsnd(msg_id, msg, sizeof(msg->dtts), 0);
    }
    return ret;
}
int ReceiveMsg(int msgid, struct message *msg, MsgType type) {
    int ret = 0;
    if(msg == NULL) {
        return -1;
    } else {
        ret = msgrcv(msgid, msg, sizeof(msg->dtts), type, 0);
    }
    return ret;
}
int ReceiveMsgAny(int msgid, struct message *msg) {
    int ret = 0;
    if(msg == NULL) {
        return -1;
    } else {
        ret = msgrcv(msgid, msg, sizeof(msg->dtts), MSG_ANY, 0);
    }
    return ret;
}