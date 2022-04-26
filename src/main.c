/* Por Luca Assumpção Dillenburg
 * Em 16/3/2022
 *
 * Servidor broker que implementa as funções básicas do MQTT.
 * Baseado em um servidor de eco disponibilizado pelo Prof. Daniel Batista
 * <batista@ime.usp.br>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include "data.c"
#include "utils.c"
#include "files.c"

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096
#define BROKER_PORT "1883"

/* Function Headers */
void start_broker_server();
void process_mqtt(int connfd);
char *parse_fixed_header(char *bytes, struct packet_header *header);
char *parse_packet_identifier(char *bytes, struct packet_header *header);

/* ========================================================= */
/*                            MAIN                           */
/* ========================================================= */

int main(int argc, char **argv)
{
    // listen_topic("topic");

    // char *path = argv[1];
    // printf("path: '%s'\n", path);
    // DIR *topic_dir = opendir(path);
    // struct dirent *file;
    // while ((file = readdir(topic_dir)) != NULL)
    //     printf("> Got file: '%s'\n", file->d_name);

    // char *msg = "hello world";
    // send_message("topic1", msg, strlen(msg));

    start_broker_server();
    return 0;
}

/* ========================================================= */
/*                   FUNCTION DEFINITIONS                    */
/* ========================================================= */

void start_broker_server()
{
    /* Os sockets. Um que será o socket que vai escutar pelas conexões
     * e o outro que vai ser o socket específico de cada conexão */
    int listenfd, connfd;
    /* Informações sobre o socket (endereço e porta) ficam nesta struct */
    struct sockaddr_in servaddr;
    /* Retorno da função fork para saber quem é o processo filho e
     * quem é o processo pai */
    pid_t childpid;

    /* Criação de um socket. É como se fosse um descritor de arquivo.
     * É possível fazer operações como read, write e close. Neste caso o
     * socket criado é um socket IPv4 (por causa do AF_INET), que vai
     * usar TCP (por causa do SOCK_STREAM), já que o MQTT funciona sobre
     * TCP, e será usado para uma aplicação convencional sobre a Internet
     * (por causa do número 0) */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
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
     * qual a porta. Neste caso será a porta padrão BROKER_PORT
     */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(BROKER_PORT));
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind :(\n");
        exit(3);
    }

    /* Como este código é o código de um servidor, o socket será um
     * socket passivo. Para isto é necessário chamar a função listen
     * que define que este é um socket de servidor que ficará esperando
     * por conexões nos endereços definidos na função bind. */
    if (listen(listenfd, LISTENQ) == -1)
    {
        perror("listen :(\n");
        exit(4);
    }

    printf("[MQQT Broker is open. Waiting connections in port %s.]\n", BROKER_PORT);
    printf("[To stop the server, press CTRL+c.]\n");

    /* O servidor no final das contas é um loop infinito de espera por
     * conexões e processamento de cada uma individualmente */
    for (;;)
    {
        /* O socket inicial que foi criado é o socket que vai aguardar
         * pela conexão na porta especificada. Mas pode ser que existam
         * diversos clientes conectando no servidor. Por isso deve-se
         * utilizar a função accept. Esta função vai retirar uma conexão
         * da fila de conexões que foram aceitas no socket listenfd e
         * vai criar um socket específico para esta conexão. O descritor
         * deste novo socket é o retorno da função accept. */
        if ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) == -1)
        {
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
         * processo filho. */
        if ((childpid = fork()) == 0)
        {
            /**** PROCESSO FILHO ****/
            printf("[Uma conexão aberta]\n");
            /* Já que está no processo filho, não precisa mais do socket
             * listenfd. Só o processo pai precisa deste socket. */
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
            /*                            EP1                            */
            process_mqtt(connfd);
            /* ========================================================= */

            /* Após ter feito toda a troca de informação com o cliente,
             * pode finalizar o processo filho */
            printf("[Uma conexão fechada]\n");
            exit(0);
        }
        else
            /**** PROCESSO PAI ****/
            /* Se for o pai, a única coisa a ser feita é fechar o socket
             * connfd (ele é o socket do cliente específico que será tratado
             * pelo processo filho) */
            close(connfd);
    }
    exit(0);
}

void process_mqtt(int connfd)
{
    /* Armazena o tamanho da string lida do cliente */
    ssize_t length;
    /* Armazena linhas recebidas do cliente */
    char bytes_received[MAXLINE + 1];
    /* Armazena o packet atual */
    struct packet_header packet_header;
    /* Armazena os dados que serão enviados */
    struct vector send_data;
    /* Armazena o ponteiro para o resto dos bytes */
    char *rest_bytes;

    /* CONNECT */
    /* TODO: Consider Flags */
    length = read(connfd, bytes_received, MAXLINE);
    rest_bytes = parse_fixed_header(bytes_received, &packet_header);
    if (length == 0 || packet_header.type != CONNECT)
    {
        perror("connect :(\n");
        exit(6);
    }
    printf("> Recebeu CONNECT\n");

    /* CONNACK */
    /* TODO: Consider Flags and Different responses */
    send_data.length = 4;
    send_data.array = (char *)malloc(4 * sizeof(char));
    send_data.array[0] = 0x20;
    send_data.array[1] = 0x02;
    send_data.array[2] = 0;
    send_data.array[3] = 0;
    write(connfd, send_data.array, send_data.length);
    free(send_data.array);
    printf("> Enviou CONNACK\n");

    char *who;
    while ((length = read(connfd, bytes_received, MAXLINE)) > 0)
    {
        rest_bytes = parse_fixed_header(bytes_received, &packet_header);

        if (packet_header.type == PUBLISH)
        {
            /* TODO: Consider Flags */
            who = "pub";

            unsigned int topic_length = construct_int(rest_bytes[1], rest_bytes[0]);
            char *topic = copy_str(&rest_bytes[2], topic_length);
            rest_bytes += 2 + topic_length;

            unsigned long int message_length = packet_header.last_ptr - rest_bytes + 1;
            char *message = copy_str(rest_bytes, message_length);

            printf("%s > Sending message to topic '%s': '%s'\n", who, topic, message);
            send_message(topic, bytes_received, length);
            printf("%s > Sent message to topic '%s': '%s'\n", who, topic, message);
            free(topic);
            free(message);

            printf("%s #### Message: ", who);
            print_bytes(bytes_received, length);
        }
        else if (packet_header.type == SUBSCRIBE)
        {
            /* TODO: Consider Flags */
            who = "sub";

            rest_bytes = parse_packet_identifier(rest_bytes, &packet_header);

            unsigned int topic_length = construct_int(rest_bytes[1], rest_bytes[0]);
            char *topic = copy_str(&rest_bytes[2], topic_length);
            printf("%s > Subscribed to topic '%s'\n", who, topic);

            /* SUBACK */
            send_data.length = 5;
            send_data.array = (char *)malloc(4 * sizeof(char));
            send_data.array[0] = 0x90;
            send_data.array[1] = 0x03;
            send_data.array[2] = high_char(packet_header.id);
            send_data.array[3] = low_char(packet_header.id);
            send_data.array[4] = 0;
            write(connfd, send_data.array, send_data.length);
            free(send_data.array);
            printf("%s > Enviou SUBACK\n", who);

            struct file topic_file = listen_topic(topic);
            while ((length = read(topic_file.fd, bytes_received, MAXLINE)) > 0)
            {
                printf("%s > Received message from topic Subscribed to topic '%s'\n", who, topic);

                /* PUBLISH (sent from Server to a Client) */
                long a = write(connfd, bytes_received, length);
                printf("%s #### Message: ", who);
                print_bytes(bytes_received, length);
                long b = fflush(connfd);
                printf("\n%s > Enviou PUBLISH to client (bytes: %u, a: %u, b: %u)\n", who, length, a, b);
            }

            free(topic);
        }

        printf("%s > Waiting new packet\n", who);
    }

    printf("%s > No more messages\n", who);
}

/* ========================================================= */
/*                          PARSING                          */
/* ========================================================= */

char *parse_fixed_header(char *bytes, struct packet_header *header)
{
    header->flags = bytes[0] & 0x0F;       /* 4 low bits of byte */
    header->type = (bytes[0] >> 4) & 0x0F; /* 4 high bits of byte */

    long remaining_length = 0;
    char *rest_bytes = decode_remaining_length(&bytes[1], &remaining_length);
    header->last_ptr = rest_bytes + remaining_length - 1;

    return rest_bytes;
}

char *parse_packet_identifier(char *bytes, struct packet_header *header)
{
    char *rest_bytes = bytes;

    char msb_id = rest_bytes[0];
    char lsb_id = rest_bytes[1];
    header->id = construct_int(lsb_id, msb_id);
    rest_bytes += 2;

    return rest_bytes;
}
