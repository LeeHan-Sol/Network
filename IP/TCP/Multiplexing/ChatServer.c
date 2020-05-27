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
    int sockfd = 0, clientFd = 0;
    struct sockaddr_in client_address, server_address;
	memset(&client_address, 0x00, sizeof(struct sockaddr_in));
	memset(&server_address, 0x00, sizeof(struct sockaddr_in));

    int addrlen = 0;
    fd_set read_fdset;
 
    int client_fd[MAX_CLIENT] = {0,};
    int client_num = 0;
    char buf[MSG_LENGTH] = {0,};
    int msg_len = 0, maxfdp1 = 0, nready = 0;
 
    if(argc != 2){
        fprintf(stdout,"Usage: ./ChatServer Port\n");
        exit(0);
    }
 
    //소켓 생성
    if((sockfd = socket(AF_INET, SOCK_STREAM,0))<0){
        fprintf(stdout,"server: can't create socket\n");
        exit(-1);
    }
     
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);//wild card
    server_address.sin_port = htons(atoi(argv[1]));
 
    if((bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address))) < 0){
        fprintf(stdout,"server: bind error\n");
        exit(-1);
    }
 
    //소켓을 Listen상태로 전환하여 클라이언트의 접속을 기다립니다.
    //대기는 최대 5개
    listen(sockfd, 5);
    //클라이언트 목록을 초기화합니다. -1이면 비어있는 상태입니다..
    for(int i = 0; i < MAX_CLIENT; i++)
    {
        client_fd[i] = -1;
    }
     
    //read_fdset의 비트필드를 0으로 초기화합니다.
    FD_ZERO(&read_fdset);
    //관리하게될 소켓fd의 수를 등록. 1을 더합니다.
    maxfdp1 = sockfd + 1;
 
    while(1)
    {
        /***************************************
          접속 설정관련
        ***************************************/
        /*비트필드에 sockfd값을 추가합니다. 대응되는 필드의 값이 1로 변경됩니다.
        0들로 초기화된 비트필드에 대응대는 값들이 1로 변경됩니다.
        while문이 한바퀴 돌때마다 한번씩 수행해야합니다.*/
        FD_SET(sockfd, &read_fdset);
        /*접속된 클라이언트들의 fd정보들도 모두 set 해줍니다.
        비트필드에 대응되는 값들이 1로 변경됩니다.*/
        for(int i = 0; i < client_num; i++)
        {
            if(client_fd[i] != -1)
            {
                FD_SET(client_fd[i], &read_fdset);
            }
        }
         
        //select()를 이용해 감지된 소켓기술자가 있는지 확인합니다.
        //timeout이 NULL이면 select 함수는 무한히 봉쇄될 수 있습니다.
        if(select(maxfdp1, &read_fdset, NULL, NULL, NULL) < 0){
            fprintf(stdout,"select error...\n");
            exit(-1);
        }
 
        //sockfd에 변경사항이 있는지 검사합니다. 리턴이 1이면 참입니다.
        if(FD_ISSET(sockfd, &read_fdset))
        {
            //sockfd에 변경사항이 있는경우: 클라이언트가 접속
            addrlen = sizeof(client_address);
            //클라이언트가 접속하였으므로 accept 하여 그 정보를 clientFd에 저장합니다.
            clientFd = accept(sockfd, (struct sockaddr*)&client_address, &addrlen);
            if(clientFd < 0)
            {
                fprintf(stdout,"Server accept error\n");
                exit(-1);
            }
             
            //클라이언트 수가 최대 허용치를 초과했는지 확인합니다.
            if(client_num >= MAX_CLIENT)
            {
                fprintf(stdout,"too many clients\n");
                close(clientFd);
                continue;
            }
            fprintf(stdout,"Connect new client(clientFd=%d)...\n", clientFd);
 
            /************************************
                새 클라이언트를 등록합니다.
            *************************************/
            for(int i = 0; i < MAX_CLIENT; i++)
            {
                if(client_fd[i] != -1) continue;
                //accept된 클라이언트의 정보를 갱신합니다.
                client_fd[i] = clientFd;
                maxfdp1++;
                client_num++;
                break;
            }

            fprintf(stdout,"client_num : %d\n", client_num);
            for(int k = 0; k < client_num; k++)
            {
                fprintf(stdout,"client_fd[%d] : %d\n", k, client_fd[k]);
            }
        }
         
        /*************************************************
            클라이언트의 요청을 처리하는 부분
        **************************************************/
        //접속된 클라이언트의 수만큼 돌면서 FD_ISSET으로 해당 클라이언트의 요청을 감지합니다.
        //변동이 있는 부분을 찾아서 해당 소켓의 요청을 처리합니다.

        for(int i = 0; i < client_num; i++)
        {
            if(FD_ISSET(client_fd[i], &read_fdset))
            {
                memset(buf, 0x00, MSG_LENGTH);
                //클라이언트의 요청을 처리합니다.
                if(readLine(client_fd[i], buf, MSG_LENGTH) > 0)
                {
                    fprintf(stdout,"Client%d : %s\n", i, buf);
                    if(strstr(buf, "quit") || (strcmp(buf, "\n")) == 0)
                    {
fprintf(stdout,"Server : Client%d quit\n", i);
                        shutdown(client_fd[i], SHUT_RDWR);
                        if(i != client_num - 1)
                        {
                            client_fd[i] = client_fd[client_num - 1];
                        }
                        client_fd[client_num - 1] = -1;
                        client_num--;
                        memset(buf, 0x00, MSG_LENGTH);
fprintf(stdout,"client_num : %d\n", client_num);
for(int k = 0; k < client_num; k++)
{
    fprintf(stdout,"client_fd[%d] : %d\n", k, client_fd[k]);
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
                                fprintf(stdout,"server: write error!!\n");
                                close(clientFd);
                                exit(0);
                            }
fprintf(stdout,"send message to %d\n", client_fd[j]);
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
    char c;
	int i = 0;

    for(; i < length; i++)
    {
        if(read(fd, (char*)&c, 1) != -1)
        {
            if(c == '\n' || c == '\0') 
            {
                buffer[i] = '\n';
                break;
            }
            buffer[i] = c;
        }
        else
        {
            fprintf(stdout,"ERROR : read() in readLine\n");
            return -1;
        }
    }

    return i;
}
