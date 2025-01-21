#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>

int main() {
    //shared memory creation
    key_t shm_key = ftok("shmfile", 65);
    int shmid = shmget(shm_key, 512, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    //semaphore creation
    key_t sem_key = ftok("semfile", 75);
    if(sem_key==-1){
        perror("sem_key creation failed\n");
        exit(1);
    }
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    // Attach to shared memory
    char *shmptr = shmat(shmid, NULL, 0);
    if (shmptr == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Initialize semaphore to 0
    semctl(semid, 0, SETVAL, 0);

    // Wait for client's signal
    struct sembuf smop;
    smop.sem_num = 0;
    smop.sem_op = -1;
    smop.sem_flg = 0;

    printf("Waiting for client msg\n");
    semop(semid, &smop, 1);

    // Read message from shared memory
    printf("Message from client: %s\n", shmptr);

    // Cleanup
    shmdt(shmptr);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
