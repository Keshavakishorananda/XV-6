#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

#define PORT 12345
#define IPaddress "127.0.0.1"
#define chunck_size 10
#define looptime 2

struct information
{
    char data[20];
    int number;
    long time;
};
typedef struct information* structdatapack;

struct timeval current;

struct retransmission
{
    int acknumber;
    long time;
};
typedef struct retransmission* structackpack;

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

void client()
{
    char input[1024];
    char buffer[1024];
    char output[1024];

    int clientsocket;
    struct sockaddr_in serveraddr, clientaddr;

    clientsocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(clientsocket<0)
    {
        perror("socket");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(IPaddress);

    // Not waiting while receiving
    int flags = fcntl(clientsocket, F_GETFL, 0);
    fcntl(clientsocket, F_SETFL, flags | O_NONBLOCK);

    
    fgets(input,sizeof(input),stdin);
    input[strlen(input)-1] = '\0';

    int num_chunks = strlen(input)/chunck_size + 1;
    sprintf(buffer,"%d",num_chunks);

    // Declaring array
    int check[num_chunks];
    for(int i=0;i<num_chunks;i++)
    {
        check[i] = 0;
    }

    // sending num_chunks to server
    int sendstatus = sendto(clientsocket,buffer,sizeof(buffer),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(sendstatus<0)
    {
        perror("send");
    }

    // putting information into chunks
    structdatapack chunks[num_chunks];
    for(int i=0;i<num_chunks;i++)
    {
        chunks[i] = (structdatapack)malloc(sizeof(struct information));
    }

    for(int i=0;i<num_chunks;i++)
    {
        int start = i*(chunck_size);
        int end = (i+1)*(chunck_size);

        strncpy(chunks[i]->data,input+start,end-start);
        chunks[i]->data[end-start] = '\0';
        chunks[i]->number = i;
    }

    // for receiving chunks
    structackpack ackchunks[num_chunks];
    for(int i=0;i<num_chunks;i++)
    {
        ackchunks[i] = (structackpack)malloc(sizeof(struct retransmission));
    }

    structackpack temp;
    temp = (structackpack)malloc(sizeof(struct retransmission));

    structdatapack tempchunk;
    tempchunk = (structdatapack)malloc(sizeof(struct information));

    // sending information in chunks continously
        for(int i=0;i<num_chunks;i++)
        {
            gettimeofday(&current,NULL);
            chunks[i]->time = current.tv_sec*1000 + current.tv_usec/1000; 
            sendstatus = sendto(clientsocket,chunks[i],sizeof(struct information),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
            if(sendstatus<0)
            {
                perror("send");
            }

            int recvstatus = recvfrom(clientsocket,ackchunks[i],sizeof(struct retransmission),0,NULL,NULL);
            if(recvstatus<0)
            {
                // printf("Not recieved the ack number\n");
            }
            else
            {
                if(ackchunks[i]->time - chunks[i]->time < 100)
                {
                    check[ackchunks[i]->acknumber] = 1;
                    printf("Received ackchunk with ack number: %d\n",ackchunks[i]->acknumber);
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
                
                int recvstatus = recvfrom(clientsocket,temp,sizeof(struct retransmission),0,NULL,NULL);
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
                    sendstatus = sendto(clientsocket,chunks[i],sizeof(struct information),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
                    if(sendstatus<0)
                    {
                        perror("send");
                    } 
                }

                int recvstatus = recvfrom(clientsocket,temp,sizeof(struct retransmission),0,NULL,NULL);
                if(recvstatus<0)
                {
                    // printf("Not recieved the ack number\n");
                }
                else
                {
                    int tempack = temp->acknumber;
                    ackchunks[tempack]->acknumber = temp->acknumber;
                    ackchunks[tempack]->time = temp->time;
                    if(ackchunks[i]->time - chunks[i]->time < 100)
                    {
                        check[ackchunks[i]->acknumber] = 1;
                        printf("Received ackchunk with ack number: %d\n",ackchunks[i]->acknumber);
                    }
                }
            }

            // flags = fcntl(clientsocket, F_GETFL, 0);
            // fcntl(clientsocket, F_SETFL, flags & ~O_NONBLOCK);


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
                        int recvstatus = recvfrom(clientsocket,temp,sizeof(struct retransmission),0,NULL,NULL);
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


        for(int i=0;i<num_chunks;i++)
        {
            check[i] = 0;
        }

        printf("-------------------------------------------------------------------------------\n");

        flags = fcntl(clientsocket, F_GETFL, 0);
        fcntl(clientsocket, F_SETFL, flags & ~O_NONBLOCK);

        for(int i=0;i<num_chunks;i++)
        {
            // if(check[i]==0)
            // {
                int msgsize;
                socklen_t lenclient = sizeof(clientaddr);
                if((msgsize = recvfrom(clientsocket,chunks[i],sizeof(struct information),0,NULL,NULL))<0)
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
                        sendto(clientsocket,ackchunks[i],sizeof(struct retransmission),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
                        if(ackchunks[i]->time - chunks[i]->time < 100)
                        {
                            check[i] = 1;
                        }
                    }
                }
            // }
        }

        while(completed(check,num_chunks))
        {
            for(int i=0;i<num_chunks;i++)
            {
                // printf("receiving error\n");
                if(check[i]==0)
                {
                    int msgsize;
                    socklen_t lenclient = sizeof(clientaddr);
                    if((msgsize = recvfrom(clientsocket,chunks[i],sizeof(struct information),0,NULL,NULL))<0)
                    {
                        // perror("recvfrom");
                        // close(clientsocket);
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
                            sendto(clientsocket,ackchunks[i],sizeof(struct retransmission),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
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
    close(clientsocket);
}

int main()
{
    client();

    return 0;
}