#ifndef AMPQ_H
#define AMPQ_H

#include <sys/types.h>
#include <netinet/in.h>
#include "queue.h"


enum class_type_t{
  CONNECTION = 0xa,
  CHANNEL    = 0x14,
  QUEUE      = 0x32,
  BASIC      = 0x3c,
};

enum method_type_t{
  CONNECTION_START = 0xa,
  CONNECTION_START_OK = 0xb,
  CONNECTION_TUNE = 0x1e,
  CONNECTION_TUNE_OK = 0x1f,
  CONNECTION_OPEN = 0x28,
  CONNECTION_OPEN_OK = 0x29,
  CONNECTION_CLOSE = 0x32,
  CONNECTION_CLOSE_OK = 0x33,

  CHANNEL_OPEN = 0xa,
  CHANNEL_OPEN_OK = 0xb,
  CHANNEL_CLOSE = 0x28,
  CHANNEL_CLOSE_OK = 0x29,

  QUEUE_DECLARE = 0xa,
  QUEUE_DECLARE_OK = 0xb,

  BASIC_PUBLISH = 0x28,
  BASIC_ACK = 0x50,
  BASIC_QOS = 0xa,
  BASIC_QOS_OK = 0xb,
  BASIC_CONSUME = 0x14,
  BASIC_CONSUME_OK = 0x15,
  BASIC_DELIVER = 0x3c,

};

enum frame_type_t{
  PROTOCOL  = 0x41,
  METHOD    = 0x01, //frame_method
  HEADER    = 0x02, //frame_header
  BODY      = 0x03, //frame_contet
  HEARTBEAT = 0x08  //frame_heartbeat
};

/*
• Declaração da fila;
• Conexão de vários clientes simultaneamente (cada cliente simultâneo pode publicar ou requisitar
mensagens da mesma fila ou de filas distintas);
• Inscrição de cliente em uma fila e consequente envio das mensagens desta fila para o cliente res-
peitando o esquema de Round Robin caso mais de um cliente esteja conectado na mesma fila;
• Publicação de mensagem em uma fila;
• Desconexão de cliente.*/

/*
 * Structure any packet received. 
 */
void print_hex(char* recvline, size_t size);

u_int8_t parse_frame_type(char* recvline, int connfd);

u_int32_t parse_frame_length(char* recvline, int connfd);

u_int16_t parse_frame_class(char* recvline, int connfd);

u_int16_t parse_frame_method(char* recvline, int connfd);

void getString(char* s, char* recvline, int start);
#endif