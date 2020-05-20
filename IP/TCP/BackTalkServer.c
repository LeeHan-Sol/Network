#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 128
#define PORT 1234
#define IP_ADDRESS "127.0.0.1"
int readLine(int fd, char *ptr, int maxlen);

void main(int argc, char *argv[])
{
    struct sockaddr_in serverAddress, clientAddress;
    int serverFd, clientFd;
    int sizeOfClientAddress;
    char buffer[BUFFER_SIZE];
    pid_t pid;

    if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Can't create socket.\n");
        exit(1);
    }

    memset((char*)&serverAddress, 0x00, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("Can't bind socket.\n");
        exit(1);
    }

    if(listen(serverFd, 5) < 0)
    {
        printf("Can't listen socket.\n");
        exit(1);
    }

    printf("\t\tSystem : Waiting connction request.\n");
    sizeOfClientAddress = sizeof(clientAddress);

    clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, &sizeOfClientAddress);
    if(clientFd < 0)
    {
        printf("\t\tSystem : Can't accept client.\n");
        exit(1);
    }
    printf("\t\tSystem : Client connected(%s)\n", inet_ntoa(clientAddress.sin_addr));

	memset(buffer, 0x00, BUFFER_SIZE);

   	if((pid = fork()) > 0)
   	{
   		/*Parent's Process*/
   		while(readLine(0, buffer, BUFFER_SIZE) != 0)
   		{
   			printf("flagSocket : %d\n", flagSocket);
	   		// fgets(buffer, BUFFER_SIZE, stdin);
	   		buffer[BUFFER_SIZE - 1] = '\0';

	   		if(write(clientFd, buffer, strlen(buffer)) < 0)
	   		{
	   			printf("ERROR : write()\n");
	   			exit(1);
	   		}

	   		if(strncmp(buffer, "end", 3) == 0)
	   		{
	            printf("\nClient : Goodbye\n");
	   			kill(pid, SIGQUIT);
	   			break;
	   		}
   		}
   	}
   	else if(pid == 0)
   	{
   		/*Child's Process*/
   		while(1)
   		{
            memset(buffer, 0x00, BUFFER_SIZE);
	   		read(clientFd, buffer, BUFFER_SIZE);
	   		if(strncmp(buffer, "end", 3) == 0 || strcmp(buffer, "\0") == 0)
	        {
	            printf("\nClient : Goodbye\n");
	            break;
	        }
	    	printf("\n%5d Client : %s\n", getpid(), buffer);
   		}
   	}
    else
    {
        printf("ERROR : pid = fork()\n");
        exit(1);
    }
   	
   	close(clientFd);
	close(serverFd);
}

int readLine(int fd, char *ptr, int maxlen) 
{ 
	int n, rc; 
	char c; 
	for(n = 1; n < maxlen; n++) 
	{ 
		if((rc = read(fd, &c, 1)) == 1) 
		{ 
			*ptr++ = c; 
			if (c == '\n') break; 
		} 
		else if (rc == 0) 
		{ 
			if(n == 1) return (0); 
			else break; 
		} 
	} 
	*ptr = 0; 
	return (n); 
}