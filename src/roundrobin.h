#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include <sys/types.h>
#include "amqp.h"
#include "scheduler.h"
/*
 *
 * Scheduler node that holds subscribers waiting to consume message.
 *
 * */
typedef struct scheduler_node_t{
  struct scheduler_node_t * next;
  connection_state_t subscriber;
} scheduler_node;

/*
 *
 * Scheduler head to avoid duplicating scheduler_name for each scheduler node.
 *
 * */
typedef struct scheduler_t{
  scheduler_node_t* head;
  scheduler_node_t* last;
} scheduler;

/*
 *
 * Scheduler Functions
 *
 * */
scheduler_t* new_scheduler_handler();
scheduler_t* add_subscriber(scheduler_t head, scheduler_node_t new_scheduler);
scheduler_t* remove_subscriber(scheduler_t head, scheduler_node_t new_scheduler);
scheduler_t* 
void free_scheduler(scheduler_t head);

/*
 *
 * Scheduler Node Functions
 *
 * */

scheduler_node_t* new_scheduler_node();
void free_scheduler_node(scheduler_node_t head);

#endif
