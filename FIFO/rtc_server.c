#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "ds1307.h"

void printTime(struct date_time_to_set *dtts) {
    printf("Current Date and Time:\n");
    printf("Time: %02d:%02d:%02d\n", dtts->hours, dtts->minutes, dtts->seconds);
    printf("Date: %02d/%02d/%02d\n", dtts->date, dtts->month, dtts->year);
    printf("Day: %d\n", dtts->day);
}

void setRTC(struct date_time_to_set *dtts, int response_fd) {
    if (setTime(dtts) == 0) {
        printf("Server: RTC updated successfully.\n");
        dtts->type = 3; // Acknowledge success
        if (write(response_fd, dtts, sizeof(*dtts)) == -1) {
            printf("Server: Failed to send acknowledgment");
        }
    } else {
        printf("Server: Failed to set RTC");
        dtts->type = -1; // Acknowledge success
        if (write(response_fd, dtts, sizeof(*dtts)) == -1) {
            printf("Server: Failed to send acknowledgment");
        }
    }
}

void getRTC(int response_fd) {
    struct date_time_to_set dtts;
    int ret = getTime(&dtts);
    if (ret == 0) {
        printf("Server: Current RTC time retrieved successfully.\n");
        dtts.type = 2; // Respond with RTC time
        if (write(response_fd, &dtts, sizeof(dtts)) == -1) {
            perror("Server: Error sending current time to client");
        }
    } else {
        printf("Server: Error retrieving RTC time\n");
        dtts.type=0;
    }
}

int main() {
    // Create FIFOs
    mkfifo("fifo_request", 0666);
    mkfifo("fifo_response", 0666);

    int request_fd = open("fifo_request", O_RDONLY);
    int response_fd = open("fifo_response", O_WRONLY);

    if (request_fd < 0 || response_fd < 0) {
        perror("Server: Error opening FIFO files");
        return -1;
    }

    struct date_time_to_set dtts;

    while (1) {
        // Read request from client
        int ret = read(request_fd, &dtts, sizeof(dtts));
        if (ret == -1) {
            perror("Server: Error reading data from FIFO");
            break;
        } else {
            printf("number of bytes read from client %d\n", ret);
        }

        if (dtts.type == 1) {
            printf("Server: Set RTC request received.\n");
            setRTC(&dtts, response_fd);
        } else if (dtts.type == 2) {
            printf("Server: Get RTC request received.\n");
            getRTC(response_fd);
        } else {
            printf("Server: Unknown request type. %d\n", dtts.type);
            return 0;
        }
    }

    close(request_fd);
    close(response_fd);
    unlink("fifo_request");
    unlink("fifo_response");

    return 0;
}
