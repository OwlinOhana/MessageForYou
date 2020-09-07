#include "Clieserv.h"
#include "Clieserv.c"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("ERROR, no port provided\n");
        exit(1);
    }

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);


	//signal(SIGINT, catch_ctrl_c_and_exit);

	printf("Please enter your name: ");
    fgets(name, 32, stdin);
    str_trim_lf(name, strlen(name));


	if (strlen(name) > 32 || strlen(name) < 2)
    {
		printf("Name must be less than 30 and more than 2 characters.\n");
		exit(1);
	}

    struct sockaddr_in server = {0};

    sockfd = Socket(AF_INET, SOCK_STREAM, 0); //протокол
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);


  // Connect to Server
    Connect(sockfd, (struct sockaddr *)&server, sizeof server); 
 

	// Send name
	send(sockfd, name, NAME_LEN, 0);

	printf("\n**********Welcome**********\n");

	pthread_t send_msg_thread;
    Pthread_create(&send_msg_thread, NULL, (void *) SendMsg, NULL); 

	pthread_t recv_msg_thread;
    Pthread_create(&recv_msg_thread, NULL, (void *) RecvMsg, NULL);
           

	while (1)
		if(flag)
        {
			printf("\nBye\n");
			break;
        }
	

	close(sockfd);

	return 0;
}
















    


