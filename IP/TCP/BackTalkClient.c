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
    struct sockaddr_in socketAddress;
    
    int socketFd, resultRead;
    char buffer[BUFFER_SIZE];
    pid_t pid;

    if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Can't create socket.\n");
        exit(1);
    }

    memset((char *)&socketAddress, 0x00, sizeof(socketAddress));

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    if(connect(socketFd, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) < 0)
    {
        printf("Can't connect.\n");
        exit(1);
    }
    printf("Connected\n\n");

    if((pid = fork()) > 0)
    {
        /*Parent's Process*/
        while(readLine(0, buffer, BUFFER_SIZE) != 0)
        {
            // if(fgets(buffer, BUFFER_SIZE, stdin) < 0)
            // {
            //     printf("ERROR : fgets()\n");
            //     exit(1);
            // }
            buffer[strlen(buffer) - 1] = '\0';

            if(write(socketFd, buffer, strlen(buffer)) < 0)
            {
                printf("ERROR : write()\n");
                exit(1);
            }

            if(strncmp(buffer, "end", 3) == 0)
            {
                printf("\nServer : Goodbye\n");
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
            read(socketFd, buffer, BUFFER_SIZE);
            if(strncmp(buffer, "end", 3) == 0 || strcmp(buffer, "\0") == 0)
            {
                printf("\nServer : Goodbye\n");
                break;
            }
            printf("\n%5d Server : %s\n", getpid(), buffer);
        }
    }
    else
    {
        printf("ERROR : pid = fork()\n");
        exit(1);
    }
    close(socketFd);
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