#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>


int MyMsgSnd(CommHandle *hcomm, int shmid, int semid, struct message * msg) {
    // Write message to shared memory
    //printf("Enter msg for server: ");
    strcpy(hcomm->shmptr, msg);

    // Signal server by incrementing semaphore
    struct sembuf smop;
    smop.sem_num = 0;
    smop.sem_op = 1;
    smop.sem_flg = 0;

    semop(semid, &smop, 1);
    printf("Message sent\n");
    return 0;
}
int MyMsgRcv(CommHandle *hComm, int shmid, int semid, struct message * msg){
    // Initialize semaphore to 0
    semctl(hComm->semid, 0, SETVAL, 0);

    // Wait for client's signal
    struct sembuf smop;
    smop.sem_num = 0;
    smop.sem_op = -1;
    smop.sem_flg = 0;

    printf("Waiting for client msg\n");
    semop(hComm->semid, &smop, 1);

    // Read message from shared memory
    printf("Message from client: %s\n", hComm->shmptr);

    // Cleanup
    shmdt(hComm->shmptr);
    shmctl(hComm->shmid, IPC_RMID, NULL);
    semctl(hComm->semid, 0, IPC_RMID);

    return 0;
}