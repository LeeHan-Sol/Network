#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 128

void main(int argc, char *argv[])
{
    struct sockaddr_in serverFd;
    int s, n;
    char buff[BUFF_SIZE];

    if(argc != 2)
    {
        printf("Usage : %s IP Address\n", argv[0]);
        exit(1);
    }

    if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Can't create socket.\n");
        exit(1);
    }

    memset((char*)&serverFd, 0x00, sizeof(serverFd));
    
    serverFd.sin_family = AF_INET;
    serverFd.sin_addr.s_addr = inet_addr(argv[1]);
    serverFd.sin_port = htonl(13);

    if(connect(s, (struct sockaddr *)&serverFd, sizeof(serverFd)) < 0)
    {
        printf("Can't connect.\n");
        exit(1);
    }
    
    while((n = read(s, buff, BUFF_SIZE)) > 0)
    {
        buff[n] = '\0';
        printf("%s", buff);
    }

    close(s);
}
