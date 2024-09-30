#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#define PORT 12345
#define IPaddress "127.0.0.1"
#define chunck_size 10
#define looptime 2

struct timeval current;

struct information
{
    char data[20];
    int number;
    long time;
};
typedef struct information* structdatapack;

struct retransmission
{
    int acknumber;
    long time;
};
typedef struct retransmission* structackpack;

struct reverseinfo
{
    char revdata[20];
    int number;
    long time;
};
typedef struct reverseinfo* structreverse;

int completed(int* arr,int size)
{
    int exist = 0;
    for(int i=0;i<size;i++)
    {
        if(arr[i]==0)
        {
            exist = 1;
            break;
        }
    }
    return exist;
}

void server()
{
    char buffer[1024];
    char output[1024];

    int serversocket;
    struct sockaddr_in serveraddr, clientaddr;

    serversocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(serversocket<0)
    {
        perror("socket");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(IPaddress);


    int bindstatus;
    if((bindstatus = bind(serversocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)))<0)
    {
        perror("bind is failed");
    }

    int msgsize;
    socklen_t lenclient = sizeof(clientaddr);
    if((msgsize = recvfrom(serversocket,buffer,sizeof(buffer),0,(struct sockaddr*)&clientaddr,&lenclient))<0)
    {
        perror("recvfrom");
        close(serversocket);
    }

    // Not waiting while receiving
    // int flags = fcntl(serversocket, F_GETFL, 0);
    // fcntl(serversocket, F_SETFL, flags | O_NONBLOCK);

    int num_chunks = atoi(buffer);
    structdatapack chunks[num_chunks];
    for(int i=0;i<num_chunks;i++)
    {
        chunks[i] = (structdatapack)malloc(sizeof(struct information));
    }
    // Declaring array
    int check[num_chunks];
    for(int i=0;i<num_chunks;i++)
    {
        check[i] = 0;
    }

    structackpack ackchunks[num_chunks];
    for(int i=0;i<num_chunks;i++)
    {
        ackchunks[i] = (structackpack)malloc(sizeof(struct retransmission));
    }

    structackpack temp;
    temp = (structackpack)malloc(sizeof(struct retransmission));

    structdatapack tempchunk;
    tempchunk = (structdatapack)malloc(sizeof(struct information));


    for(int i=0;i<num_chunks;i++)
    {
        int msgsize;
        socklen_t lenclient = sizeof(clientaddr);
        if((msgsize = recvfrom(serversocket,chunks[i],sizeof(struct information),0,(struct sockaddr*)&clientaddr,&lenclient))<0)
        {
            perror("recvfrom");
        }
        else
        {
            printf("Received chunk with seq number: %d\n",chunks[i]->number);
            printf("Message from client with seq number %d is %s\n",chunks[i]->number,chunks[i]->data);

            // Not sending acknumber for every 3rd chunk
            if(i == 0 || i%3 != 0)
            {
                ackchunks[i]->acknumber = chunks[i]->number;
                gettimeofday(&current,NULL);
                ackchunks[i]->time = current.tv_sec*1000 + current.tv_usec/1000;
                // printf("acknumber %d\n",ackchunks[i]->acknumber);
                sendto(serversocket,ackchunks[i],sizeof(struct retransmission),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
                if(ackchunks[i]->time - chunks[i]->time < 100)
                {
                    check[i] = 1;
                }
            }
        }
    
    }



    while(completed(check,num_chunks))
    {
        for(int i=0;i<num_chunks;i++)
        {
            if(check[i]==0)
            {
                int msgsize;
                socklen_t lenclient = sizeof(clientaddr);
                if((msgsize = recvfrom(serversocket,chunks[i],sizeof(struct information),0,(struct sockaddr*)&clientaddr,&lenclient))<0)
                {
                    // perror("recvfrom");
                    // close(serversocket);
                }
                else
                {
                    printf("Received chunk with seq number: %d\n",chunks[i]->number);
                    printf("Message from client with seq number %d is %s\n",chunks[i]->number,chunks[i]->data);

                    // Not sending acknumber for every 3rd chunk

                        ackchunks[i]->acknumber = chunks[i]->number;
                        gettimeofday(&current,NULL);
                        ackchunks[i]->time = current.tv_sec*1000 + current.tv_usec/1000;
                        // printf("acknumber %d\n",ackchunks[i]->acknumber);
                        sendto(serversocket,ackchunks[i],sizeof(struct retransmission),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
                        if(ackchunks[i]->time - chunks[i]->time < 100)
                        {
                            check[i] = 1;
                        }
                }
            }
        }
    }


    for(int i=0;i<num_chunks;i++)
    {
        strcat(output,chunks[i]->data);
    }

    printf("Final message to server is:-  %s\n", output);

    for(int i=0;i<num_chunks;i++)
    {
        check[i] = 0;
    }

    printf("---------------------------------------------------------------------------------------------------\n");
    

    // Not waiting while receiving
    int flags = fcntl(serversocket, F_GETFL, 0);
    fcntl(serversocket, F_SETFL, flags | O_NONBLOCK);

    // sending information in chunks continously
    for(int i=0;i<num_chunks;i++)
    {
        gettimeofday(&current,NULL);
        chunks[i]->time = current.tv_sec*1000 + current.tv_usec/1000; 
        int sendstatus = sendto(serversocket,chunks[i],sizeof(struct information),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
        if(sendstatus<0)
        {
            perror("send");
        }

        int recvstatus = recvfrom(serversocket,temp,sizeof(struct retransmission),0,(struct sockaddr*)&clientaddr,&lenclient);
        if(recvstatus<0)
        {
            // printf("error\n");
            // printf("Not recieved the ack number\n");
        }
        else
        {
            int tempack = temp->acknumber;
            ackchunks[tempack]->acknumber = temp->acknumber;
            ackchunks[tempack]->time = temp->time;
            if(temp->time - chunks[tempack]->time < 100)
            {
                check[ackchunks[tempack]->acknumber] = 1;
                printf("Received ackchunk with ack number: %d\n",ackchunks[tempack]->acknumber);
            }
        }
    }

    time_t starttime = time(NULL);
    time_t currenttime;

    while(completed(check,num_chunks))
    {
        currenttime = time(NULL);
        if(currenttime >= starttime + looptime)
        {
            break;
        }
        for(int i=0;i<num_chunks;i++)
        {

            int recvstatus = recvfrom(serversocket,temp,sizeof(struct retransmission),0,(struct sockaddr*)&clientaddr,&lenclient);
            if(recvstatus<0)
            {
                // printf("Not recieved the ack number\n");
            }
            else
            {
                int tempack = temp->acknumber;
                ackchunks[tempack]->acknumber = temp->acknumber;
                ackchunks[tempack]->time = temp->time;
                if(temp->time - chunks[tempack]->time < 100)
                {
                    check[tempack] = 1;
                    printf("Received ackchunk with ack number: %d\n",ackchunks[tempack]->acknumber);
                }
            }
        }
    }

    // resending non received chunks
        while(completed(check,num_chunks))
        {
            for(int i=0;i<num_chunks;i++)
            {
                if(check[i] == 0)
                {
                    gettimeofday(&current,NULL);
                    chunks[i]->time = current.tv_sec*1000 + current.tv_usec/1000; 
                    int sendstatus = sendto(serversocket,chunks[i],sizeof(struct information),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
                    if(sendstatus<0)
                    {
                        perror("send");
                    } 
                }

                int recvstatus = recvfrom(serversocket,temp,sizeof(struct retransmission),0,(struct sockaddr*)&clientaddr,&lenclient);
                if(recvstatus<0)
                {
                    // printf("Not recieved the ack number\n");
                }
                else
                {
                    int tempack = temp->acknumber;
                    ackchunks[tempack]->acknumber = temp->acknumber;
                    ackchunks[tempack]->time = temp->time;
                    if(temp->time - chunks[tempack]->time < 100)
                    {
                        check[ackchunks[tempack]->acknumber] = 1;
                        printf("Received ackchunk with ack number: %d\n",ackchunks[tempack]->acknumber);
                    }
                }
            }

            starttime = time(NULL);
            while(completed(check,num_chunks))
            {
                currenttime = time(NULL);
                if(currenttime >= starttime + looptime)
                {
                    break;
                }
                for(int i=0;i<num_chunks;i++)
                {
                    if(check[i]==0)
                    {
                        int recvstatus = recvfrom(serversocket,temp,sizeof(struct retransmission),0,(struct sockaddr*)&clientaddr,&lenclient);
                        if(recvstatus<0)
                        {
                            // printf("Not recieved the ack number\n");
                        }
                        else
                        {
                            int tempack = temp->acknumber;
                            ackchunks[tempack]->acknumber = temp->acknumber;
                            ackchunks[tempack]->time = temp->time;
                            if(temp->time - chunks[tempack]->time < 100)
                            {
                                printf("Received ackchunk with ack number: %d\n",temp->acknumber);
                                check[ackchunks[tempack]->acknumber] = 1;
                            }
                        }
                    }
                }
            }
        }

    close(serversocket);


}

int main()
{
    server();

    return 0;
}