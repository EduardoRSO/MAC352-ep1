#ifndef AMPQ_H
#define AMPQ_H

#include <sys/types.h>
#include "queue.h"

//amqp*.stripped.xml
#define FRAME_MIN_SIZE 4096
#define FRAME_END 206
#define REPLY_SUCCESS 200
#define AMQP_DEFAULT_CHANNEL_HEARTBEAT 0
#define MAXLINE 4096

/*
 * 
 * Classes seen through wireshark
 *
 * */
enum class_type_t{
  CONNECTION = 0xa,
  CHANNEL    = 0x32,
  QUEUE      = 0x80,
  BASIC      = 0x96
};

/*
 *
 * Methods seen through wireshark
 *
 */
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

/*
 *
 * Type os frames used by amqp
 *
 * */
enum frame_type_t{
  PROTOCOL  = 0x41,
  METHOD    = 0x01, //frame_method
  HEADER    = 0x02, //frame_header
  BODY      = 0x03, //frame_contet
  HEARTBEAT = 0x08  //frame_heartbeat
};

/*
 *
 * Amqp data types
 *
 * */
typedef u_int16_t class_id_t;
typedef u_int16_t method_id_t;
typedef u_char* protocol_t;
typedef u_int8_t id_major_t;
typedef u_int8_t id_minor_t;
typedef u_int8_t version_major_t;
typedef u_int8_t version_minor_t;
typedef u_int16_t channel_t;
typedef u_int32_t length_t;
typedef u_int16_t weight_t;
typedef u_int64_t body_size_t;
typedef u_int16_t property_flag_t;


/*
 *
 * Amqp data structures
 *
 */
typedef struct frame_heartbeat_t{
  char* a; 
} frame_heartbeat_t;

typedef struct frame_protocol_header_t{
  protocol_t protocol;
  id_major_t id_major;
  id_minor_t id_minor;
  version_major_t version_major;
  version_minor_t version_minor;
} frame_protocol_header_t;

typedef struct frame_method_t{
  class_id_t class_id;
  method_id_t method_id;
  void *arguments;
 } frame_method_t;

typedef struct frame_header_t{
  class_id_t class_id;
  weight_t weight;
  body_size_t body_size;
  property_flag_t property_flag;
  void* property_list;
 } frame_header_t; 

typedef struct frame_body_t{
  length_t length;
  void *buffer;
} frame_body_t;

typedef struct frame_format_t{
  enum frame_type_t frame_type;
  channel_t channel; //must be 0 - 65535
  length_t length; //payload size excluding frame-end
  union{
    frame_heartbeat_t frame_heartbeat;
    frame_protocol_header_t frame_protocol_header;
    frame_method_t frame_method;
    frame_header_t frame_header;
    frame_body_t frame_body;
  } payload;
  u_char frame_end; //must always be "%xCE"
} frame_format_t;

/* 
frame_protocol_header:
+---+---+---+---+---+---+---+---+
|'A'|'M'|'Q'|'P'| 0 | 0 | 9 | 1 |
+---+---+---+---+---+---+---+---+
          8 octets

frame_method
+----------+-----------+-------------- - -
| class-id | method-id | arguments...
+----------+-----------+-------------- - -
    short     short    

frame_header
+----------+--------+-----------+----------------+------------- - -
| class-id | weight | body size | property flags | property list...
+----------+--------+-----------+----------------+------------- - -
   short     short    long long      short           remainder

frame_body
 +-----------------------+ +-----------+
| Opaque binary payload | | frame-end |
+-----------------------+ +-----------+         
                                  
General frame format
0      1         3      7          size+7       size+8
+------+---------+------+ +---------+ +-----------+
| type | channel | size | | Payload | | Frame-end |
+------+---------+------+ +---------+ +-----------+
 octet   short     long  'size' octets    octet
*/


/*
 *
 * Data structure to hold all info about connection state
 *
 * */
typedef struct connection_state_t{
  int connfd;
  queue_t *binded_queue;
  queues_handler_t *queues_handler;
  char recvline[MAXLINE];
  char sendline[MAXLINE];
} connection_state;

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
int parse_frame_type(char* recvline);
char* create_connection_start_ok_packet();
char* create_connection_tune_ok_packet();
char* create_connection_open_ok_packet();
char* create_connection_close_ok_packet();
char* create_channel_open_ok_packet();
char* create_channel_close_ok_packet();
char* create_queue_declare_ok_packet();
char* create_basic_consume_ok_packet();
char* create_basic_qos_ok_packet();
#endif
