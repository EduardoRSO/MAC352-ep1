#include "queue.h"

#include "string.h"
#include <sys/mman.h>
#include <stdio.h>

queue queues_data;
char* empty = "\0";

void* malloc_shared_data(size_t size){
    void* m = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,0,0);
    return m;
}

//https://www.ibm.com/docs/en/i/7.2?topic=ssw_ibm_i_72/apis/munmap.html
void free_shared_data(void* p, size_t size){
    munmap(p, size);
}

void initialize_structure_queues_data(){
    for(int i=0; i < MAX_QUEUE_SIZE;i++){
        queues_data.queue_name[i] = malloc_shared_data(MAX_QUEUE_NAME_SIZE * sizeof(char));
        queues_data.queue_name[i][0] = 0;
        queues_data.queue_messages[i] = malloc_shared_data(MAX_MESSAGE_NUMBER * sizeof(char*));
        queues_data.queue_consumers[i] = malloc_shared_data(MAX_CONSUMER_NUMBER * sizeof(int*)); 
        for(int j=0; j < MAX_MESSAGE_NUMBER;j++){
            queues_data.queue_messages[i][j] = malloc_shared_data(MAX_MESSAGE_SIZE * sizeof(char));
            queues_data.queue_messages[i][j][0] = 0;
            queues_data.queue_consumers[i][j] = malloc_shared_data(sizeof(int));
            queues_data.queue_consumers[i][j][0] = 0;
        }
    }
}

void create_structure_queues_data(){
    queues_data.queue_name = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(char*));
    queues_data.queue_messages = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(char**));
    queues_data.queue_consumers = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(int**));
    initialize_structure_queues_data();
}

void free_structure_queues_data(){
    for (int i = 0; i < MAX_QUEUE_SIZE; i++){
        for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
            free_shared_data(queues_data.queue_consumers[i][j], sizeof(int));
            free_shared_data(queues_data.queue_messages[i][j], MAX_MESSAGE_SIZE * sizeof(char));
        }
        free_shared_data(queues_data.queue_consumers[i], MAX_CONSUMER_NUMBER * sizeof(int**));
        free_shared_data(queues_data.queue_messages[i], MAX_MESSAGE_NUMBER * sizeof(char*));
        free_shared_data(queues_data.queue_name[i], MAX_QUEUE_NAME_SIZE * sizeof(char));
    }
    free_shared_data(queues_data.queue_name, MAX_QUEUE_SIZE * sizeof(char*));
    free_shared_data(queues_data.queue_messages, MAX_QUEUE_SIZE * sizeof(char**));
    free_shared_data(queues_data.queue_consumers, MAX_QUEUE_SIZE * sizeof(int**));
}

void add_queue(char* queue_name){
    int i;
    if((i = get_id(queue_name)) == -1)
        i = get_id(empty);
    memcpy(queues_data.queue_name[i], queue_name, strlen(queue_name));
}

void publish(char* queue_name, char* msg){
    //printf("queue: %s\nmsg: %s\n", queue_name, msg);
    add_queue(queue_name); //change it later
    int i = get_id(queue_name);
    for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
        if(strcmp(queues_data.queue_messages[i][j],empty) == 0){
            memcpy(queues_data.queue_messages[i][j],msg,MAX_MESSAGE_SIZE);
            //printf("msg: ");
            //puts(queues_data.queue_messages[i][j]);
            return;
        }
    }
}

void add_consumer(char* queue_name, int* connfd){
    add_queue(queue_name); //change it later
    int i = get_id(queue_name);
    for(int j = 0; j < MAX_QUEUE_SIZE; j++){
        if(queues_data.queue_consumers[i][j][0] == 0){
            memcpy(queues_data.queue_consumers[i][j],connfd, sizeof(int));
            return;
        }
    }    
}


int consume(char* queue_name, int* connfd, char* msg){
    int i = get_id(queue_name);
    if(queues_data.queue_consumers[i][0][0] == 0 || strcmp(queues_data.queue_messages[i][0], empty)==0) return 0;
    *connfd = queues_data.queue_consumers[i][0][0];
    memcpy(msg, queues_data.queue_messages[i][0], MAX_MESSAGE_SIZE);
    for (int j = 1; j < MAX_CONSUMER_NUMBER; j++){
        if(queues_data.queue_consumers[i][j][0] != 0){ 
            queues_data.queue_consumers[i][j-1][0] = queues_data.queue_consumers[i][j][0];
        }else{ 
            queues_data.queue_consumers[i][j][0] = *connfd;
            break;
        }
    }
    for(int j = 1; j < MAX_MESSAGE_NUMBER;j++)
        if(strcmp(queues_data.queue_messages[i][j], empty) != 0) 
            memcpy(queues_data.queue_messages[i][j-1],queues_data.queue_messages[i][j],MAX_MESSAGE_SIZE);
        else 
            break;
    return 1;   
}

int get_id(char* queue_name){
    for(int i = 0; i < MAX_QUEUE_SIZE;i++){
        if(strcmp(queues_data.queue_name[i], queue_name) == 0){
            return i;
        }
    }
    return -1;
}

//send deliver pkt
//deliver pkt
