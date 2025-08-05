#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdint.h>

#define BUFSIZE 1024
#define OPSIZE 4

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    int sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        error_handling("sock() error");
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("connect() error");
    }
    else{
        puts("Connected........");
    }
    char opmsg[BUFSIZE];
    int32_t operand_count;
    fputs("Operand count: ", stdout);
    scanf("%d", &operand_count);
    opmsg[0] = (char)operand_count;

    for(int i = 0;i < operand_count;++i){
        printf("Operand %d: ", i + 1);
        scanf("%d", (int32_t *)&opmsg[i * OPSIZE + 1]);
    }
    getchar();
    fputs("Operator: ", stdout);
    scanf("%c", &opmsg[operand_count * OPSIZE + 1]);
    write(sock, opmsg, operand_count * OPSIZE + 2);
    int32_t result;
    read(sock, &result, sizeof(result));
    printf("Operation result: %d \n", result);
    close(sock);
    return 0;
}
