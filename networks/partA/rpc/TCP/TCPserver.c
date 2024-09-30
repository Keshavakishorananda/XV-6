#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT1 12345
#define PORT2 54321

void server()
{
    char buffer1[1024];
    char buffer2[1024];
    int serversocket1 = socket(PF_INET,SOCK_STREAM,0);
    int serversocket2 = socket(PF_INET,SOCK_STREAM,0);
    int clientsocket1;
    int clientsocket2;

    struct sockaddr_in servertcp1, servertcp2;
    struct sockaddr_in clienttcp1, clienttcp2;

    servertcp1.sin_family = AF_INET;
    servertcp1.sin_port = htons(PORT1);
    servertcp1.sin_addr.s_addr = inet_addr("127.0.0.1");

    servertcp2.sin_family = AF_INET;
    servertcp2.sin_port = htons(PORT2);
    servertcp2.sin_addr.s_addr = inet_addr("127.0.0.1");

    int bindstatus1 = bind(serversocket1,(struct sockaddr*)&servertcp1,sizeof(servertcp1));
    if(bindstatus1==-1)
    {
        printf("Not able to bind\n");
        perror("bind");
        close(serversocket1);
    }
    int bindstatus2 = bind(serversocket2,(struct sockaddr*)&servertcp2,sizeof(servertcp2));
    if(bindstatus2==-1)
    {
        printf("Not able to bind\n");
        perror("bind");
        close(serversocket2);
    }

    int listenstatus1 = listen(serversocket1,5);
    if(listenstatus1==-1)
    {
        printf("Server is unable to listen\n");
        perror("listen");
        close(serversocket1);
    }
    int listenstatus2 = listen(serversocket2,5);
    if(listenstatus2==-1)
    {
        printf("Server is unable to listen\n");
        perror("listen");
        close(serversocket2);
    }

    socklen_t lenclientsock1 = sizeof(clienttcp1);
    socklen_t lenclientsock2 = sizeof(clienttcp2);
    if((clientsocket1 = accept(serversocket1,(struct sockaddr*)&clienttcp1,&lenclientsock1))==-1)
    {
        close(serversocket1);
    }
    if((clientsocket2 = accept(serversocket2,(struct sockaddr*)&clienttcp2,&lenclientsock2))==-1)
    {
        close(serversocket2);
    }

    while(1)
    {
        memset(buffer1, 0, sizeof(buffer1));
        int msgsize1;
        if((msgsize1 = recv(clientsocket1,buffer1,sizeof(buffer1),0))<0)
        {
            printf("Nothing is received\n");
            perror("recv is failed");
        }

        printf("message from clienta: %s",buffer1);

        memset(buffer2, 0, sizeof(buffer2));
        int msgsize2;
        if((msgsize2 = recv(clientsocket2,buffer2,sizeof(buffer2),0))<0)
        {
            printf("Nothing is received\n");
            perror("recv is failed");
        }

        printf("message from clientb: %s",buffer2);

        if(strcmp(buffer1,"yes\n")==0 && strcmp(buffer2,"yes\n")==0)
        {
            strcpy(buffer1,"start");
            strcpy(buffer2,"start");
            send(clientsocket1,buffer1,sizeof(buffer1),0);
            send(clientsocket2,buffer2,sizeof(buffer2),0);
            if(recv(clientsocket1,buffer1,sizeof(buffer1),0)<0)
            {
                perror("recv is failed");
            }
            if(recv(clientsocket2,buffer2,sizeof(buffer2),0)<0)
            {
                perror("recv is failed");
            }

            if(strcmp(buffer1,buffer2)==0)
            {
                strcpy(buffer1,"Draw");
                strcpy(buffer2,"Draw");
                send(clientsocket1,buffer1,sizeof(buffer1),0);
                send(clientsocket2,buffer2,sizeof(buffer2),0);
            }
            else if(strcmp(buffer1,"rock\n")==0 && strcmp(buffer2,"paper\n")==0)
            {
                strcpy(buffer1,"Lose");
                strcpy(buffer2,"Win");
                send(clientsocket1,buffer1,sizeof(buffer1),0);
                send(clientsocket2,buffer2,sizeof(buffer2),0);
            }
            else if(strcmp(buffer1,"rock\n")==0 && strcmp(buffer2,"scissor\n")==0)
            {
                strcpy(buffer1,"Win");
                strcpy(buffer2,"Lose");
                send(clientsocket1,buffer1,sizeof(buffer1),0);
                send(clientsocket2,buffer2,sizeof(buffer2),0);
            }
            else if(strcmp(buffer1,"paper\n")==0 && strcmp(buffer2,"scissor\n")==0)
            {
                strcpy(buffer1,"Lose");
                strcpy(buffer2,"Win");
                send(clientsocket1,buffer1,sizeof(buffer1),0);
                send(clientsocket2,buffer2,sizeof(buffer2),0);
            }
            else if(strcmp(buffer2,"rock\n")==0 && strcmp(buffer1,"paper\n")==0)
            {
                strcpy(buffer1,"Win");
                strcpy(buffer2,"Lose");
                send(clientsocket1,buffer1,sizeof(buffer1),0);
                send(clientsocket2,buffer2,sizeof(buffer2),0);
            }
            else if(strcmp(buffer2,"rock\n")==0 && strcmp(buffer1,"scissor\n")==0)
            {
                strcpy(buffer1,"Lose");
                strcpy(buffer2,"Win");
                send(clientsocket1,buffer1,sizeof(buffer1),0);
                send(clientsocket2,buffer2,sizeof(buffer2),0);
            }
            else if(strcmp(buffer2,"paper\n")==0 && strcmp(buffer1,"scissor\n")==0)
            {
                strcpy(buffer1,"Win");
                strcpy(buffer2,"Lose");
                send(clientsocket1,buffer1,sizeof(buffer1),0);
                send(clientsocket2,buffer2,sizeof(buffer2),0);
            }
        }
        else
        {
            strcpy(buffer1,"end");
            strcpy(buffer2,"end");
            send(clientsocket1,buffer1,sizeof(buffer1),0);
            send(clientsocket2,buffer2,sizeof(buffer2),0);
            break;
        }

    }

    close(clientsocket1);
    close(clientsocket2);
    close(serversocket1);
    close(serversocket2);
}

int main()
{
    server();

    return 0;
}