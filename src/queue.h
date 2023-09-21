#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>


/*
 *
 * Queue node that holds a message buffer waiting a consume call.
 *
 * */
typedef struct queue_node_t{
  struct queue_t * next;
  size_t buffer_size;
  char* buffer;
} queue_node_t;

/*
 *
 * Queue head to avoid duplicating queue_name for each queue node.
 *
 * */
typedef struct queue_t{
  char* queue_name;
  size_t queue_size;
  queue_node_t* head;
  queue_node_t* last;
} queue_t;


/*
 *
 * Holds all queues in use. It is just a circular linked list of queue_t.
 *
 * */
typedef struct queues_handler_t{
    queue_t* next_queue;
    queue_t* current_queue;
} queues_handler_t;

/*
 *
 * Queue Handler Functions
 *
 * */
queues_handler_t* new_queue_handler();
queues_handler_t* qh_enqueue(queues_handler_t head, queue_t new_queue);
queue_t* get_queue_by_name(queues_handler_t head, char* name);
void free_queue_handler(queues_handler_t head);

/*
 *
 * Queue Functions
 *
 * */
queue_t* new_queue();
queue_t enqueue(queue_t head, char* buffer);
queue_t dequeue(queue_t head, char* buffer);
void free_queue(queue_t head);

/*
 *
 * Queue Node Functions
 *
 * */
queue_node_t* new_queue_node();
void free_queue_node(queue_node_t head);

#endif
