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
#include <sys/select.h>
#include <sys/time.h>
 
//메시지 길이 지정
#define BUFFER_SIZE 1024

typedef struct Name
{
    char name[32];
    int length;
}Name;

int readLine(int, char*, int);
 
int main(int argc, char *argv[]){
    struct sockaddr_in serverAddress;
    Name userName;
    int socketFd; //소켓 File Descriptor 선언
 
    fd_set read_fdset;
 
    char buf[BUFFER_SIZE], writeLine[BUFFER_SIZE];
    int len, msg_len, rcvlen, maxfdp1;
 
    if(argc != 4)
    {
        printf("Usage: echo_cli_select Server_IP Port_number Name\n");
        exit(0);
    }
    sprintf(userName.name, "[%s]", argv[3]);
    userName.length = strlen(userName.name);

    if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("client: can't create socket\n");
        exit(-1);
    }
    //sockaddr_in 구조체를 0으로 초기화 후 소켓 정보 입력
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(atoi(argv[2]));

    //서버와의 연결 시도
    if (connect(socketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
        printf("client: can't connect\n");
        exit(-1);
    }
    //read_fdset 비트 초기화
    FD_ZERO(&read_fdset);
    
    //관리하게될 소켓fd의 수를 등록(최대 + 1)
    maxfdp1 = socketFd + 1;
    while(1){
        /*키보드 입력용 fd를 초기화
        0번은 키보드 입력을 의미*/
        FD_SET(0, &read_fdset);
        FD_SET(socketFd, &read_fdset);

        if (select(maxfdp1, &read_fdset, NULL, NULL, NULL) < 0){
            printf("select error..\n");
            exit(-1);
        }
        /*****************************************
        키보드 입력 요청이 있는지 확인한다.
        ******************************************/
        if (FD_ISSET(0, &read_fdset))
        {
            //버퍼를 초기화 하고 키보드로부터 값을 입력받는다.
            memset(buf, 0x00, sizeof(buf));
            msg_len = readLine(0, buf, BUFFER_SIZE);
printf("\t\t(i) buf after readLine : %s\n", buf);

            memset(writeLine, 0x00, BUFFER_SIZE);
            sprintf(writeLine, "%s %s", userName.name, buf);

            //키보드로부터 입력받은 내용을 서버에 전송한다.
            if(write(socketFd, writeLine, BUFFER_SIZE) < 0)
            {
                printf("client: write error!!\n");
                close(socketFd);
                exit(0);
 
            }
printf("\t\t(i) writeLine : %s\n", writeLine);

            if((strcmp(buf, "quit")) == 0)
            {
                printf("\t\t[Server] Goodbye\n");
                break;
            }
        }
        /******************************************
        서버로부터 메시지가 있는지 확인한다.
        *******************************************/
        if (FD_ISSET(socketFd, &read_fdset))
        {
            //수신버퍼 초기화
            memset(buf, 0x00, sizeof(buf));
            //서버로부터 echo된 메시지를 수신한다.
            if (readLine(socketFd, buf, BUFFER_SIZE) < 0)
            {
                close(socketFd);
                exit(1);
            }
            //echo되어 돌아온 메시지를 출력해 준다.
            printf("%s\n", buf);
        }
    }
    close(socketFd);
    return 0;
}

int readLine(int fd, char* buffer, int length)
{
printf("\t\t(i) readLine execute\n");
    char c;
    int i;

    for(i = 0; i < length; i++)
    {
        if(read(fd, (char*)&c, 1) != -1)
        {
            if(c == '\n' || c == '\0') 
            {
printf("\t\t(i) buffer find '\\0'\n");
                buffer[i] = '\0';
                break;
            }
printf("\t\t(i) buffer find %c\n", c);
            buffer[i] = c;
        }
        else
        {
            printf("ERROR : read() in readLine\n");
            return -1;
        }
    }
printf("\t\t(i) readLine result : %s(%d)\n", buffer, i);

    return i;
}