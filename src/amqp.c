#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "amqp.h"
#include "hardcode.h"
#include <string.h>


void print_hex(char* s, size_t size){
      for(int i = 0; i < size; ++i)
          printf("%02x ", (unsigned int)*s++);
      printf("\n");           
} 

u_int8_t parse_frame_type(char* recvline, int connfd){
    u_int8_t type;
    read(connfd, &type, 1); // read type from buffer
    //return ntohs(*((u_int8_t*)recvline));
    return type;
}

u_int32_t parse_frame_length(char* recvline, int connfd){
    read(connfd, recvline, 2); //read channel from buffer
    read(connfd, recvline, 4); //read length from buffer
    return ntohl(*((u_int32_t*)recvline));
}

u_int16_t parse_frame_class(char* recvline, int connfd){
    read(connfd, recvline, 2); //read class from buffer
    return ntohs(*((u_int16_t*)recvline));
}

u_int16_t parse_frame_method(char* recvline, int connfd){
    read(connfd, recvline, 2); //read method from buffer
    return ntohs(*((u_int16_t*)recvline));
}

void getString(char* s,char* recvline, int start){
    int j = 0;
    for(int i = start; (recvline[i] != 0) || (recvline[i] == 206); i++){
        s[j++] = recvline[i];
    }
    s[j] = '\0';
}

void getQueueName(int connfd, char* recvline, uint32_t frame_length, char* qName){
    read(connfd, recvline, frame_length-3);
    getString(qName, recvline, 3);
}

void getPublishData(int connfd, char* recvline, uint32_t frame_length, char* qName, char* payload){
    read(connfd, recvline, frame_length-3);
    getString(qName,recvline,4);
    read(connfd,recvline, 3); //content header type + channel
    read(connfd,recvline, 4); //content header lenght 4 hex bytes
    u_int32_t length = ntohl(*((u_int32_t*)recvline));
    read(connfd,recvline, length+1+3);
    read(connfd,recvline, 4);//content body length
    length = ntohl(*((u_int32_t*)recvline));
    read(connfd,recvline, length +1);
    getString(payload,recvline,0);
    //payload[strlen(payload)-2] = 0;
    //printf("%s %d %s\n", qName, length, payload);
}
                                    

void send_connection_start(int connfd){
    write(connfd, CONNECTION_START_PKT, SZ_CONNECTION_START_PKT-1);
}

void send_connection_tune(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd,CONNECTION_TUNE_PKT , SZ_CONNECTION_TUNE_PKT-1);
}

void send_connection_open_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, CONNECTION_OPEN_OK_PKT, SZ_CONNECTION_OPEN_OK_PKT-1);                            
}

void send_connection_close_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, CONNECTION_CLOSE_OK_PKT, SZ_CONNECTION_CLOSE_OK_PKT-1);                            
}

void send_channel_open_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd,CHANNEL_OPEN_OK_PKT , SZ_CHANNEL_OPEN_OK_PKT-1);                                                     
}

void send_channel_close_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, CHANNEL_CLOSE_OK_PKT, SZ_CHANNEL_CLOSE_OK_PKT-1);
}

frame create_frame(u_int8_t t, u_int16_t ch, u_int32_t l, u_int16_t cl, u_int16_t m){
    frame a;
    a.type = t;
    a.channel = htons(ch);
    a.length = htonl(l);
    a.class = htons(cl);
    a.method = htons(m);
    return a;
}

void unparse_frame(char *pkt, int*sz, frame a){
    memcpy(pkt+*sz,(char*)&a.type,sizeof(a.type)); *sz+= sizeof(a.type);
    memcpy(pkt+*sz,(char*)&a.channel,sizeof(a.channel)); *sz+= sizeof(a.channel);
    memcpy(pkt+*sz,(char*)&a.length,sizeof(a.length)); *sz+= sizeof(a.length);
    memcpy(pkt+*sz,(char*)&a.class,sizeof(a.class)); *sz+= sizeof(a.class);
    memcpy(pkt+*sz,(char*)&a.method,sizeof(a.method)); *sz+= sizeof(a.method);
}

void unparse_queue_ok(char*pkt, int*sz, char* qName){
    u_int8_t len = strlen(qName);
    u_int32_t v3 = htonl(0);
    memcpy(pkt+*sz,(char*)&(len),sizeof(len)); *sz+= sizeof(len);
    memcpy(pkt+*sz,qName, len); *sz+= len;
    memcpy(pkt+*sz,(char*)&(v3),sizeof(v3)); *sz+= sizeof(v3);
    memcpy(pkt+*sz,(char*)&(v3),sizeof(v3)); *sz+= sizeof(v3);
    memcpy(pkt+*sz, "\xce",1); *sz+=1;
}

void send_queue_declare_ok(int connfd, char* recvline, u_int32_t frame_length, char* qName){
    char pkt[MAXLINE];int sz = 0;
    unparse_frame(pkt,&sz, create_frame(1,1,frame_length+1,50,11));
    unparse_queue_ok(pkt,&sz, qName);
    print_hex(pkt, sz);
    write(connfd, pkt, sz);
}

void send_basic_qos_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, BASIC_QOS_OK_PKT, SZ_BASIC_QOS_OK_PKT-1);
}

void send_basic_ack(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
}

void send_basic_consume_ok(int connfd, char* recvline, u_int32_t frame_length){
    write(connfd, BASIC_CONSUME_OK_PKT, SZ_BASIC_CONSUME_OK_PKT-1);
}