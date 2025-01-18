// Receiver
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg_payload.h"

int main() {
    key_t key = ftok("progfile", 65);
    int msg_id = msgget(key, 0666);

    struct message msg;

    // Try to receive a message of type 2
    printf("Waiting for a message of type 2...\n");
    if (msgrcv(msg_id, &msg, sizeof(msg.payload), 2, 0) == -1) {
        perror("Failed to receive message");
    } else {
        printf("Received message: %d\n", msg.payload.seconds);
    }

    // Cleanup
    msgctl(msg_id, IPC_RMID, NULL);
    return 0;
}
