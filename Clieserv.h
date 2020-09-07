#ifndef CLIESERV_H
#define CLIESERV_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
//#include <signal.h>

#define MAX_CLIENT 5
#define Buffer_size 2048
#define NAME_LEN 15

typedef struct 
{
    struct sockaddr_in addres;
    int sockfd;
    int uid;
    char name [NAME_LEN];
} clients;

int flag = 0;
int sockfd = 0;
char name[32];



static int cliCount = 0;
static int uid = 10;
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sickfd, struct sockaddr *addr, socklen_t *addrlen);
void Connect(int socket, const struct sockaddr *addr, socklen_t addrlen);
void Inet_pton(int af, const char *src, void *dst);
int Sendto(int sockfd, const void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t addrlen);
int Resvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t *addrlen);
void printIpAddr(struct sockaddr_in addr);
void QueueRemove(int uid);
void AddInQueue(clients *cl);
void sendMessage(char *text, int uid);
void *handleClient();
int Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arq);
void RecvMsg();
void SendMsg();
void catch_ctrl_c_and_exit(int sig); 
void str_trim_lf (char* arr, int length);
void str_overwrite_stdout();



#endif