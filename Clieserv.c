#ifndef CLIESERV_C
#define CLIESERV_C

#include "Clieserv.h" 

clients *Clients[MAX_CLIENT];


int Socket(int domain, int type, int protocol)
{
    int res = socket(domain, type, protocol);
    if (res == -1)
    {
        perror("socket failed\n");
        exit(1);
    }
    return res;

}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int res = bind(sockfd, addr, addrlen);
    if (res == -1)
    {
        perror("bind failed\n");
        exit(1);
    }
}

void Listen(int sockfd, int backlog)
{
    int res = listen(sockfd, backlog);
     if (res == -1)
    {
        perror("listen failed");
        exit(1);
    }

}

int Accept(int sickfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int res = accept(sickfd, addr, addrlen);
    if (res == -1)
    {
        perror("accept failed\n");
        exit(1);
    }
    return res;
}


void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int res = connect(sockfd, addr, addrlen);
    if (res == -1)
    {
        perror("connect failed\n");
        exit(1);
    }
}

void Inet_pton(int af, const char *src, void *dst)
{
    int res = inet_pton(af, src, dst);
    if (res == 0)
    {
        printf("inet_pton failed: src does not contain a character strling representing a valid network addres in specified addres family\n");
        exit(1);
    }
    if (res == -1)
    {
        perror("inet_pton failed\n");
        exit(1);
    }
}
int Write(int handle, void *buf, int count)
{
    int res = write (handle, buf, count);
    if(res < count || res < 0)
        {
            perror("write failed\n");
            exit(1);
        }
    return res;
}

int Sendto(int sockfd, const void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t addrlen)
{
    int res = sendto(sockfd, buf, len, flags, addr, addrlen);
    if (res < 0)
        perror("Sendto failed\n");
    return res;
}

int Recv(int sockfd, void *buf, size_t len, int flags)
{
    int res = recv(sockfd, buf, len, flags);
    if (res < 0)
        perror("resvfrom failed\n");
    return res;
}

void printIpAddr(struct sockaddr_in addr)
{
    printf("%d.%d.%d.%d", 
            addr.sin_addr.s_addr & 0xff,
            (addr.sin_addr.s_addr & 0xff00) >> 8,
            (addr.sin_addr.s_addr & 0xff0000) >> 16,
            (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

void str_trim_lf (char* arr, int length) 
{
  for (int i = 0; i < length; i++)
    if (arr[i] == '\n') 
    {
      arr[i] = '\0';
      break;
    }
  
}

void AddInQueue(clients *cl)
{
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < MAX_CLIENT; i++)
        if (!Clients[i])
        {
            Clients[i] = cl;
            break;
        }


    pthread_mutex_unlock(&clientsMutex);
}

void QueueRemove(int uid)
{
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < MAX_CLIENT; i++)
        if (Clients[i])
            if (Clients[i]->uid == uid)
            {
                Clients[i] = NULL;
                break;
            }
    pthread_mutex_unlock(&clientsMutex);
}

void sendMessage(char *text, int uid)
{
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < MAX_CLIENT; i++)
        if (Clients[i])
            if (Clients[i]->uid != uid)
                if(write(Clients[i]->sockfd, text, strlen(text)) < 0)
                {
					perror("ERROR: write to descriptor failed");
					break;
                }
    pthread_mutex_unlock(&clientsMutex);
}

void *handleClient(void *arg)
{
    char buf[Buffer_size];
    char name[NAME_LEN];
    int leaveFlags;

    cliCount++;
    clients *cli = (clients *)arg;

    if(recv(cli->sockfd, name, NAME_LEN, 0) <= 0 || strlen(name) <  2 || strlen(name) >= NAME_LEN-1)
    {
		printf("Didn't enter the name.\n");
		leaveFlags = 1;
	} 
    else
    {
		strcpy(cli->name, name);
		sprintf(buf, "%s has joined\n", cli->name);
		printf("%s", buf);
		sendMessage(buf, cli->uid);
	}

	bzero(buf, Buffer_size);

    while(1)
    {
        if (leaveFlags)
            break;

        Recv(cli->sockfd, buf, Buffer_size, 0);

        if (strlen(buf) > 0)
        {
            sendMessage(buf, cli->uid);
            str_trim_lf(buf, strlen(buf));
            printf("%s -> %s", buf, cli->name);
        }
        else if (strcmp(buf, "exit") == 0)
        {
            sprintf(buf, "%s has left\n", cli->name);
            printf("%s", buf);
            sendMessage(buf, cli->uid);
            leaveFlags = 1;
        }
        else 
        {
            printf("-1\n");
            leaveFlags = 1;
        }
        bzero(buf, Buffer_size);
    }

    close(cli->sockfd);
    QueueRemove(cli->uid);
    free(cli);
    cliCount--;
    pthread_detach(pthread_self());

    return 0;

}

int Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arq)
{
    int res = pthread_create(thread, attr, start_routine, arq);
    if (res != 0)
    {
        perror("pthread_create failed\n");
        exit(1);
    }
    return res;
}




void str_overwrite_stdout()
{
  printf("%s", "> ");
  fflush(stdout);
}


void catch_ctrl_c_and_exit(int sig) 
{
    flag = 1;
}

void SendMsg()
{
    char message[Buffer_size] = {};
    char buffer[Buffer_size + NAME_LEN] = {};

    while(1) 
    {
        str_overwrite_stdout();
        fgets(message, Buffer_size, stdin);
        str_trim_lf(message, Buffer_size);

        if (strcmp(message, "exit") == 0) 
                break;
        else 
        {
            sprintf(buffer, "%s: %s\n", name, message);
            send(sockfd, buffer, strlen(buffer), 0);
        }

        bzero(message, Buffer_size + NAME_LEN);
        bzero(buffer, Buffer_size);
    }
    catch_ctrl_c_and_exit(2);
}

void RecvMsg() 
{
	char message[Buffer_size] = {0};
    while (1) 
    {
            int receive = recv(sockfd, message, Buffer_size, 0);
            if (receive > 0) 
            {
                printf("%s", message);
                str_overwrite_stdout();
            } 
            else if (receive == 0) 
                    break;
            
            memset(message, 0, sizeof(message));
    }
}

#endif
