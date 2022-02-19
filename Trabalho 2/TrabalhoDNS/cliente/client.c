#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define MAX 4000

struct hostent *gethostbyname(const char *name);

int main(int argc, char **argv) {
    char buff[MAX];
    int sl;
    struct sockaddr_in saddr;
    struct hostent *h;

    if (argc != 3) {
        printf("Uso %s <porta> <nome do servidor>\n", argv[0]);
    }

    // criação e verificação do socket
    sl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sl == -1) {
        perror("socket");
        return 0;
    }

    bzero(&saddr, sizeof(saddr));

    // Buscando o IP do site digitado 
    h = gethostbyname(argv[2]);
    if (h == NULL) {
        herror("gethostbyname");
        return 0;
    }

    printf("O servidor %s possui o endereço de IP %s\n", argv[2], inet_ntoa(*(struct in_addr *) h->h_addr));

    saddr.sin_port = htons(atoi(argv[1]));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *) h->h_addr));

    // conectando o socket do cliente com o socket do servidor
    if (connect(sl, (struct sockaddr *) &saddr, sizeof(saddr)) == -1) {
        perror("connect");
        return 0;
    } else {
        printf("Conexão aceita de %s - %d\n", inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
    }

    while (1) {
        bzero(buff, sizeof(buff));
        printf("Digite a requisição (para sair digite 'exit')> ");
        // escrevendo a mensagem do cliente no buffer
        fgets(buff, sizeof buff, stdin);
        // mandando o conteúdo do buffer para o servidor
        send(sl, buff, strlen(buff), 0);

        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Finalizando...\n");
            break;
        }

        bzero(buff, sizeof(buff));

        //lendo a mensagem do servidor
        recv(sl, buff, MAX, 0);

        //mostrando o que o servidor escreveu
        printf("\n%s\n", buff);
    }

    // fechando o socket
    close(sl);
}
