#include "queue.h"

#include "string.h"
#include <sys/mman.h>
#include <stdio.h>

queue queues_data;

void* malloc_shared_data(size_t size){
    void* m = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,0,0);
    return m;
}

//https://www.ibm.com/docs/en/i/7.2?topic=ssw_ibm_i_72/apis/munmap.html
void free_shared_data(void* p, size_t size){
    if(!munmap(p, size))
        printf("free_shared_data()\n");
}

void initialize_structure_queues_data(){
    for(int i=0; i < MAX_QUEUE_NUMBER;i++){
        //initialize queue_name
        queues_data.queue_name[i] = malloc_shared_data(sizeof(queues_data.queue_name[i]));
        queues_data.queue_name[i] = 0;

        for(int j=0; j < MAX_MESSAGE_NUMBER;j++){
            queues_data.queue_messages[i][j] = malloc_shared_data(sizeof(queues_data.queue_messages[i]));
            queues_data.queue_messages[i][j] = 0;
            queues_data.queue_consumers[i][j] = malloc_shared_data(sizeof(queues_data.queue_consumers[i]));
            queues_data.queue_consumers[i][j] = 0;        
        }
    }
}

void create_structure_queues_data(){
    queues_data.queue_name = malloc_shared_data(sizeof(queues_data.queue_name));
    queues_data.queue_messages = malloc_shared_data(sizeof(queues_data.queue_messages));
    queues_data.queue_consumers = malloc_shared_data(sizeof(queues_data.queue_consumers));
    initialize_structure_queues_data();
}

void free_structure_queues_data(){
    for (int i = 0; i < MAX_QUEUE_NUMBER; i++){
        for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
            free_shared_data(queues_data.queue_consumers[i][j], MAX_MESSAGE_NUMBER * sizeof(unsigned int*));
            free_shared_data(queues_data.queue_messages[i][j], MAX_MESSAGE_NUMBER * sizeof(char*));
        }
        free_shared_data(queues_data.queue_name[i], MAX_QUEUE_NAME_SIZE * sizeof(char));
    }
    free_shared_data(queues_data.queue_name, MAX_QUEUE_NUMBER * sizeof(char*));
    free_shared_data(queues_data.queue_messages, MAX_QUEUE_NUMBER * sizeof(char**));
    free_shared_data(queues_data.queue_consumers, MAX_QUEUE_NUMBER * sizeof(unsigned int**));
}


