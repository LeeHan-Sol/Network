#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

void main()
{
    struct servent* pmyServent;

    pmyServent = getservbyname("echo", "udp");

    if(pmyServent == NULL)
    {
        printf("서비스 정보를 얻을 수 없습니다.\n");
        exit(1);
    }

    printf("Port number of 'echo', 'udp' with ntohs(): %x\n", ntohs(pmyServent->s_port));
    printf("Port number of 'echo', 'udp' without ntohs(): %x\n", pmyServent->s_port);

    exit(0);
}
