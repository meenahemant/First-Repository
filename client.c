//
// Created by Hemant on 11/11/20.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/un.h>
#include <stddef.h>
#include "client.h"
#include "server.h"

int send_bmd_path_to_socket(char *msg, char *socket_file) {
    int sockfd = make_named_socket(socket_file, true);
    printf("heelo");

    /* Write some stuff and read the echoes. */
    log_msg("CLIENT: Connect to server, about to write some stuff...", false);
    if (write(sockfd, msg, strlen(msg)) > 0) {
        /* get confirmation echoed from server and print */
        char buffer[5000];
        memset(buffer, '\0', sizeof(buffer));
        if (read(sockfd, buffer, sizeof(buffer)) > 0) {
            printf("CLIENT: Received from server:: %s\n", buffer);
        }
    }
    log_msg("CLIENT: Processing done, about to exit...", false);
    close(sockfd); /* close the connection */
    return 1;
}
