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
#include "server.h"
#include "esb.h"
#include "../adapter/xml_json.h"
#include "../adapter/email.h"
#include "../adapter/http.h"





bool create_worker_thread(int fd);

/**
 * This code is adapted from the samples available at:
 * https://opensource.com/article/19/4/interprocess-communication-linux-networking and
 * https://www.gnu.org/software/libc/manual/html_node/Local-Socket-Example.html
 *
 * Compile it using: gcc local_socket_client_server.c -lpthread -o ipc_demo

 * Needless to say, this code is not perfect and may have some subtle bugs. A purpose
 * if this code is to show how to write a socket based client server program that
 * off-loads the client connection to a new thread for processing.
 */

void log_msg(const char *msg, bool terminate) {
    printf("%s\n", msg);
    if (terminate) exit(-1); /* failure */
}

/**
 * Create a named (AF_LOCAL) socket at a given file path.
 * @param socket_file
 * @param is_client whether to create a client socket or server socket
 * @return Socket descriptor
 */
int make_named_socket(const char *socket_file, bool is_client) {
    printf("Creating AF_LOCAL socket at path %s\n", socket_file);
    if (!is_client && access(socket_file, F_OK) != -1) {
        log_msg("An old socket file exists, removing it.", false);
        if (unlink(socket_file) != 0) {
            log_msg("Failed to remove the existing socket file.", true);
        }
    }
    struct sockaddr_un name;
    /* Create the socket. */
    int sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        log_msg("Failed to create socket.", true);
    }

    /* Bind a name to the socket. */
    name.sun_family = AF_LOCAL;
    strncpy (name.sun_path, socket_file, sizeof(name.sun_path));
    name.sun_path[sizeof(name.sun_path) - 1] = '\0';

    /* The size of the address is
       the offset of the start of the socket_file,
       plus its length (not including the terminating null byte).
       Alternatively you can just do:
       size = SUN_LEN (&name);
   */
    size_t size = (offsetof(struct sockaddr_un, sun_path) +
                   strlen(name.sun_path));
    if (is_client) {
        if (connect(sock_fd, (struct sockaddr *) &name, size) < 0) {
            log_msg("connect failed", 1);
        }
    } else {
        if (bind(sock_fd, (struct sockaddr *) &name, size) < 0) {
            log_msg("bind failed", 1);
        }
    }
    return sock_fd;
}

/**
 * Starts a server socket that waits for incoming client connections.
 * @param socket_file
 * @param max_connects
 */
_Noreturn void start_server_socket(char *socket_file, int max_connects) {
    int sock_fd = make_named_socket(socket_file, false);

    /* listen for clients, up to MaxConnects */
    if (listen(sock_fd, max_connects) < 0) {
        log_msg("Listen call on the socket failed. Terminating.", true); /* terminate */
    }
    log_msg("Listening for client connections...\n", false);
    /* Listens indefinitely */
    while (1) {
        struct sockaddr_in caddr; /* client address */
        int len = sizeof(caddr);  /* address length could change */

        printf("Waiting for incoming connections...\n");
        int client_fd = accept(sock_fd, (struct sockaddr *) &caddr, &len);  /* accept blocks */

        if (client_fd < 0) {
            log_msg("accept() failed. Continuing to next.", 0); /* don't terminate, though there's a problem */
            continue;
        }
        /* Start a worker thread to handle the received connection. */
        if (!create_worker_thread(client_fd)) {
            log_msg("Failed to create worker thread. Continuing to next.", 0);
            continue;
        }

    }  /* while(1) */
}

/**
 * This functions is executed in a separate thread.
 * @param sock_fd
 */
void thread_function(int sock_fd) {
    log_msg("SERVER: thread_function: starting", false);
    char buffer[5000];
    memset(buffer, '\0', sizeof(buffer));
    int count = read(sock_fd, buffer, sizeof(buffer));
    if (count > 0) {
        printf("SERVER: Received from client: %s\n", buffer);
        write(sock_fd, buffer, sizeof(buffer)); /* echo as confirmation */
    }
    printf("           content  %s      ", buffer);
    char data[100];
    strcpy(data,"../.");
    strcat(data,buffer);
    int status= process_esb_request(data);
    if(status==1){
        printf("Process esb request succesfully");
    }
    bmd*b = parse_bmd_xml(data);
    printf("payload is %s  \n",b->payload);
    char*file_name= xml_to_json(b->payload,"Payload");
    printf("f %s \n",file_name);
    int email_status= transport_email("m.hemant3004@gmail.com",file_name);
    if(email_status==1){
        
        printf("email send successfully\n");
    }
    int http_status =http("192.168.43.94",file_name);
    if(http_status==1){
        printf("send successfully\n");
    }
    close(sock_fd); /* break connection */
    log_msg("SERVER: thread_function: Done. Worker thread terminating.", false);
    pthread_exit(NULL); // Must be the last statement
}

/**
 * This function launches a new worker thread.
 * @param sock_fd
 * @return Return true if thread is successfully created, otherwise false.
 */
bool create_worker_thread(int sock_fd) {
    log_msg("SERVER: Creating a worker thread.", false);
    pthread_t thr_id;
    int rc = pthread_create(&thr_id,
            /* Attributes of the new thread, if any. */
                            NULL,
            /* Pointer to the function which will be
             * executed in new thread. */
                            thread_function,
            /* Argument to be passed to the above
             * thread function. */
                            (void *) sock_fd);
    if (rc) {
        log_msg("SERVER: Failed to create thread.", false);
        return false;
    }
    return true;
}

/**
 * This is the driver function you can use to test client-server
 * communication using sockets.
 * @param argc
 * @param argv
 * @return
 */
int main() {
    start_server_socket("./my_sock",10);
}
