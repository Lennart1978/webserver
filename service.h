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

void err_exit(char *message);
int get_line(int sock_fd, char *buffer, int length);
int is_html(char *filename);
size_t file_size(char *filename);
void http_service(int client_fd);