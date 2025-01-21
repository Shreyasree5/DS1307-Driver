#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include "ds1307.h"

typedef enum {
    COMM_TYPE_MSGQ = 0,
    COMM_TYPE_SHMSEM,
    COMM_TYPE_INVALID
} CommType;

typedef struct
{
    int msgid;
    char* shmptr;
    int semid;
    int shmid;
    CommType type;
} CommHandle;


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

int CreateSharedMem(CommHandle* hComm, int flags) {
    if (hComm == NULL) {
        printf("Invalid input");
        return -1;
    }

    key_t shm_key = ftok("shmfile", 65);
    if(shm_key==-1){
        perror("shm_key creation failed\n");
        return -1;
    }
    hComm->shmid = shmget(shm_key, 512, flags);
    if (hComm-> shmid == -1) {
        perror("shmget failed");
        return -1;
    }
    // Attach to shared memory
    hComm->shmptr = shmat(hComm->shmid, NULL, flags);
    if (hComm->shmptr == (char*)-1) {
        perror("shmat failed");
        return -1;
    }

    return 0;
}

int CreateSem(int* semid, int flags) {
    if (semid == NULL) {
        printf("Invalid input");
        return -1;
    }
    //semaphore creation
    key_t sem_key = ftok("semfile", 75);
    if(sem_key==-1){
        perror("sem_key creation failed\n");
        return -1;
    }
    *semid = semget(sem_key, 1, flags);
    if (*semid == -1) {
        perror("semget failed");
        return -1;
    }

    return 0;
}

int CreateResources(CommHandle* hComm, CommType type, int flags) {

    if (hComm == NULL) {
        printf("Invalid input\n");
        return -1;
    }
    hComm->type = type;
    if (type == COMM_TYPE_MSGQ) {
        hComm->msgid = CreateMsgQueue(flags);
        if (hComm->msgid < 0) {
            return -1;
        }
    } else if (type == COMM_TYPE_SHMSEM) {
        int ret = CreateSharedMem(hComm, flags);
        if (ret < 0) {
            return ret;
        }
        ret = CreateSem(&hComm->semid, flags);
        if (ret < 0) {
            return ret;
        }
    }else{
        return -1;
    }

    return 0;
}

int GetUserInput() {
    int choice;
    printf("Choose 1.Set Time 2.Get Time\n");
    scanf("%d", &choice);
    return choice;
}
int SendMsg(CommHandle* hComm, struct message* msg) {
    int ret = 0;
    if (msg == NULL || hComm == NULL) {
        return -1;
    } else if (hComm->type== COMM_TYPE_MSGQ){
        ret = msgsnd(hComm->msgid, msg, sizeof(msg->dtts), 0);
    } else if (hComm->type == COMM_TYPE_SHMSEM){
        ret = MyMsgSnd(hComm, msg);
    }
    return ret;
}

int ReceiveMsgAny(CommHandle* hComm, struct message *msg) {
    int ret = 0;
    if(msg == NULL || hComm == NULL) {
        printf("msg or hcomm null");
        return -1;
    } else if (hComm->type == COMM_TYPE_MSGQ) {
        ret = msgrcv(hComm->msgid, msg, sizeof(msg->dtts), MSG_ANY, 0);
    }else if (hComm->type == COMM_TYPE_SHMSEM) {
        ret = MyMsgRcv(hComm, msg, MSG_ANY);
    }else{
        printf("Invalid comm type\n");
        return -1;
    }
    return ret;
}

int MyMsgSnd(CommHandle *hComm, struct message * msg) {
    memcpy(hComm->shmptr, msg, sizeof(struct message));

    // Signal server by incrementing semaphore
    struct sembuf smop;
    smop.sem_num = 0;
    smop.sem_op = 1;
    smop.sem_flg = 0;

    semop(hComm->semid, &smop, 1);
    printf("Message sent via MyMsgSnd\n");
    return 0;
}

int MyMsgRcv(CommHandle *hComm, struct message * msg, int MSG_ANY){
    if (msg == NULL || hComm == NULL) {
        return -1;
    }
    // Wait for client's signal
    struct sembuf smop;
    smop.sem_num = 0;
    smop.sem_op = -1;
    smop.sem_flg = 0;
    if (semop(hComm->semid, &smop, 1) < 0) {
        perror("Error waiting for semaphore");
        return -1;
    }
    memcpy(msg, hComm->shmptr, sizeof(struct message));
    return 0;
}