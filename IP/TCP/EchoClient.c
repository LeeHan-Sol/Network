#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 128
#define PORT 1234
#define IP_ADDRESS "127.0.0.1"

void main(int argc, char *argv[])
{
    struct sockaddr_in socketAddress;
    
    int socketFd, resultRead, stringInBuffer;
    char buffer[BUFFER_SIZE];

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

    while(1)
    {
        printf(">> ");
        if(fgets(buffer, BUFFER_SIZE, stdin) < 0)
        {
            printf("ERROR : fgets()\n");
            exit(1);
        }
        buffer[strlen(buffer) - 1] = '\0';

        if(write(socketFd, buffer, strlen(buffer)) < 0)
        {
            printf("ERROR : write()\n");
            exit(1);
        }
 
        if(strcmp(buffer, "end") == 0)
        {
            printf("Disconnected\n");
            break;
        }

        for(int i = 0, resultRead = 0; i < strlen(buffer); i += resultRead)
        {
            if((resultRead = read(socketFd, buffer, strlen(buffer))) < 0)
            {
                printf("ERROR : read()\n");
                exit(1);
            }
        }
        printf("Server : %s\n", buffer);
        memset(buffer, 0x00, BUFFER_SIZE);
    }
    close(socketFd);
}

