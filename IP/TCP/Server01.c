#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFF_SIZE 128

int main(int argc, char* argv[])
{
    char buffer[BUFF_SIZE], temp[20], test[20];
    struct sockaddr_in serverAddress, clientAddress;

    
    int serverFd, clientFd;
    int length, sizeOfMessage;

    if(argc != 2)
    {
        printf("Usage ./filename [PORT]\n");
        exit(0);
    }

    if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("SERVER : Can Not Open Socket\n");
        exit(0);
    }

    //memset(&serverAddress, 0x00, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(atoi(argv[1]));

    if(bind(serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("SERVER : Can Not bind() local address\n");
        exit(0);
    }

    if(listen(serverFd, 5) < 0)
    {
        printf("SEVER : Can Not listen() Connect\n");
        exit(0);
    }

    memset(buffer, 0x00, sizeof(buffer));
    length = sizeof(clientAddress);
    printf("=====[PORT] : %d =====\n", atoi(argv[1]));
    printf("SERVER : Waiting Connection Request\n");

    while(1)
    {
        clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, (socklen_t*)&length);

        if(clientFd < 0)
        {
            printf("SERVER : Accept Failed\n");
            exit(0);
        }

        inet_ntop(AF_INET, &clientAddress.sin_addr.s_addr, temp, sizeof(temp));
        printf("SERVER : %s Client Connect\n", temp);
        
        send(clientFd, "test test test", 0xff, 0);
        sizeOfMessage = recv(clientFd, (char *)buffer, BUFF_SIZE, 0);
        send(clientFd, (char*)buffer, sizeOfMessage, 0);
        close(clientFd);
        printf("SERVER : %s Client Closed\n", temp);
    }

    close(serverFd);

    return 0;
}
