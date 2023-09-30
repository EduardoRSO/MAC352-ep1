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

#include "amqp.h"
#include "hardcode.h"
#include "poll.h"
#include "queue.h"

#define LISTENQ 1
#define MAXLINE 4096

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
    ssize_t n;
   
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
            printf("[Uma conexão aberta]\n");
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
            printf("[Cliente conectado no processo filho %d\n",getpid());
            struct pollfd pfd[1];
            pfd[0].fd = connfd;
            pfd[0].events = POLLIN;
            poll(pfd,1,-1);
            int i = 0;
            while(1){
                poll(pfd,1,-1);
                if(pfd[0].revents & POLLIN){    
                    if(i == 0){
                    read(connfd,recvline, 10); 
                    write(connfd, CONNECTION_START_PKT, SZ_CONNECTION_START_PKT-1);
                    i++;
                    }
                    u_int8_t frame_type = parse_frame_type(recvline, connfd);
                    u_int32_t frame_length = parse_frame_length(recvline,connfd);
                    u_int16_t frame_class = parse_frame_class(recvline,connfd);
                    u_int16_t frame_method = parse_frame_method(recvline, connfd);
                    //printf("type %d\nlength %d\nclass %d\nmethod %d\n",frame_type, frame_length, frame_class, frame_method);
                    switch (frame_class){
                        case CONNECTION:
                            switch (frame_method)
                            {
                            case CONNECTION_START_OK:
                                //received connection start ok
                                //send connection tune
                                read(connfd, recvline, frame_length-3);
                                write(connfd,CONNECTION_TUNE_PKT , SZ_CONNECTION_TUNE_PKT-1);
                                //printf("Connection start ok\n");
                                break;
                            case CONNECTION_TUNE_OK:
                                //received connection tune ok
                                read(connfd, recvline, frame_length-3);
                                //printf("Connection tune ok ok\n");
                                break;
                            case CONNECTION_OPEN:
                                //received connection open
                                //send connection open ok
                                read(connfd, recvline, frame_length);
                                write(connfd,CONNECTION_OPEN_OK_PKT , SZ_CONNECTION_OPEN_OK_PKT-1);
                                //printf("Connection open\n");
                                break;
                            case CONNECTION_CLOSE:
                                //received connection close
                                //send connection close ok
                                read(connfd, recvline, frame_length-3);
                                write(connfd, CONNECTION_CLOSE_OK_PKT, SZ_CONNECTION_CLOSE_OK_PKT-1);
                                //printf("connection close\n");
                                free_structure_queues_data();
                                exit(0);
                                break;
                            default:
                                printf("Unknown connection class packet\n");
                                break;
                            }
                            break;
                        case CHANNEL:
                            switch (frame_method)
                            {
                            case CHANNEL_OPEN:
                                //received channel open
                                //send channel open ok
                                read(connfd, recvline, frame_length-3);
                                write(connfd,CHANNEL_OPEN_OK_PKT , SZ_CHANNEL_OPEN_OK_PKT-1);
                                //printf("Channel open\n");
                                break;
                            case CHANNEL_CLOSE:
                                //received channel close
                                //send channel close ok
                                read(connfd, recvline, frame_length-3);
                                write(connfd, CHANNEL_CLOSE_OK_PKT, SZ_CHANNEL_CLOSE_OK_PKT-1);
                                //write(connfd, BASIC_DELIVER_PKT, SZ_BASIC_DELIVER_PKT);    
                                //printf("channel close\n");
                                break;
                            default:
                                printf("Unknown channel class packet\n");
                                break;
                            }
                            break;
                        case QUEUE:
                            //received queue declare
                            //send queue declare ok
                            read(connfd, recvline, frame_length-3); //get queue name
                            char qName[MAX_QUEUE_NAME_SIZE];
                            getString(qName, recvline, 3);
                            puts(qName);
                            add_queue(qName);
                            write(connfd, QUEUE_DECLARE_OK_PKT, SZ_QUEUE_DECLARE_OK_PKT-1);
                            printf("queue declare\n");
                            break;
                        case BASIC:
                            switch(frame_method){
                                case BASIC_PUBLISH:
                                    //received basic publish
                                    //send nothing
                                    read(connfd, recvline, frame_length-3);
                                    char qName[MAX_QUEUE_SIZE];
                                    char payload[MAX_MESSAGE_SIZE];
                                    getString(qName,recvline,4);
                                    puts(qName);
                                    read(connfd,recvline, MAXLINE);
                                    getString(payload,recvline,30);
                                    payload[strlen(payload)-2] = 0;
                                    puts(payload);
                                    publish(qName, payload);                                        
                                    write(connfd, CHANNEL_CLOSE_OK_PKT, SZ_CHANNEL_CLOSE_OK_PKT-1);
                                    //printf("basic publish\n");
                                    break;
                                case BASIC_ACK:
                                    //received basic ack
                                    //send nothing
                                    read(connfd, recvline, frame_length-3);
                                    //printf("basic ack\n");
                                    break;
                                case BASIC_QOS:
                                    //received basic qos
                                    //send basic qos ok
                                    read(connfd, recvline, frame_length-3);
                                    write(connfd, BASIC_QOS_OK_PKT, SZ_BASIC_QOS_OK_PKT-1);
                                    //printf("basic qos\n");
                                    break;
                                case BASIC_CONSUME:
                                    //received basic consume
                                    //send basic consume ok
                                    read(connfd, recvline, frame_length-3);
                                    char qNameC[MAX_QUEUE_SIZE];
                                    getString(qNameC, recvline, 3);
                                    puts(qNameC);
                                    write(connfd, BASIC_CONSUME_OK_PKT, SZ_BASIC_CONSUME_OK_PKT-1);
                                    //printf("basic consume\n");
                                    break;
                                default:
                                    printf("Unknown basic class packet\n");
                                    break;
                            }
                            break;
                        default:
                            //received protocol header
                            //send connection start
                            write(connfd, CONNECTION_START_PKT, SZ_CONNECTION_START_PKT-1);
                            //printf("connection start\n");                            
                            break;       
                    }
                }
            
                //printf("----------\n");
        }
            
            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 FIM                           */
            /* ========================================================= */
        }    /* ========================================================= */
        else
            /**** PROCESSO PAI ****/
            /* Se for o pai, a única coisa a ser feita é fechar o socket
             * connfd (ele é o socket do cliente específico que será tratado
             * pelo processo filho)
             */
            close(connfd);
    }
    free_structure_queues_data();
    exit(0);
}
