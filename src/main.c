/* Por Prof. Daniel Batista <batista@ime.usp.br>
 * Em 27/8/2023
 * 
 * Um código simples de um servidor de eco a ser usado como base para
 * o EP1. Ele recebe uma linha de um cliente e devolve a mesma linha.
 * Teste ele assim depois de compilar:
 * 
 * ./redes-servidor-exemplo-ep1 8000
 * 
 * Com este comando o servidor ficará escutando por conexões na porta
 * 8000 TCP (Se você quiser fazer o servidor escutar em uma porta
 * menor que 1024 você precisará ser root ou ter as permissões
 * necessáfias para rodar o código com 'sudo').
 *
 * Depois conecte no servidor via telnet. Rode em outro terminal:
 * 
 * telnet 127.0.0.1 8000
 * 
 * Escreva sequências de caracteres seguidas de ENTER. Você verá que o
 * telnet exibe a mesma linha em seguida. Esta repetição da linha é
 * enviada pelo servidor. O servidor também exibe no terminal onde ele
 * estiver rodando as linhas enviadas pelos clientes.
 * 
 * Obs.: Você pode conectar no servidor remotamente também. Basta
 * saber o endereço IP remoto da máquina onde o servidor está rodando
 * e não pode haver nenhum firewall no meio do caminho bloqueando
 * conexões na porta escolhida.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "amqp.h"
#include "hardcode.h"
#include "poll.h"
#include "queue.h"

#define LISTENQ 1

int main (int argc, char **argv) {
    /* Os sockets. Um que será o socket que vai escutar pelas conexões
     * e o outro que vai ser o socket específico de cada conexão
     */
    int listenfd, connfd;
    /* Informações sobre o socket (endereço e porta) ficam nesta struct
     */
    struct sockaddr_in servaddr;
    /* Retorno da função fork para saber quem é o processo filho e
     * quem é o processo pai
     */
    pid_t childpid;
    /* Armazena linhas recebidas do cliente
     */
    char recvline[MAXLINE + 1];
    /* Armazena o tamanho da string lida do cliente
     */
   
    if (argc != 2) {
        fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
        fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
        exit(1);
    }

    /* Criação de um socket. É como se fosse um descritor de arquivo.
     * É possível fazer operações como read, write e close. Neste caso o
     * socket criado é um socket IPv4 (por causa do AF_INET), que vai
     * usar TCP (por causa do SOCK_STREAM), já que o AMQP funciona sobre
     * TCP, e será usado para uma aplicação convencional sobre a Internet
     * (por causa do número 0)
     */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket :(\n");
        exit(2);
    }

    /* Agora é necessário informar os endereços associados a este
     * socket. É necessário informar o endereço / interface e a porta,
     * pois mais adiante o socket ficará esperando conexões nesta porta
     * e neste(s) endereços. Para isso é necessário preencher a struct
     * servaddr. É necessário colocar lá o tipo de socket (No nosso
     * caso AF_INET porque é IPv4), em qual endereço / interface serão
     * esperadas conexões (Neste caso em qualquer uma -- INADDR_ANY) e
     * qual a porta. Neste caso será a porta que foi passada como
     * argumento no shell (atoi(argv[1])). No caso do servidor AMQP,
     * utilize a porta padrão do protocolo que você deve descobrir
     * lendo a especificaçao dele ou capturando os pacotes de conexões
     * ao RabbitMQ com o Wireshark
     */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind :(\n");
        exit(3);
    }

    /* Como este código é o código de um servidor, o socket será um
     * socket que escutará por conexões. Para isto é necessário chamar
     * a função listen que define que este é um socket de servidor que
     * ficará esperando por conexões nos endereços definidos na função
     * bind.
     */
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen :(\n");
        exit(4);
    }

    printf("[Servidor no ar. Aguardando conexões na porta %s]\n",argv[1]);
    printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n");
   
    /* O servidor no final das contas é um loop infinito de espera por
     * conexões e processamento de cada uma individualmente
     */

    /*------------------------------------------------*/
    create_structure_queues_data();
    /*------------------------------------------------*/

	for (;;) {
        /* O socket inicial que foi criado é o socket que vai aguardar
         * pela conexão na porta especificada. Mas pode ser que existam
         * diversos clientes conectando no servidor. Por isso deve-se
         * utilizar a função accept. Esta função vai retirar uma conexão
         * da fila de conexões que foram aceitas no socket listenfd e
         * vai criar um socket específico para esta conexão. O descritor
         * deste novo socket é o retorno da função accept.
         */
        //print_queues_data();
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept :(\n");
            exit(5);
        }
      
        /* Agora o servidor precisa tratar este cliente de forma
         * separada. Para isto é criado um processo filho usando a
         * função fork. O processo vai ser uma cópia deste. Depois da
         * função fork, os dois processos (pai e filho) estarão no mesmo
         * ponto do código, mas cada um terá um PID diferente. Assim é
         * possível diferenciar o que cada processo terá que fazer. O
         * filho tem que processar a requisição do cliente. O pai tem
         * que voltar no loop para continuar aceitando novas conexões.
         * Se o retorno da função fork for zero, é porque está no
         * processo filho.
         */
        if ( (childpid = fork()) == 0) {
            /**** PROCESSO FILHO ****/
            //printf("[Uma conexão aberta]\n");
            /* Já que está no processo filho, não precisa mais do socket
             * listenfd. Só o processo pai precisa deste socket.
             */
            close(listenfd);
         
            /* Agora pode ler do socket e escrever no socket. Isto tem
             * que ser feito em sincronia com o cliente. Não faz sentido
             * ler sem ter o que ler. Ou seja, neste caso está sendo
             * considerado que o cliente vai enviar algo para o servidor.
             * O servidor vai processar o que tiver sido enviado e vai
             * enviar uma resposta para o cliente (Que precisará estar
             * esperando por esta resposta) 
             */

            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 INÍCIO                        */
            /* ========================================================= */
            /* ========================================================= */
            /* TODO: É esta parte do código que terá que ser modificada
             * para que este servidor consiga interpretar comandos AMQP
             */
            printf("[conexão %d aberta] connfd = %d\n", getpid(), connfd);
            struct pollfd pfd[1];
            pfd[0].fd = connfd;
            pfd[0].events = POLLIN;
            poll(pfd,1,-1);
            int i = 0;
            while(1){
                poll(pfd,1,-1);
                if(pfd[0].revents & POLLIN){    
                   if(i == 0){ //handles protocol header
                        read(connfd,recvline, 10); 
                        write(connfd, CONNECTION_START_PKT, SZ_CONNECTION_START_PKT-1);
                        i++;
                    }
                    parse_frame_type(recvline, connfd);
                    u_int32_t frame_length = parse_frame_length(recvline,connfd);
                    u_int16_t frame_class = parse_frame_class(recvline,connfd);
                    u_int16_t frame_method = parse_frame_method(recvline, connfd);
                    switch (frame_class){
                        case CONNECTION:
                            switch (frame_method)
                            {
                            case CONNECTION_START_OK:
                                //received connection start ok
                                //send connection tune
                                send_connection_tune(connfd, recvline, frame_length);
                                break;
                            case CONNECTION_TUNE_OK:
                                //received connection tune ok
                                //send nothing and read full buffer
                                read(connfd, recvline, frame_length-3);
                                break;
                            case CONNECTION_OPEN:
                                //received connection open
                                //send connection open ok
                                send_connection_open_ok(connfd, recvline, frame_length);
                                break;
                            case CONNECTION_CLOSE:
                                //received connection close
                                //send connection close ok
                                send_connection_close_ok(connfd,recvline,frame_length);
                                printf("[conexão %d fechada]\n", getpid());
                                print_queues_data();
                                free_structure_queues_data();
                                exit(0);
                                break;
                            default:
                                printf("    [-]unknown connection class packet\n");
                                break;
                            }
                            break;
                        case CHANNEL:
                            switch (frame_method)
                            {
                            case CHANNEL_OPEN:
                                //received channel open
                                //send channel open ok
                                send_channel_open_ok(connfd, recvline, frame_length);
                                break;
                            case CHANNEL_CLOSE:
                                //received channel close
                                //send channel close ok
                                send_channel_close_ok(connfd, recvline, frame_length);
                                break;
                            default:
                                printf("    [-]unknown channel class packet\n");
                                break;
                            }
                            break;
                        case QUEUE:
                            //received queue declare
                            //send queue declare ok
                            char qName[MAX_QUEUE_NAME_SIZE];
                            get_queue_name(connfd,recvline,frame_length,qName);
                            add_queue(qName);
                            send_queue_declare_ok(connfd, recvline, frame_length, qName);
                            break;
                        case BASIC:
                            switch(frame_method){
                                case BASIC_PUBLISH:
                                    //received basic publish
                                    //send nothing
                                    char qName[MAX_QUEUE_SIZE];
                                    char payload[MAX_MESSAGE_SIZE];
                                    get_publish_data(connfd, recvline, frame_length, qName, payload);
                                    publish(qName, payload);
                                    break;
                                case BASIC_ACK:
                                    //received basic ack
                                    //send nothing
                                    send_basic_ack(connfd, recvline, frame_length);
                                    break;
                                case BASIC_QOS:
                                    //received basic qos
                                    //send basic qos ok
                                    send_basic_qos_ok(connfd, recvline, frame_length);
                                    break;
                                case BASIC_CONSUME:
                                    //received basic consume
                                    //send basic consume ok
                                    char qNameC[MAX_QUEUE_SIZE];
                                    int fd = connfd;
                                    get_queue_name(connfd,recvline,frame_length,qNameC);
                                    add_consumer(qNameC, &fd);
                                    send_basic_consume_ok(connfd, recvline, frame_length);
                                    deliver(qNameC);
                                    print_queues_data();
                                    break;
                                default:
                                    printf("    [-]unknown basic class packet\n");
                                    break;
                            }
                            break;
                        default:
                            //received protocol header
                            //send connection start
                            read(connfd,recvline, 10); 
                            send_connection_start(connfd);
                            break;       
                    }
                }
    }
            
            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 FIM                           */
            /* ========================================================= */
        }    /* ========================================================= */
        else{
            /**** PROCESSO PAI ****/
            /* Se for o pai, a única coisa a ser feita é fechar o socket
             * connfd (ele é o socket do cliente específico que será tratado
             * pelo processo filho)
             */

            //close(connfd); ---------------> trying to avoid file descriptors being replicated after fork
        }
    }
    exit(0);
}

//descobrir como e quando enviar o deliver pkt