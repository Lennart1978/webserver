#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include "service.h"

#define MY_PORT 80
#define N_CONNECTIONS 20
#define HTML_PATH "."

/**
 * The main entry point for the web server application.
 * 
 * This function sets up a TCP socket, binds it to a specific port, and listens for incoming client connections.
 * When a client connects, it forks a new child process to handle the client request, while the parent process continues
 * to listen for new connections.
 * 
 * The child process calls the `http_service` function to handle the client request, and then exits.
 */
int main()
{
    int sock_fd, client_fd, err, addr_size, pid;
    struct sockaddr_in my_addr, client_addr;

    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
        err_exit("webserver: Can't create new socket\n");
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MY_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    err = bind(sock_fd, &my_addr, sizeof(struct sockaddr_in));
    if(err == -1)
        err_exit("webserver: Can't bind name to socket\n");
    
    setuid(getuid());

    err = listen(sock_fd, N_CONNECTIONS);
    if(err == -1)
        err_exit("webserver: Can't listen on socket\n");
    
    if(chdir(HTML_PATH) != 0)
        err_exit("webserver: Can't set HTML path\n");
    
    signal(SIGCHLD, SIG_IGN);

    printf("Type Ctrl-C to stop.\n");

    while(1)
    {
        addr_size = sizeof(struct sockaddr_in);
        client_fd = accept(sock_fd, &client_addr, &addr_size);
        if(client_fd == -1)
            err_exit("webserver: Can't accept client connection\n");
        
        if((pid = fork()) == -1)
        {
            fprintf(stderr, "webserver: Can't fork child process\n");
            exit(EXIT_FAILURE);
        }
        // child process
        else if(pid == 0)
        {
            close(sock_fd);
            http_service(client_fd);
            close(client_fd);
            exit(EXIT_SUCCESS);
        }
        close(client_fd);
    }

    return 0;
}