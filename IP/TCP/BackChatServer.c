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
 
#define MSG_LENGTH 1024
#define MAX_CLIENT 50

int readLine(int, char*, int);
 
int main(int argc, char *argv[]){
    int sockfd, clientFd;
    struct sockaddr_in cliaddr, servaddr;
    int addrlen;
    fd_set read_fdset;
 
    int client_fd[MAX_CLIENT];
    int client_num = 0;
    char buf[MSG_LENGTH];
    int msg_len, maxfdp1, nready, i;
 
    if(argc != 2){
        printf("Usage: echo_ser_select[PortNumber]\n");
        exit(0);
    }
 
    //소켓 생성
    if((sockfd = socket(AF_INET, SOCK_STREAM,0))<0){
        printf("server: can't create socket\n");
        exit(-1);
    }
     
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//wild card
    servaddr.sin_port = htons(atoi(argv[1]));
 
    //바인딩: 소켓에 포트번호를 부여한다.
    if((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0){
        printf("server: bind error\n");
        exit(-1);
    }
 
    //소켓을 Listen상태로 전환하여 클라이언트의 접속을 기다린다.
    //대기는 최대 5개
    listen(sockfd, 5);
    //클라이언트 목록을 초기화 한다. -1이면 없는것.
    for(i = 0; i < MAX_CLIENT; i++)
    {
        client_fd[i] = -1;
    }
     
    //read_fdset의 비트필드를 0으로 초기화 한다.
    FD_ZERO(&read_fdset);
    //관리하게될 소켓fd의 수를 등록. 1을 더해줘야 한다.
    maxfdp1 = sockfd + 1;
 
    while(1)
    {
        /***************************************
          접속 설정관련
        ***************************************/
        /*비트필드에 sockfd값을 추가한다. 대응되는 필드의 값이 1로 변경된다.
        0들로 초기화된 비트필드에 대응대는 값들이 1로 변경될 것이다.
        while문이 한바퀴 돌때마다 한번씩 수행해 주어야 문제가 되지 않는다.*/
        FD_SET(sockfd, &read_fdset);
        /*접속된 클라이언트들의 fd정보들도 모두 set 해준다.
        비트필드에 대응되는 값들이 1로 변경된다.*/
        for(i = 0; i < client_num; i++)
        {
            if(client_fd[i] != -1)
            {
                FD_SET(client_fd[i], &read_fdset);
            }
        }
         
        //select()를 이용해 감지된 소켓기술자가 있는지 확인한다.
        //timeout이 NULL이면 select 함수는 무한히 봉쇄될 수 있다.
        if(select(maxfdp1, &read_fdset, NULL, NULL, NULL) < 0){
            printf("select error...\n");
            exit(-1);
        }
 
        //sockfd에 변경사항이 있는지 검사한다. 리턴이 1이면 참이다.
        if(FD_ISSET(sockfd, &read_fdset))
        {
            //sockfd에 변경사항이 있는경우: 클라이언트가 접속했다.
            addrlen = sizeof(cliaddr);
            //클라이언트가 접속하였으므로 accept 하여 그 정보를 clientFd에 저장한다.
            clientFd = accept(sockfd, (struct sockaddr*)&cliaddr, &addrlen);
            if(clientFd < 0)
            {
                printf("Server accept error\n");
                exit(-1);
            }
             
            //클라이언트 수가 최대 허용치를 초과했는지 확인
            if(client_num >= MAX_CLIENT)
            {
                printf("too many clients\n");
                close(clientFd);
                continue;
            }
            printf("Connect new client(clientFd=%d)...\n", clientFd);
 
            /************************************
                새 클라이언트를 등록한다.
            *************************************/
            for(i = 0; i < MAX_CLIENT; i++)
            {
                if(client_fd[i] != -1) continue;
                //accept된 클라이언트의 정보를 갱신한다.
                client_fd[i] = clientFd;
                maxfdp1++;
                client_num++;
                break;
            }

            printf("client_num : %d\n", client_num);
            for(int k = 0; k < client_num; k++)
            {
                printf("client_fd[%d] : %d\n", k, client_fd[k]);
            }
        }
         
        /*************************************************
            클라이언트의 요청을 처리하는 부분
        **************************************************/
        //접속된 클라이언트의 수만큼 돌면서 FD_ISSET으로 해당 클라이언트의 요청을 감지한다.
        //변동이 있는 부분을 찾아서 해당 소켓의 요청을 처리해 준다.

        for(i = 0; i < client_num; i++)
        {
            if(FD_ISSET(client_fd[i], &read_fdset))
            {
                memset(buf, 0x00, MSG_LENGTH);
                //클라이언트의 요청이 있었으므로 처리해준다.
                //클라이언트 요청을 버퍼에 읽는다.
                if(readLine(client_fd[i], buf, MSG_LENGTH) > 0)
                {
                    printf("Client%d : %s\n", i, buf);
                    if(strstr(buf, "quit") || (strcmp(buf, "\n")) == 0)
                    {
printf("Server : Client%d quit\n", i);
                        shutdown(client_fd[i], SHUT_RDWR);
                        if(i != client_num - 1)
                        {
                            client_fd[i] = client_fd[client_num - 1];
                        }
                        client_fd[client_num - 1] = -1;
                        client_num--;
                        memset(buf, 0x00, MSG_LENGTH);
// strcat(buf, "client quit");
// printf("buf : %s(%ld)\n", buf, strlen(buf));
printf("client_num : %d\n", client_num);
for(int k = 0; k < client_num; k++)
{
    printf("client_fd[%d] : %d\n", k, client_fd[k]);
}
                        continue;
                    }
                    //클라이언트에 전송
                    for(int j = 0; j < client_num; j++)
                    {
                        if(client_fd[j] != -1)
                        {
                            if(write(client_fd[j], buf, strlen(buf)) < 0)
                            {
                                printf("server: write error!!\n");
                                close(clientFd);
                                exit(0);
                            }
printf("send message to %d\n", client_fd[j]);
                        }
                    }
                }
            }
        }
    }
    close(sockfd);

    return 0;
}

int readLine(int fd, char* buffer, int length)
{
// printf("\t\t(i) readLine execute(%d)\n", fd);
    char c;
    int i;

    for(i = 0; i < length; i++)
    {
        if(read(fd, (char*)&c, 1) != -1)
        {
            if(c == '\n' || c == '\0') 
            {
// printf("\t\t(i) buffer find '\\0'\n");
                buffer[i] = '\n';
                break;
            }
// printf("\t\t(i) buffer find %c\n", c);
            buffer[i] = c;
        }
        else
        {
            printf("ERROR : read() in readLine\n");
            return -1;
        }
    }
// printf("\t\t(i) readLine result : %s(%d)\n", buffer, i);

    return i;
}