#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "amqp.h"
//#include "hardcode.h"
#include <string.h>

void print_hex(char* s, size_t size){
      for(int i = 0; i < size; ++i)
          printf("%02x ", (unsigned int)*s++);
      printf("\n");           
}
  
void print_hex_(u_int16_t* s, size_t size){
      for(int i = 0; i < size; ++i)
          printf("%02x ", (unsigned int) *s++);
      printf("\n");    
}
  

long int parse_frame_class(char * recvline, int n){
    printf("%x\n", recvline[0]);
    return 0;
}

long int parse_frame_method(char * recvline, int n){
    char method[n];
    strncpy(method, recvline + (8-1), 10);
    print_hex(method, n);
    long int l = strtol(method, NULL, 0);
    printf("%ld\n", l);
    return strtol(method, NULL, 0);
}
