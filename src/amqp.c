#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "amqp.h"
//#include "hardcode.h"
#include <string.h>

#define MAX_SIZE 4096
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
