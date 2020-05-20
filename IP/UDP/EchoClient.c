#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int serverFD;
    int n;
    char *ipAddress, *port;
    char buffer[128];
    struct sockaddr_in serverAddr;
    FILE *fileInput;
    
    if((fileInput = fopen("Count10k.txt", "r")) < 0)
    {
        printf("(I) ERROR : fopen()\n");
        exit(1);
    }

    if(argc != 3)
    {
        printf("Usage : ./EchoServer IP_ADDRESS PORT\n");
        exit(1);
    }
    ipAddress = argv[1];
    port = argv[2];

    /*소켓 생성*/
    if((serverFD = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("(I) ERROR : socket()\n");
        exit(1);
    }
    
    /*서버의 소켓주소 구조체 serverAddr을 '\0'으로 초기화*/
    memset(&serverAddr, 0, sizeof(serverAddr));
    /*serverAddr의 주소 지정*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress);
    serverAddr.sin_port = htons(atoi(argv[2]));

    while(1)
    {
        memset(buffer, 0x00, sizeof(buffer));

        if((fgets(buffer, sizeof(buffer), fileInput)) < 0) 
        {
            printf("(I) 파일로부터 입력이 없습니다.\n");
            break;
        }

        fputs(buffer, stdout);

        if((n = sendto(serverFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) <= 0)
        {
            printf("(I) 전송 완료\n");
            break;
        }

        if(strstr(buffer, "10000")) break;

    }
        //recvfrom(serverFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    fputs("(I) Complete sending data\n", stdout);

    fclose(fileInput);
    close(serverFD);

    return 0;
}
