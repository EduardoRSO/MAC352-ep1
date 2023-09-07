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
  CONNECTION = 10,
  CHANNEL    = 20,
  QUEUE      = 50,
  BASIC      = 60
};

/*
 *
 * Methods seen through wireshark
 *
 */
enum method_type_t{
  CONNECTION_START = 10,
  CONNECTION_START_OK = 11,
  CONNECTION_TUNE = 30,
  CONNECTION_TUNE_OK = 31,
  CONNECTION_OPEN = 40,
  CONNECTION_OPEN_OK = 41,
  CONNECTION_CLOSE = 50,
  CONNECTION_CLOSE_OK = 51,

  CHANNEL_OPEN = 10,
  CHANNEL_OPEN_OK = 11,
  CHANNEL_CLOSE = 40,
  CHANNEL_CLOSE_OK = 41,

  QUEUE_DECLARE = 10,
  QUEUE_DECLARE_OK = 11,

  BASIC_PUBLISH = 40,
  BASIC_ACK = 80,
  BASIC_QOS = 10,
  BASIC_QOS_OK = 11,
  BASIC_CONSUME = 20,
  BASIC_CONSUME_OK = 21,
  BASIC_DELIVER = 60,

};

/*
 *
 * Type os frames used by amqp
 *
 * */
enum frame_type_t{
  METHOD    = 1, //frame_method
  HEADER    = 2, //frame_header
  BODY      = 3, //frame_contet
  HEARTBEAT = 8  //frame_heartbeat
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
typedef struct frame_format_t{
  frame_type_t frame_type;
  channel_t channel; //must be 0 - 65535
  length_t length; //payload size excluding frame-end
  union{
    typedef struct frame_heartbeat_t frame_heartbeat;
    typedef struct frame_protocol_header_t frame_protocol_header;
    typedef struct frame_method_t frame_method;
    typedef struct frame_header_t frame_header;
    typedef struct frame_body_t frame_body;
  } payload;
  u_char frame_end; //must always be "%xCE"
} frame_format;

typedef struct frame_heartbeat_t{
  //Has nothing inside;
} frame_heartbeat;

typedef struct frame_protocol_header_t{
  protocol_t protocol;
  id_major_t id_major;
  id_minor_t id_minor;
  version_major_t version_major;
  version_minor_t version_minor;
} protocol_header;

typedef struct frame_method_t{
  class_id_t class_id;
  method_id_t method_id;
  void *arguments;
 } frame_method;

typedef struct frame_header_t{
  class_id_t class_id;
  weight_t weight;
  body_size_t body_size;
  property_flag_t property_flag;
  void* property_list;
 } frame_header; 

typedef struct frame_body_t{
  length_t length;
  void *buffer;
} frame_body;

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
frame_format_t* parse_frame(char* packet);

unsigned char* read_heartbeat_packet();
unsigned char* create_heartbeat_packet();

unsigned char* read_method_packet();
unsigned char* create_method_packet(class_id_t class_id, method_id_t);

unsigned char* read_header_pacter();
unsigned char* create_header_packet();

unsigned char* read_body_packet();
unsigned char* create_body_packet();


#endif
