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
    //printf("free_shared_data()\n");
}

void initialize_structure_queues_data(){
    for(int i=0; i < MAX_QUEUE_SIZE;i++){
        //initialize queue_name
        queues_data.queue_name[i] = malloc_shared_data(MAX_QUEUE_NAME_SIZE * sizeof(char));
        queues_data.queue_name[i][0] = 0;
        //char* e = "\0";
        //printf("%d\n", strcmp(queues_data.queue_name[i], e)==0);
        //printf("queue_name ok\n");
        queues_data.queue_messages[i] = malloc_shared_data(MAX_MESSAGE_NUMBER * sizeof(char*));
        queues_data.queue_consumers[i] = malloc_shared_data(MAX_CONSUMER_NUMBER * sizeof(unsigned int*)); 
        for(int j=0; j < MAX_MESSAGE_NUMBER;j++){
            queues_data.queue_messages[i][j] = malloc_shared_data(MAX_MESSAGE_SIZE * sizeof(char));
            queues_data.queue_messages[i][j][0] = 0;
            //printf("queue_message ok\n");
            queues_data.queue_consumers[i][j] = malloc_shared_data(sizeof(unsigned int));
            queues_data.queue_consumers[i][j] = 0;
            //printf("queue_consumer ok\n");        
        }
    }
}

void create_structure_queues_data(){
    queues_data.queue_name = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(char*));
    queues_data.queue_messages = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(char**));
    queues_data.queue_consumers = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(unsigned int**));
    initialize_structure_queues_data();
}

void free_structure_queues_data(){
    for (int i = 0; i < MAX_QUEUE_SIZE; i++){
        for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
            free_shared_data(queues_data.queue_consumers[i][j], sizeof(unsigned int));
            free_shared_data(queues_data.queue_messages[i][j], MAX_MESSAGE_SIZE * sizeof(char));
        }
        free_shared_data(queues_data.queue_consumers[i], MAX_CONSUMER_NUMBER * sizeof(unsigned int**));
        free_shared_data(queues_data.queue_messages[i], MAX_MESSAGE_NUMBER * sizeof(char*));
        free_shared_data(queues_data.queue_name[i], MAX_QUEUE_NAME_SIZE * sizeof(char));
    }
    free_shared_data(queues_data.queue_name, MAX_QUEUE_SIZE * sizeof(char*));
    free_shared_data(queues_data.queue_messages, MAX_QUEUE_SIZE * sizeof(char**));
    free_shared_data(queues_data.queue_consumers, MAX_QUEUE_SIZE * sizeof(unsigned int**));
}

void add_queue(char* queue_name){
    int i = get_id(empty);
    //pr("i:%d", i);
    memcpy(queues_data.queue_name[i], queue_name, MAX_QUEUE_NAME_SIZE);
}

void publish(char* queue_name, char* msg){
    int i = get_id(queue_name);
    for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
        if(strcmp(queues_data.queue_messages[i][j],(char*)'0')){
            strcpy(queues_data.queue_messages[i][j],msg);
            return;
        }
    }
    //printf("publish()\n");
}

void add_consumer(char* queue_name, int connfd){
    int i = get_id(queue_name);
    for(int j = 0; j < MAX_QUEUE_SIZE; j++){
        if(queues_data.queue_consumers[i][j] == 0){
            *queues_data.queue_consumers[i][j] = connfd;
            return;
        }
    }
    ///printf("add_consumer\n");
}

int get_id(char* queue_name){
    for(int i = 0; i < MAX_QUEUE_SIZE;i++){
        if(strcmp(queues_data.queue_name[i], queue_name) == 0){
            return i;
        }
    }
    return -1;
    //printf("get_id()\n");
}
