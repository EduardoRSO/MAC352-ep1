#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>

#define MAX_QUEUE_SIZE 100
#define MAX_QUEUE_NAME_SIZE 100
#define MAX_MESSAGE_SIZE 100
#define MAX_CONSUMER_NUMBER 100
#define MAX_MESSAGE_NUMBER 100

/*
 * I am goind to use mmap to share memory between forks, so I changed
 * the struct to be a creepy array. Ex:
 *
 *  char* QUEUE_NAME queues_data[i].queue_name           -> holds queue name
 *  char** QUEUE_MESSAGES queues_data[i].queue_messages  -> holds messages
 *  int** QUEUE_CONSUMERS queues_data[i].queue_consumers -> schedule consumers
 * 
 * */
typedef struct queue_t{
  char**  queue_name;
  char*** queue_messages;
  int***  queue_consumers;
} queue;

extern queue queues_data;

void create_structure_queues_data();
void initialize_strucuture_queues_data();
void free_structure_queues_data();
void free_shared_data(void* p, size_t size);
void* malloc_shared_data(size_t size);
void add_queue(char* queue_name);
void publish(char* queue_name, char* msg);
void add_consumer(char* queue_name, int* connfd);
void consume(char* queue_name, int* connfd, char* msg);
int get_id(char* queue_name);

#endif
