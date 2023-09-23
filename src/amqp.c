#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "amqp.h"
#include "hardcode.h"
#include <string.h>

long int parse_frame_class(char * recvline){
    char class[2];
    class[0] = recvline[7];
    class[1] = recvline[8];
    return strtol(class, NULL, 16);
}

long int parse_frame_method(char * recvline){
    char method[2];
    method[0] = recvline[9];
    method[1] = recvline[10];
    return strtol(method, NULL, 16);
}

char* create_connection_start_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,CONNECTION_START_OK_PKT, strlen(CONNECTION_START_OK_PKT));
    return pkt; 
}

char* create_connection_tune_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,CONNECTION_TUNE_OK_PKT, strlen(CONNECTION_TUNE_OK_PKT));
    return pkt; 
}

char* create_connection_open_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,CONNECTION_OPEN_OK_PKT, strlen(CONNECTION_OPEN_OK_PKT));
    return pkt; 
}

char* create_connection_close_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,CONNECTION_CLOSE_OK_PKT, strlen(CONNECTION_CLOSE_OK_PKT));
    return pkt; 
}

char* create_channel_open_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,CHANNEL_OPEN_OK_PKT, strlen(CHANNEL_OPEN_OK_PKT));
    return pkt; 
}

char* create_channel_close_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,CHANNEL_CLOSE_OK_PKT, strlen(CHANNEL_CLOSE_OK_PKT));
    return pkt; 
}

char* create_queue_declare_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,QUEUE_DECLARE_OK_PKT, strlen(QUEUE_DECLARE_OK_PKT));
    return pkt; 
}

char* create_basic_consume_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,BASIC_CONSUME_OK_PKT, strlen(BASIC_CONSUME_OK_PKT));
    return pkt; 
}

char* create_basic_qos_ok_packet(){
    char* pkt = NULL;
    memcpy(pkt,BASIC_QOS_OK_PKT, strlen(BASIC_QOS_OK_PKT));
    return pkt; 
}
