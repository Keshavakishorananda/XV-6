# PART-B:-
# Difference between Traditional TCP and Our TCP
1.Time required to transfer data is more in case of TCP implemented using UDP because it is always available to listen and to stop it we have to loop for some more time than required.<br>
2.No control flow is managed in TCP implemented by UDP.<br>
3.In traditional TCP while ending communication it will be done Three steps or four steps but here no such procedure is followed.<br>
4.In TCP while starting the communication it follows four step mechanism while here it is not the case.

# Extension for Implementation of flow Control
1.Putting some rate limit to receive the data.<br>
Eg:- using 'recvfrom()' function with regular intervals.<br>

2.If sender sending too fast then receiver will send an alert or information packet with recommnened rate limit.<br>

I will declare a count=0 variable initially in server and if there are rejectects more than capability.<br>
Eg:-
```c
int count = 0;
while(completed(check,num_chunks))
    {
        for(int i=0;i<num_chunks;i++)
        {
            if(check[i]==0)
            {
                int recvstatus = recvfrom(serversocket,chunks[i],sizeof(struct information),0,(struct sockaddr*)&clientaddr,&lenclient);
                if(recvstatus<0)
                {
                  printf("not Received");
                }
                else
                {
                    printf("Received chunk with seq number: %d\n",chunks[i]->number);
                    ackchunks[i]->acknumber = chunks[i]->number;
                    gettimeofday(&current,NULL);
                    ackchunks[i]->time = current.tv_sec*1000 + current.tv_usec/1000;
                    sendto(serversocket,ackchunks[i],sizeof(struct retransmission),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
                    if(ackchunks[i]->time - chunks[i]->time < 100)
                    {
                        check[i] = 1;
                    }
                    count++;
                }
            }
        }
    }
```
<br>
If count value is greater than threshold value in some particular period of time then i will send an alert to client to send data with regular gap of time.
