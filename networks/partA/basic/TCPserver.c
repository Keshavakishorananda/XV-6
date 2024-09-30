#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 54321

void server()
{
    char buffer[1024];
    int serversocket = socket(PF_INET,SOCK_STREAM,0);
    if(serversocket<0)
    {
        perror("socket");
    }
    int clientsocket;

    struct sockaddr_in servertcp;
    struct sockaddr_in clienttcp;

    servertcp.sin_family = AF_INET;
    servertcp.sin_port = htons(PORT);
    servertcp.sin_addr.s_addr = inet_addr("127.0.0.1");

    int bindstatus = bind(serversocket,(struct sockaddr*)&servertcp,sizeof(servertcp));
    if(bindstatus==-1)
    {
        printf("Not able to bind\n");
        perror("bind");
        int confirm = close(serversocket);
        if(confirm < 0)
        {
            perror("close");
        }
    }

    int listenstatus = listen(serversocket,5);
    if(listenstatus==-1)
    {
        printf("Server is unable to listen\n");
        perror("listen");
        int confirm = close(serversocket);
        if(confirm < 0)
        {
            perror("close");
        }
    }

    socklen_t lenclientsock = sizeof(clienttcp);
    if((clientsocket = accept(serversocket,(struct sockaddr*)&clienttcp,&lenclientsock))==-1)
    {
        int confirm = close(serversocket);
        if(confirm < 0)
        {
            perror("close");
        }
    }


    memset(buffer, 0, sizeof(buffer));
    int msgsize;
    if((msgsize = recv(clientsocket,buffer,sizeof(buffer),0))<0)
    {
        printf("Nothing is received\n");
        perror("recv is failed");
    }

    printf("message from client: %s",buffer);


    if(send(clientsocket,buffer,sizeof(buffer),0)<0)
    {
        perror("send is failed");
    }


    int cliclose = close(clientsocket);
    if(cliclose < 0)
    {
        perror("close");
    }
    int serclose = close(serversocket);
    if(serclose < 0)
    {
        perror("close");
    }
}

int main()
{
    server();

    return 0;
}