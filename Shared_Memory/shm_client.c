#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>


int main() {
    //shared memory creation
    key_t shm_key = ftok("shmfile", 65);
    if(shm_key==-1){
        perror("shm_key creation failed\n");
        exit(1);
    }
    int shmid = shmget(shm_key, 512, 0);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }
    // Attach to shared memory
    char *shmptr = shmat(shmid, NULL, 0);
    if (shmptr == (char*)-1) {
        perror("shmat failed");
        exit(1);
    }

    //semaphore creation
    key_t sem_key = ftok("semfile", 75);
    if(sem_key==-1){
        perror("sem_key creation failed\n");
        exit(1);
    }
    int semid = semget(sem_key, 1, 0);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    // Write message to shared memory
    printf("Enter msg for server: ");
    fgets(shmptr, 512, stdin);

    // Signal server by incrementing semaphore
    struct sembuf smop;
    smop.sem_num = 0;
    smop.sem_op = 1;
    smop.sem_flg = 0;

    semop(semid, &smop, 1);
    printf("Message sent\n");
    return 0;
}
