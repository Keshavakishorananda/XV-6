#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define IPadrress "127.0.0.1"
#define PORT 12346

void client()
{
    char buffer[1024];
    int clientsocket;
    struct sockaddr_in serveraddr;

    clientsocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(clientsocket < 0)
    {
        perror("socket");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(IPadrress);

    fgets(buffer,sizeof(buffer),stdin);

    int sendstatus = sendto(clientsocket,buffer,sizeof(buffer),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(sendstatus<0)
    {
        perror("send");
    }

    int recvstatus = recvfrom(clientsocket,buffer,sizeof(buffer),0,NULL,NULL);
    if(recvstatus<0)
    {
        perror("recv");
    }

    printf("message from server: %s",buffer);

    int confirm = close(clientsocket);
    if(confirm < 0)
    {
        perror("close");
    }
}

int main()
{
    client();

    return 0;
}
