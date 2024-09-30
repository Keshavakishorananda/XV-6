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

    struct sockaddr_in serveraddr;

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int connectstatus;
    if((connectstatus = connect(clientsocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)))==-1)
    {
        printf("connection is not established\n");
        perror("connect is failed");
        close(clientsocket);
    }

    while(1)
    {
        printf("Let us start the game[yes/no] : ");
        fgets(buffer,sizeof(buffer),stdin);

        // if(strcmp(buffer,"no\n")==0)
        // {
        //     break;
        // }

        int sendstatus = send(clientsocket,buffer,sizeof(buffer),0);
        int recvstatus = recv(clientsocket,buffer,sizeof(buffer),0);

        if(strcmp(buffer,"end")==0)
        {
            break;
        }

        memset(buffer,0,sizeof(buffer));
        printf("What do you choose[rock/paper/scissor] : ");
        fgets(buffer,sizeof(buffer),stdin);

        sendstatus = send(clientsocket,buffer,sizeof(buffer),0);

        recvstatus = recv(clientsocket,buffer,sizeof(buffer),0);

        printf("message from server:%s\n",buffer);

        printf("---------------------------------\n");
    }

    close(clientsocket);
}

int main()
{
    client();

    return 0;
}