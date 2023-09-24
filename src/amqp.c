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
  

unsigned char parse_frame_class(char * recvline, int n){
    return recvline[8];
}

unsigned char parse_frame_method(char * recvline, int n){
    return recvline[10];
}
