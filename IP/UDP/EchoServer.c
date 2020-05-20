#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char* port;
    char buffer[128];
    int serverFD, clientFD;
    int n, clientLength;
    int i = 1;
    struct sockaddr_in serverAddr, clientAddr;
    FILE *fileOutput;

    clientLength = sizeof(clientAddr);
    
    if((fileOutput = fopen("Server.txt", "w")) < 0)
    {
        printf("(I) ERROR : fopen()\n");
        exit(1);
    }

    if(argc != 2)
    {
        printf("Usage : ./EchoServer PORT\n");
        exit(1);
    }
    port = argv[1];

    if((serverFD = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("(I) ERROR : socket()\n");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(atoi(port));

    if((bind(serverFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) < 0)
    {
        printf("(I) ERROR : bind()\n");
        exit(1);
    }

    while(1)
    {
        printf("(I) Server ready(%d)\n", i);
        memset((struct sockaddr*)&clientAddr, 0x00, sizeof(clientAddr));

        if((n = recvfrom(serverFD, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientLength)) <= 0)
        {
            close(clientFD);
            continue;
        }

        fputs(buffer, fileOutput);

        if(strstr(buffer, "10000"))
        {
            printf("Complete recieve\n");
            break;
        }

        i++;
    }

    fclose(fileOutput);
    close(serverFD);

    return 0;
}
