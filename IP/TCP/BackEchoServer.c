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
    struct sockaddr_in serverAddress, clientAddress;
    int serverFd, clientFd;
    int sizeOfClientAddress;
    char buffer[BUFFER_SIZE];
    int stringInBuffer;
    int flagSocket = 0;

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

    while(1)
    {
        if(flagSocket == 0)
        {
            printf("\t\tSystem : Waiting connction request.\n");
            sizeOfClientAddress = sizeof(clientAddress);

            clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, &sizeOfClientAddress);
            if(clientFd < 0)
            {
                printf("Server : Can't accept client.\n");
                exit(1);
            }
            printf("\t\tSystem : Client connected(%s)\n", inet_ntoa(clientAddress.sin_addr));
            flagSocket++;
        }

        memset(buffer, 0x00, BUFFER_SIZE);
        read(clientFd, buffer, BUFFER_SIZE);
        if(strcmp(buffer, "end") == 0 || strcmp(buffer, "\0") == 0)
        {
            printf("Client : Goodbye\n");
            close(clientFd); 
            flagSocket = 0;
            continue;
        }
        printf("Client : %s\n", buffer);

        write(clientFd, buffer, strlen(buffer));
    }
    close(serverFd);
}
