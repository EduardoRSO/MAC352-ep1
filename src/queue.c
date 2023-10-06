#include "queue.h"

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
        queues_data.queue_consumers[i] = malloc_shared_data(MAX_CONSUMER_NUMBER * sizeof(int)); 
        for(int j=0; j < MAX_MESSAGE_NUMBER;j++){
            queues_data.queue_messages[i][j] = malloc_shared_data(MAX_MESSAGE_SIZE * sizeof(char));
            queues_data.queue_messages[i][j][0] = 0;
            queues_data.queue_consumers[i][j] = 0;
        }
    }
}

void create_structure_queues_data(){
    queues_data.queue_name = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(char*));
    queues_data.queue_consumers = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(int*));
    queues_data.queue_messages = malloc_shared_data(MAX_QUEUE_SIZE * sizeof(char**));
    initialize_structure_queues_data();
}

void free_structure_queues_data(){
    for (int i = 0; i < MAX_QUEUE_SIZE; i++){
        for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
            free_shared_data(queues_data.queue_messages[i][j], MAX_MESSAGE_SIZE * sizeof(char));
        }
        free_shared_data(queues_data.queue_messages[i], MAX_MESSAGE_NUMBER * sizeof(char*));
        free_shared_data(queues_data.queue_name[i], MAX_QUEUE_NAME_SIZE * sizeof(char));
        free_shared_data(queues_data.queue_consumers[i], MAX_CONSUMER_NUMBER * sizeof(int*));
    }
    free_shared_data(queues_data.queue_name, MAX_QUEUE_SIZE * sizeof(char*));
    free_shared_data(queues_data.queue_messages, MAX_QUEUE_SIZE * sizeof(char**));
    free_shared_data(queues_data.queue_consumers, MAX_QUEUE_SIZE * sizeof(int*));
}

void add_queue(char* queue_name){
    int i;
    if((i = get_id(queue_name)) != -1){
        printf("    [-]add_queue: %s alredy declared\n", queue_name);
    }
    else if((i = get_id(empty)) == -1){
        printf("    [-]add_queue: get_id(%s): queue not found\n", empty);
    }
    else{
        memcpy(queues_data.queue_name[i], queue_name, strlen(queue_name));
        printf("    [+]add_queue: %s %d %ld\n", queues_data.queue_name[i], i, strlen(queues_data.queue_name[i]));
    }
    return;
}

void publish(char* queue_name, char* msg){
    int i;
    if((i = get_id(queue_name)) == -1){
        printf("    [-]publish: get_id(%s): queue not found\n", queue_name);
        return;
    }
    for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
        if(strcmp(queues_data.queue_messages[i][j],empty) == 0){
            memcpy(queues_data.queue_messages[i][j],msg,MAX_MESSAGE_SIZE);
            printf("    [+]publish: %s %d %d %s\n",queue_name, i, j, queues_data.queue_messages[i][j]);
            return;
        }
    }
}

void add_consumer(char* queue_name, int* connfd){
    int i;
    if((i = get_id(queue_name)) == -1){
        printf("    [-]add_consumer: get_id(%s): queue not found\n", queue_name);
        return;
    }
    for(int j = 0; j < MAX_QUEUE_SIZE; j++){
        if(queues_data.queue_consumers[i][j] == 0){
            memcpy(&queues_data.queue_consumers[i][j],connfd, sizeof(int));
            printf("    [+]add_consumer: %s %d %d\n",queue_name, *connfd, j);
            print_consumers(i);
            return;
        }
    }    
}

int move_consumer_to_last_position(int i){
    int swap = queues_data.queue_consumers[i][0];
    for(int j = 0; j < MAX_CONSUMER_NUMBER; j++)
        if(queues_data.queue_consumers[i][j+1] != 0){
            memcpy(&queues_data.queue_consumers[i][j],&queues_data.queue_consumers[i][j+1],sizeof(int));
        }
        else if(queues_data.queue_consumers[i][j+1] == 0){
            memcpy(&queues_data.queue_consumers[i][j], &swap, sizeof(int));
            return 0;
        }
    return -1;
}

void remove_message(int i){
    memcpy(queues_data.queue_messages[i][0], "\0", sizeof(char));
    for(int j = 0; j < MAX_MESSAGE_NUMBER; j++){
        if(strcmp(queues_data.queue_messages[i][j+1],empty) != 0){
            memcpy(queues_data.queue_messages[i][j],queues_data.queue_messages[i][j+1], strlen(queues_data.queue_messages[i][j+1]));
            memcpy(queues_data.queue_messages[i][j+1], "\0", sizeof(char));
        }else{
            break;
        }
    }
}

int consume(char* queue_name, int* connfd, char* msg){
    int i;
    if((i = get_id(queue_name)) == -1){
        printf("    [-]consume: get_id(%s): queue not found\n", queue_name);
        return -1;
    }
    if(queues_data.queue_consumers[i][0] == 0 || strcmp(queues_data.queue_messages[i][0], empty)==0){
        printf("    [-]consume: %s %d %s\n", queue_name, queues_data.queue_consumers[i][0], queues_data.queue_messages[i][0]);
        return -1;
    }
    memcpy(connfd, &queues_data.queue_consumers[i][0],sizeof(int));
    memcpy(msg, queues_data.queue_messages[i][0], MAX_MESSAGE_SIZE);
    printf("    [+]consume: %s %d %d %s\n",queue_name, i, *connfd, msg);
    move_consumer_to_last_position(i);
    remove_message(i);
    return 0;   
}

int get_id(char* queue_name){
    for(int i = 0; i < MAX_QUEUE_SIZE;i++){
        if(strcmp(queues_data.queue_name[i], queue_name) == 0){
            return i;
        }
    }
    return -1;
}

void print_consumers(int i){
    printf("  [C]:");
    for(int j = 0; j < MAX_CONSUMER_NUMBER;j++){
        if(queues_data.queue_consumers[i][j] != 0){
            printf("%d, ",queues_data.queue_consumers[i][j]);
        }
        else{
            printf("\n");
            break;
        }
    }
}

void print_names(){
    printf("    [N]:\n");
    for(int i = 0; i < MAX_QUEUE_SIZE;i++){
        if(strcmp(queues_data.queue_name[i],empty) != 0){
            printf("%s ",queues_data.queue_name[i]);
        }
        else{
            printf("\n");
            break; 
        }
    }
}

void print_messages(int i){
    printf("  [M]:");
    for(int j = 0; j < MAX_MESSAGE_NUMBER;j++){
        if(strcmp(queues_data.queue_messages[i][j], empty) != 0){
            printf("%s, ",queues_data.queue_messages[i][j]);
        }
        else{
            printf("\n");
            break;
        }
    }
}
void print_queues_data(){
    printf("<MEMORY>:\n");
    for(int i = 0; i < MAX_QUEUE_SIZE;i++){
        if(strcmp(queues_data.queue_name[i],empty) != 0){
            printf("%s\n",queues_data.queue_name[i]);
            print_consumers(i);
            print_messages(i);
        }
        else{
            printf("\n");
            break; 
        }
    }
    printf("</MEMORY>:\n");
}
