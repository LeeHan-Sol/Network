#include <stdio.h>
#include <stdlib.h>
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

    if(argc < 2)
    {
        printf("Usage : %s host_name(도메인)\n", argv[0]);
        exit(1);
    }

    hostEnt = gethostbyname(argv[1]);
    if(hostEnt == 0)
    {
        printf("ERROR : Can Not Execute gethostbyname()\n");
        exit(1);
    }

    printf("host name : \t\t%s\n", hostEnt->h_name);

    while(hostEnt->h_aliases[i] != NULL)
    {
        printf("aliases name : \t\t%s\n", hostEnt->h_aliases[i]);
        i++;
    }

    printf("host address type : \t\t%x\n", hostEnt->h_addrtype);

    printf("length of host address : \t%d\n", hostEnt->h_length);

    i = 0;
    while(hostEnt->h_addr_list[i] != NULL)
    {
        inAddress.s_addr = *((u_long*)(hostEnt->h_addr_list[i]));
        printf("address for host :\t\t%s\n", inet_ntoa(inAddress));
        i++;
    }
}
