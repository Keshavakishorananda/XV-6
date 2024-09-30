#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define IPadrress "127.0.0.1"
#define PORT1 12346
#define PORT2 54321

void server()
{
    char buffer1[1024];
    char buffer2[1024];

    int serversocket1, serversocket2;
    struct sockaddr_in serveraddr1, clientaddr1;
    struct sockaddr_in serveraddr2, clientaddr2;

    serversocket1 = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    serversocket2 = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);


    serveraddr1.sin_family = AF_INET;
    serveraddr1.sin_port = htons(PORT1);
    serveraddr1.sin_addr.s_addr = inet_addr(IPadrress);

    serveraddr2.sin_family = AF_INET;
    serveraddr2.sin_port = htons(PORT2);
    serveraddr2.sin_addr.s_addr = inet_addr(IPadrress);

    int bindstatus1;
    if((bindstatus1 = bind(serversocket1,(struct sockaddr*)&serveraddr1,sizeof(serveraddr1)))<0)
    {
        perror("bind is failed");
        close(serversocket1);
    }

    int bindstatus2;
    if((bindstatus2 = bind(serversocket2,(struct sockaddr*)&serveraddr2,sizeof(serveraddr2)))<0)
    {
        perror("bind is failed");
        close(serversocket2);
    }

    while(1)
    {
        int msgsize1, msgsize2;
        socklen_t lenclient1 = sizeof(clientaddr1);
        if((msgsize1 = recvfrom(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,&lenclient1))<0)
        {
            perror("recvfrom");
            close(serversocket1);
        }
        printf("message from clienta : %s",buffer1);

        socklen_t lenclient2 = sizeof(clientaddr2);
        if((msgsize2 = recvfrom(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,&lenclient2))<0)
        {
            perror("recvfrom");
            close(serversocket2);
        }
        printf("message from clientb : %s",buffer2);

        if(strcmp(buffer1,"yes\n")==0 && strcmp(buffer2,"yes\n")==0)
        {
            if(msgsize1>0 && msgsize2>0)
            {
                strcpy(buffer1,"start");
                strcpy(buffer2,"start");
                sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                if(recvfrom(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,&lenclient1)<0)
                {
                    perror("recv is failed");
                }
                if(recvfrom(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,&lenclient2)<0)
                {
                    perror("recv is failed");
                }

                if(strcmp(buffer1,buffer2)==0)
                {
                    strcpy(buffer1,"Draw");
                    strcpy(buffer2,"Draw");
                    sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                    sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                }
                else if(strcmp(buffer1,"rock\n")==0 && strcmp(buffer2,"paper\n")==0)
                {
                    strcpy(buffer1,"Lose");
                    strcpy(buffer2,"Win");
                    sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                    sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                }
                else if(strcmp(buffer1,"rock\n")==0 && strcmp(buffer2,"scissor\n")==0)
                {
                    strcpy(buffer1,"Win");
                    strcpy(buffer2,"Lose");
                    sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                    sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                }
                else if(strcmp(buffer1,"paper\n")==0 && strcmp(buffer2,"scissor\n")==0)
                {
                    strcpy(buffer1,"Lose");
                    strcpy(buffer2,"Win");
                    sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                    sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                }
                else if(strcmp(buffer2,"rock\n")==0 && strcmp(buffer1,"paper\n")==0)
                {
                    strcpy(buffer1,"Win");
                    strcpy(buffer2,"Lose");
                    sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                    sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                }
                else if(strcmp(buffer2,"rock\n")==0 && strcmp(buffer1,"scissor\n")==0)
                {
                    strcpy(buffer1,"Lose");
                    strcpy(buffer2,"Win");
                    sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                    sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                }
                else if(strcmp(buffer2,"paper\n")==0 && strcmp(buffer1,"scissor\n")==0)
                {
                    strcpy(buffer1,"Win");
                    strcpy(buffer2,"Lose");
                    sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
                    sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
                }
            }
        }
        else
        {
            strcpy(buffer1,"end");
            strcpy(buffer2,"end");
            sendto(serversocket1,buffer1,sizeof(buffer1),0,(struct sockaddr*)&clientaddr1,sizeof(clientaddr1));
            sendto(serversocket2,buffer2,sizeof(buffer2),0,(struct sockaddr*)&clientaddr2,sizeof(clientaddr2));
            break;
        }
    }

    close(serversocket1);
    close(serversocket2);

}

int main()
{
    server();

    return 0;
}