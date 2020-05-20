#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

void main(int argc, char* argv[])
{
    struct hostent* hostEnt;
    struct in_addr inAddress;

    int i = 0;

    if(argc < 3)
    {
        printf("Usage : %s host_name(도메인) dotted_decimal(10진수 주소)\n", argv[0]);
        exit(1);
    }

    hostEnt = gethostbyname(argv[1]);
    if(hostEnt == NULL)
    {
        printf("Can Not gethostbynama()\n");
        exit(1);
    }

    inAddress.s_addr = *((u_long*)(hostEnt->h_addr));
    printf("%s's binary IP address (hexa) : \t%x\n", argv[1], inAddress.s_addr);
    printf("%s's dotted decimal IP address : \t%s\n\n", argv[1], inet_ntoa(inAddress));

    inAddress.s_addr = inet_addr(argv[2]);
    if(inAddress.s_addr == 0)
    {
        printf("ERROR : inAddress = inet_addr(argv[2])");
        exit(1);
    }
    hostEnt = gethostbyaddr((char*)&(inAddress.s_addr), sizeof(inAddress.s_addr), AF_INET);

    printf("%s's binary IP address (hexa) : \t%x\n", argv[2], inAddress.s_addr);
    printf("%s's hostname : \t\t\t%s\n", argv[2], hostEnt->h_name);

}
