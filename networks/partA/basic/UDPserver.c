#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define IPadrress "127.0.0.1"
#define PORT 12346

void server()
{
    char buffer[1024];

    int serversocket, clientsocket;
    struct sockaddr_in serveraddr, clientaddr;

    serversocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(serversocket < 0)
    {
        perror("socket");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(IPadrress);

    int bindstatus;
    if((bindstatus = bind(serversocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)))<0)
    {
        perror("bind is failed");
        close(serversocket);
    }

    int msgsize;
    socklen_t lenclient = sizeof(clientaddr);
    if((msgsize = recvfrom(serversocket,buffer,sizeof(buffer),0,(struct sockaddr*)&clientaddr,&lenclient))<0)
    {
        perror("recvfrom");
        close(serversocket);
    }

    if(msgsize>0)
    {
        int sendstatus = sendto(serversocket,buffer,sizeof(buffer),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
        if(sendstatus < 0 )
        {
            perror("sendto");
        }
    }

    int confirm  = close(serversocket);
    if(confirm < 0)
    {
        perror("close");
    }
}

int main()
{
    server();

    return 0;
}