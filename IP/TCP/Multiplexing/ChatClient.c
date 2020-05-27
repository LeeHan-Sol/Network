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
 
#define BUFFER_SIZE 1024

typedef struct Name
{
    char name[32];
    int length;
}Name;

int readLine(int, char*, int);
 
int main(int argc, char *argv[]){
    struct sockaddr_in server_address;
    Name userName;
	memset(&userName, 0x00, sizeof(Name));
    int socketFd = 0; //소켓 File Descriptor 선언
 
    fd_set read_fdset;
 
    char buf[BUFFER_SIZE] = {0,}, writeLine[BUFFER_SIZE] = {0,};
    int len = 0, msg_len = 0, rcvlen = 0, maxfdp1 = 0;
 
    if(argc != 4)
    {
        fprintf(stderr, "Usage: ./ChatClient Server_IP Port_number Name\n");
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
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    if (connect(socketFd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
	{
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
        키보드 입력 요청이 있는지 확인합니다.
        ******************************************/
        if (FD_ISSET(0, &read_fdset))
        {
            //버퍼를 초기화 하고 키보드로부터 값을 입력합니다.
            memset(buf, 0x00, sizeof(buf));
            msg_len = readLine(0, buf, BUFFER_SIZE);

            memset(writeLine, 0x00, BUFFER_SIZE);
            sprintf(writeLine, "%s %s", userName.name, buf);

            //키보드로부터 입력받은 내용을 서버에 전송합니다.
            if(write(socketFd, writeLine, BUFFER_SIZE) < 0)
            {
                printf("client: write error!!\n");
                close(socketFd);
                exit(0);
 
            }

            if((strcmp(buf, "quit")) == 0)
            {
                printf("\t\t[Server] Goodbye\n");
                break;
            }
        }
        /******************************************
        서버로부터 메시지가 있는지 확인합니다.
        *******************************************/
        if (FD_ISSET(socketFd, &read_fdset))
        {
            //수신버퍼 초기화
            memset(buf, 0x00, sizeof(buf));
            //서버로부터 echo된 메시지를 수신합니다.
            if (readLine(socketFd, buf, BUFFER_SIZE) < 0)
            {
                close(socketFd);
                exit(1);
            }
            //echo되어 돌아온 메시지를 출력합니다.
            printf("%s\n", buf);
        }
    }

    close(socketFd);
    return 0;
}

int readLine(int fd, char* buffer, int length)
{
    char c;
    int i;

    for(i = 0; i < length; i++)
    {
        if(read(fd, (char*)&c, 1) != -1)
        {
            if(c == '\n' || c == '\0') 
            {
                buffer[i] = '\0';
                break;
            }
            buffer[i] = c;
        }
        else
        {
            printf("ERROR : read() in readLine\n");
            return -1;
        }
    }

    return i;
}
