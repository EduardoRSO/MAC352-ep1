#include "amqp.h"


//https://stackoverflow.com/questions/3022552/is-there-any-standard-htonl-like-function-for-64-bits-integers-in-c
uint64_t htonll(uint64_t n) {
    uint32_t left = (uint32_t) (n >> 32);
    uint32_t right = (uint32_t) (n & 0xffff);    

    uint32_t new_left = htonl(right);
    uint32_t new_right = htonl(left);

    return ((uint64_t) new_left << 32) | ((uint64_t) new_right);
}

void print_hex(char* s, size_t size){
      for(int i = 0; i < size; ++i)
          printf("%02x ", (unsigned int)*s++);
      printf("\n");           
} 

u_int8_t parse_frame_type(char* recvline, int connfd){
    u_int8_t type;
    read(connfd, &type, 1); // read type from buffer
    //return ntohs(*((u_int8_t*)recvline));
    return type;
}

u_int32_t parse_frame_length(char* recvline, int connfd){
    read(connfd, recvline, 2); //read channel from buffer
    read(connfd, recvline, 4); //read length from buffer
    return ntohl(*((u_int32_t*)recvline));
}

u_int16_t parse_frame_class(char* recvline, int connfd){
    read(connfd, recvline, 2); //read class from buffer
    return ntohs(*((u_int16_t*)recvline));
}

u_int16_t parse_frame_method(char* recvline, int connfd){
    read(connfd, recvline, 2); //read method from buffer
    return ntohs(*((u_int16_t*)recvline));
}

frame create_frame(u_int8_t t, u_int16_t ch, u_int32_t l, u_int16_t cl, u_int16_t m){
    frame a;
    a.type = t;
    a.channel = htons(ch);
    a.length = htonl(l);
    a.class = htons(cl);
    a.method = htons(m);
    return a;
}

void unparse_frame(char *pkt, int*sz, frame a){
    memcpy(pkt+*sz,(char*)&a.type,sizeof(a.type)); *sz+= sizeof(a.type);
    memcpy(pkt+*sz,(char*)&a.channel,sizeof(a.channel)); *sz+= sizeof(a.channel);
    memcpy(pkt+*sz,(char*)&a.length,sizeof(a.length)); *sz+= sizeof(a.length);
    memcpy(pkt+*sz,(char*)&a.class,sizeof(a.class)); *sz+= sizeof(a.class);
    memcpy(pkt+*sz,(char*)&a.method,sizeof(a.method)); *sz+= sizeof(a.method);
}

void unparse_queue_ok(char*pkt, int*sz, char* qName){
    u_int8_t len = strlen(qName);
    u_int32_t v3 = htonl(0);
    memcpy(pkt+*sz,(char*)&(len),sizeof(len)); *sz+= sizeof(len);
    memcpy(pkt+*sz,qName, len); *sz+= len;
    memcpy(pkt+*sz,(char*)&(v3),sizeof(v3)); *sz+= sizeof(v3);
    memcpy(pkt+*sz,(char*)&(v3),sizeof(v3)); *sz+= sizeof(v3);
    memcpy(pkt+*sz, "\xce",1); *sz+=1;
}

void unparse_deliver(char* pkt, int* sz, char* qName){
    //do not have too much time left to this assign
    char hardcoded2[] = "\x1f\x61\x6d\x71\x2e\x63\x74\x61\x67\x2d\x55\x6e\x73\x75\x6f\x31\x58\x6c\x68\x46\x58\x41\x6e\x45\x68\x6f\x58\x76\x58\x68\x59\x41\x00\x00\x00\x00\x00\x00\x00\x01\x00";
    int sz_hardcoded2 = 42;
    u_int8_t len = strlen(qName);
    memcpy(pkt+*sz,hardcoded2, sz_hardcoded2); *sz += sz_hardcoded2;
    memcpy(pkt+*sz,(char*)&(len),sizeof(len)); *sz+= sizeof(len);
    memcpy(pkt+*sz,qName, len); *sz+= len;
    memcpy(pkt+*sz, "\xce",1); *sz+=1;
    //print_hex(pkt, *sz);
    //printf("unparse_deliver\n");
}

void unparse_content_header(char* pkt,int* sz, u_int64_t msg_length){
    u_int8_t t = 2;
    u_int16_t ch = htons(1);
    u_int32_t l = htonl(15);
    u_int16_t cl = htons(60);
    u_int16_t w = htons(0);
    u_int64_t b = htonll(msg_length); 
    u_int16_t pf = htons(4096);
    u_int8_t d = 1;
    
    memcpy(pkt+*sz,(char*)&t,sizeof(t)); *sz+= sizeof(t);
    memcpy(pkt+*sz,(char*)&ch,sizeof(ch)); *sz+= sizeof(ch);
    memcpy(pkt+*sz,(char*)&l,sizeof(l)); *sz+= sizeof(l);
    memcpy(pkt+*sz,(char*)&cl,sizeof(cl)); *sz+= sizeof(cl);
    memcpy(pkt+*sz,(char*)&w,sizeof(w)); *sz+= sizeof(w);
    memcpy(pkt+*sz,(char*)&b,sizeof(b)); *sz+= sizeof(b);
    memcpy(pkt+*sz,(char*)&pf,sizeof(pf)); *sz+= sizeof(pf);
    memcpy(pkt+*sz,(char*)&d,sizeof(d)); *sz+= sizeof(d);
    memcpy(pkt+*sz,"\xce",1); *sz+=1;
    //print_hex(pkt, *sz);
    //printf("content_header\n");
}

void unparse_content_body(char* pkt, int* sz,char* msg){
    u_int8_t t = 3;
    u_int16_t ch = htons(1);
    u_int32_t l = htonl((u_int32_t)strlen(msg));

    memcpy(pkt+*sz,(char*)&t,sizeof(t)); *sz+= sizeof(t);
    memcpy(pkt+*sz,(char*)&ch,sizeof(ch)); *sz+= sizeof(ch);
    memcpy(pkt+*sz,(char*)&l,sizeof(l)); *sz+= sizeof(l);
    memcpy(pkt+*sz, msg,strlen(msg)); *sz+= strlen(msg); //erro aqui X consertei era o l no lugar do strlen
    memcpy(pkt+*sz, "\xce",1); *sz+=1;
    //print_hex(pkt, *sz);
    //printf("content_body\n");
    //os pacotes não estão se concatenando??? no wireshark está ok, mas eh como se não houvesse a divisa
}

void get_string(char* s,char* recvline, int start, int strlen){
    int j = 0;
    for(int i = start; j < strlen; i++){
        s[j++] = recvline[i];
    }
    s[j] = '\0';
    //printf("    [+]get_string: %s %d\n", s, j);
}

void get_queue_name(int connfd, char* recvline, uint32_t frame_length, char* qName){
    read(connfd, recvline, frame_length-3);
    get_string(qName, recvline, 3, recvline[2]);
}

void get_publish_data(int connfd, char* recvline, uint32_t frame_length, char* qName, char* payload){
    read(connfd, recvline, frame_length-3);
    get_string(qName, recvline, 4, recvline[3]);
    read(connfd,recvline, 3); //content header type + channel
    read(connfd,recvline, 4); //content header lenght 4 hex bytes
    u_int32_t length = ntohl(*((u_int32_t*)recvline));
    read(connfd,recvline, length+1+3);
    read(connfd,recvline, 4);//content body length
    length = ntohl(*((u_int32_t*)recvline));
    read(connfd,recvline, length +1);
    get_string(payload,recvline,0, length);
    //payload[strlen(payload)-2] = 0;
    //printf("%s %d %s\n", qName, length, payload);
}

void deliver(char* qName){
    int sch;
    char msg[MAXLINE];
    if(consume(qName, &sch, msg) != -1){
        printf("    [+]deliver: %s %d %s\n", qName, sch, msg);
        send_basic_deliver(sch, qName, msg);
    }else{
        printf("    [-]deliver: consume: %s %d %s\n", qName, sch, msg);
    }
}

void deliver_all(){
    //print_queues_data();
    for(int i=0;i<MAX_QUEUE_SIZE;i++){
        int sch;
        char msg[MAXLINE];
        char empty[] = "\0";
        //printf("entrei aqui\n");
        if(queues_data.queue_name[i] && strcmp(queues_data.queue_name[i],empty) != 0){
            //printf("entrei aqui2\n");
            while(consume(queues_data.queue_name[i], &sch, msg) != -1){
                //printf("entrei aqui3\n");
                send_basic_deliver(sch, queues_data.queue_name[i], msg);
                }
        }
        else{
            //printf("entrei aqui4\n");
            break;
        }
    }
}
                                    

void send_connection_start(int connfd){
    write(connfd, CONNECTION_START_PKT, SZ_CONNECTION_START_PKT-1);
}

void send_connection_tune(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd,CONNECTION_TUNE_PKT , SZ_CONNECTION_TUNE_PKT-1);
}

void send_connection_open_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, CONNECTION_OPEN_OK_PKT, SZ_CONNECTION_OPEN_OK_PKT-1);                            
}

void send_connection_close_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, CONNECTION_CLOSE_OK_PKT, SZ_CONNECTION_CLOSE_OK_PKT-1);                            
}

void send_channel_open_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd,CHANNEL_OPEN_OK_PKT , SZ_CHANNEL_OPEN_OK_PKT-1);                                                     
}

void send_channel_close_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, CHANNEL_CLOSE_OK_PKT, SZ_CHANNEL_CLOSE_OK_PKT-1);
}

void send_queue_declare_ok(int connfd, char* recvline, u_int32_t frame_length, char* qName){
    char pkt[MAXLINE];int sz = 0;
    unparse_frame(pkt,&sz, create_frame(1,1,frame_length+1,50,11));
    unparse_queue_ok(pkt,&sz, qName);
    //print_hex(pkt, sz);
    write(connfd, pkt, sz);
}

void send_basic_qos_ok(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
    write(connfd, BASIC_QOS_OK_PKT, SZ_BASIC_QOS_OK_PKT-1);
}

void send_basic_ack(int connfd, char* recvline, u_int32_t frame_length){
    read(connfd, recvline, frame_length-3);
}

void send_basic_consume_ok(int connfd, char* recvline, u_int32_t frame_length){
    write(connfd, BASIC_CONSUME_OK_PKT, SZ_BASIC_CONSUME_OK_PKT-1);
}

void send_basic_deliver(int connfd, char* qName, char* msg){
    //printf("send_basic-deliver: %d %s %s\n",connfd, qName, msg);
    char pkt[MAXLINE]; int sz = 0; u_int32_t frame_length = 47+strlen(qName); //deliver_offset
    unparse_frame(pkt,&sz,create_frame(METHOD, FIXED_CHANNEL, frame_length, BASIC_DELIVER, BASIC_DELIVER));
    unparse_deliver(pkt,&sz,qName);
    unparse_content_header(pkt,&sz,strlen(msg));
    unparse_content_body(pkt,&sz,msg);
    //print_hex(pkt, sz);
    write(connfd, pkt, sz);
}

