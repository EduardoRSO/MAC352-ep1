#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>

#define MAX_QUEUE_NUMBER 100
#define MAX_QUEUE_NAME_SIZE 1024
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
  unsigned int***  queue_consumers;
} queue;

extern queue queues_data;

void create_structure_queues_data();
void initialize_strucuture_queues_data();
void free_structure_queues_data();
void free_shared_data(void* p, size_t size);
void* malloc_shared_data(size_t size);
int add_queue(char* queue_name);
int publish(char* queue_name, char* msg);
int consume(char* queue_name);
int get_id(char* queue_name);

#endif
