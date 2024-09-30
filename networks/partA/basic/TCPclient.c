#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define Ipaddr "127.0.0.1"

void client()
{
    char buffer[1024];

    int clientsocket = socket(AF_INET,SOCK_STREAM,0);
    if(clientsocket<0)
    {
        perror("socket");
    }

    struct sockaddr_in serveraddr;

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(54321);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int connectstatus;
    if((connectstatus = connect(clientsocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)))==-1)
    {
        printf("connection is not established\n");
        perror("connect is failed");
        close(clientsocket);
    }


    fgets(buffer,sizeof(buffer),stdin);

    int sendstatus = send(clientsocket,buffer,sizeof(buffer),0);
    if(sendstatus<0)
    {
        perror("send");
    }

    int recvstatus = recv(clientsocket,buffer,sizeof(buffer),0);
    if(recvstatus<0)
    {
        perror("recv");
    }

    printf("message from server:%s",buffer);

    int confirm = close(clientsocket);
    if(confirm<0)
    {
        perror("close");
    }
}

int main()
{
    client();

    return 0;
}