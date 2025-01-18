// Sender
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg_payload.h"

void GetUserDate(struct date_time_to_set *dtts) {
    printf("Enter date, time, and day to set in RTC:\n");
    printf("Seconds (0-59): ");
    scanf("%d", &dtts->seconds);
    printf("Minutes (0-59): ");
    scanf("%d", &dtts->minutes);
    printf("Hours (0-23): ");
    scanf("%d", &dtts->hours);
    printf("Day (1=Sun, 7=Sat): ");
    scanf("%d", &dtts->day);
    printf("Date (1-31): ");
    scanf("%d", &dtts->date);
    printf("Month (1-12): ");
    scanf("%d", &dtts->month);
    printf("Year (0-99): ");
    scanf("%d", &dtts->year);
}

void printTime(const struct date_time_to_set *dtts) {
    printf("Current Date and Time:\n");
    printf("Time: %02d:%02d:%02d\n", dtts->hours, dtts->minutes, dtts->seconds);
    printf("Date: %02d/%02d/%02d\n", dtts->date, dtts->month, dtts->year);
    printf("Day: %d\n", dtts->day);
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
        ret = msgsnd(msg_id, msg, sizeof(msg->payload), 0);
    }
    return ret;
}

int main() {
    key_t key = ftok("progfile", 65);
    int msg_id = msgget(key, IPC_CREAT | 0666);

    struct message msg1;
    msg1.msg_type = 2;
    msg1.payload.seconds = 1331;

    int choice =  GetUserInput();
    if (choice == 1) {
        GetUserDate(&msg1.payload);
        int ret = SendMsg(msg_id, &msg1);
        if (ret < 0) {
            printf("SendMsg failed %d\n", ret);
        }
    }
    return 0;
}
