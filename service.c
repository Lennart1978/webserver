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

#define DEFAULT_FILE "index.html"

void err_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int get_line(int sock_fd, char *buffer, int length)
{
    int i = 0;

    while((i < length - 1) && (recv(sock_fd, &(buffer[i]), 1, 0) == 1))
    {
        if(buffer[i] == '\n')
            break;
        else
            i++;
    }
    if((i > 0) && (buffer[i - 1] == '\r'))
        i--;
    buffer[i] = '\0';
    return i;
}

int is_html(char *filename)
{
    if(strcmp(&(filename[strlen(filename) - 5]), ".html") == 0)
        return 1;
    if(strcmp(&(filename[strlen(filename) - 4]), ".htm") == 0)
        return 1;
    return 0;
}

size_t file_size(char *filename)
{
    struct stat file_info;

    if(stat(filename, &file_info) == -1)
        return 0;
    return file_info.st_size;
}

void http_service(int client_fd)
{
    char buffer[256], cmd[8], url[128], *filename;
    int length;
    FILE *stream;

    if(get_line(client_fd, buffer, 256) == 0)
        return;
    if(sscanf(buffer, "%7s %127s", cmd, url) < 2)
        return;
    while(get_line(client_fd, buffer, 256) > 0)
    {
        if((strcmp(cmd, "GET") != 0) && (strcmp(cmd, "HEAD") != 0))
            return;
    }

    filename = &(url[1]);
    if(strlen(filename) == 0)
        filename = DEFAULT_FILE;
    
    if((stream = fopen(filename, "r")) == NULL)
    {
        send(client_fd, "HTTP/1.0 404 Not Found\r\n"
                        "Content-type: text/html\r\n"
                        "Content-length: 91\r\n\r\n"
                        "<html><head><title>Error</title></head>"
                        "<body><hr><h2>File not found.</h2><hr>"
                        "</body></html>", 162, 0);
        return;
    }

    send(client_fd, "HTTP/1.0 200 OK\r\n", 17, 0);
    if(is_html(filename))
        send(client_fd, "Content-type: text/html\r\n", 25, 0);
    sprintf(buffer, "Content-length: %d\r\n\r\n", (int)file_size(filename));
    send(client_fd, buffer, strlen(buffer), 0);
    if(strcmp(cmd, "GET") == 0)
    {
        while(!feof(stream))
        {
            length = fread(buffer, 1, 256, stream);
            if(length > 0)
            {
                send(client_fd, buffer, length, 0);
                printf("Sent %d bytes\n", length);
            }                
        }
    }
    fclose(stream);
    return;
}