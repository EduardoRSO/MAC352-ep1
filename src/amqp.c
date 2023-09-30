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

void send_queue_declare_ok(int connfd, char* recvline, u_int32_t frame_length, char* qName){
    char pkt[MAXLINE];
    //o length do pkt1 precisa ser alterado, então na verdade será preciso que um pacote inteiro seja construido se atendo aos detalhes
    memcpy(pkt, QUEUE_DECLARE_OK_PKT1, SZ_QUEUE_DECLARE_OK_PKT1);
    memcpy(pkt + SZ_QUEUE_DECLARE_OK_PKT1, qName, strlen(qName));
    memcpy(pkt+SZ_QUEUE_DECLARE_OK_PKT1 + strlen(qName), QUEUE_DECLARE_OK_PKT2, SZ_QUEUE_DECLARE_OK_PKT2 -1);    
    print_hex(pkt, SZ_QUEUE_DECLARE_OK_PKT1 + strlen(qName) + SZ_QUEUE_DECLARE_OK_PKT2-2);
    write(connfd, pkt, SZ_QUEUE_DECLARE_OK_PKT1 + strlen(qName) + SZ_QUEUE_DECLARE_OK_PKT2-2);
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