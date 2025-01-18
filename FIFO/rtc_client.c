#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "ds1307.h"

void enterData(struct date_time_to_set *dtts) {
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

int main() {
    struct date_time_to_set dtts;
    int fd_request, fd_response;
    int choice;

    // Open FIFO for sending requests
    fd_request = open("fifo_request", O_RDWR);
    if (fd_request < 0) {
        perror("Client: Error opening request FIFO");
        return -1;
    }
    // Open FIFO for receiving responses
    // fd_response = open("fifo_response", O_RDONLY);
    // if (fd_response < 0) {
    //     perror("Client: Error opening response FIFO");
    //     return -1;
    // }
    fd_response = fd_request;

    while(1) {
        // Prompt user for action
        printf("Choose an option:\n1. Set Time\n2. Get Time\n");
        scanf("%d", &choice);
        int ret;
        switch (choice) {
            case 1:
                enterData(&dtts);
                dtts.type = 1; // Set Time request
                printf("dtts.type %d\n", dtts.type);
                ret = write(fd_request, &dtts, sizeof(dtts));
                if (ret == -1) {
                    perror("Client: Error sending set RTC request");
                    close(fd_request);
                    return ret;
                } else {
                    printf("Client: Set RTC request sent, num bytes sent %d\n", ret);
                    break;
                }

            case 2:
                dtts.type = 2; // Get Time request
                ret = write(fd_request, &dtts, sizeof(dtts));
                if (ret == -1) {
                    perror("Client: Error sending get RTC request");
                    close(fd_request);
                    return ret;
                } else {
                    printf("Client: Get RTC request sent, num bytes sent %d\n", ret);
                    break;
                }

            default:
                printf("Invalid choice. Exiting.\n");
                goto exit;
        }

        if (read(fd_response, &dtts, sizeof(dtts)) == -1) {
            perror("Client: Error reading response");
            goto exit;
        }

        if (dtts.type == 3) {
            printf("Client: RTC updated successfully.\n");
        } else if (dtts.type == 2) {
            printf("Client: Current RTC time received:\n");
            printTime(&dtts);
        } else {
            printf("Client: Received unknown response.\n");
        }
    }

exit:
    close(fd_request);
    // close(fd_response);

    return 0;
}
