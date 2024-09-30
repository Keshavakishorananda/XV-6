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

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(IPadrress);

    while(1)
    {
        printf("Let start the game[yes/no] : ");
        fgets(buffer,sizeof(buffer),stdin);

        int sendstatus = sendto(clientsocket,buffer,sizeof(buffer),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
        if(sendstatus<0)
        {
            perror("send");
        }
        int recvstatus = recvfrom(clientsocket,buffer,sizeof(buffer),0,NULL,NULL);

        if(strcmp(buffer,"end")==0)
        {
            break;
        }

        memset(buffer,0,sizeof(buffer));
        printf("What do you choose[rock/paper/scissor] : ");
        fgets(buffer,sizeof(buffer),stdin);
        sendstatus = sendto(clientsocket,buffer,sizeof(buffer),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

        recvstatus = recvfrom(clientsocket,buffer,sizeof(buffer),0,NULL,NULL);
        if(recvstatus<0)
        {
            perror("recv");
        }

        printf("message from server: %s\n",buffer);
        printf("------------------------------------\n");
    }

    close(clientsocket);

}

int main()
{
    client();

    return 0;
}
