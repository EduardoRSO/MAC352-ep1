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

unsigned char parse_frame_class(char * recvline, int n){
    return recvline[8];
}

unsigned char parse_frame_method(char * recvline, int n){
    return recvline[10];
}

void getString(char* s,char* recvline, int start){
  int j = 0;
  for(int i = start; (recvline[i] != 0) || (recvline[i] == '0xce'); i++)
      s[j++] = recvline[i];
  s[j] = '\n';
}
