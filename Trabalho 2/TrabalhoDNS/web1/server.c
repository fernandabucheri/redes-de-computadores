#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define REQ_GET "GET"
#define MAX_REQ_LEN 1000
#define FBLOCK_SIZE 4000

#define COD_ERROR_0_METHOD "ERROR-0 Method not supported\n"
#define COD_ERROR_1_FILE "ERROR-1 File could not be open\n"

struct req_t {
    char method[128];
    char filename[200];
};

typedef struct req_t req_t;

void get_request(req_t *r, char *rstr) {
    bzero(r, sizeof(req_t));
    sscanf(rstr, "%s %s", r->method, r->filename);
}

void send_file(int sockfd, req_t r) {
    int fd, nr;
    unsigned char fbuff[FBLOCK_SIZE];

    fd = open(r.filename, O_RDONLY, S_IRUSR);
    if (fd == -1) {
        perror("open");
        send(sockfd, COD_ERROR_1_FILE, strlen(COD_ERROR_1_FILE), 0);
        return;
    }

    do {
        bzero(fbuff, FBLOCK_SIZE);
        nr = read(fd, fbuff, FBLOCK_SIZE);
        if (nr > 0) {
            send(sockfd, fbuff, nr, 0);
        }
    } while (nr > 0);

    close(fd);
    return;
}

void proc_request(int sockfd, req_t r) {
    if (strcmp(r.method, REQ_GET) == 0) {
        send_file(sockfd, r);
    } else {
        send(sockfd, COD_ERROR_0_METHOD, strlen(COD_ERROR_0_METHOD), 0);
    }
    return;
}

int main(int argc, char **argv) {
    struct sockaddr_in saddr;
    struct sockaddr_in caddr;
    int add_len;
    int sc;
    char request[MAX_REQ_LEN];
    int sl;
    req_t req;

    if (argc != 3) {
        printf("Uso %s <porta> <IP>\n", argv[0]);
    }

    sl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sl == -1) {
        perror("socket");
        return 0;
    }

    saddr.sin_port = htons(atoi(argv[1]));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_addr.s_addr = inet_addr(argv[2]);

    if (bind(sl, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        return 0;
    }

    if (listen(sl, 1000) == -1) {
        perror("listen");
        return 0;
    }

    printf("Esperando clientes...\n");

    add_len = sizeof(struct sockaddr_in);

    while (1) {
        sc = accept(sl, (struct sockaddr *) &caddr, (socklen_t *) &add_len);
        if (sc == -1) {
            perror("accept");
        }
        printf("Conectado com cliente %s: %d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        while (1) {
            bzero(request, MAX_REQ_LEN);
            // lendo a mensagem do cliente e copiando para o buffer
            recv(sc, request, MAX_REQ_LEN, 0);
            // mostrando o que o cliente escreveu
            printf("\nRequisição feita pelo cliente: %s", request);

            // se a mensagem for "exit", finaliza
            if (strncmp("exit", request, 4) == 0) {
                printf("Finalizando...\n");
                return 0;
            }

            // Faz a requisição do arquivo
            get_request(&req, request);
            printf("method: %s\nfilename: %s\n", req.method, req.filename);
            proc_request(sc, req);
        }
        close(sc);
    }
    close(sl);
    return 0;
}
