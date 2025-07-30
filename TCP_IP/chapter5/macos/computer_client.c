#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <memory.h>

#define BUFSIZE 1024

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]){
    printf("%d\n", argc);
    for(int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    int sock;
    struct sockaddr_in serv_addr;
    if(argc != 3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        error_handling("sock() error");
    }
    
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

    int operand_count;
    fputs("Operand count: ", stdout);
    scanf("%d", &operand_count);
    getchar();
    int *operand;
    operand = (int *)malloc(sizeof(int) * operand_count);
    if(NULL == operand){
        error_handling("malloc() error");
    }
    for(int i = 0;i < operand_count;++i){
        printf("Operand %d: ", i + 1);
        scanf("%d", operand + i);
        getchar();
    }
    char operator;
    fputs("Operator: ", stdout);
    scanf("%c", &operator);
    int i = 0;
    for(i = 0;i < operand_count - 1; ++i){
        printf("%d\n", *(operand+i));
        write(sock, (char*)(operand+i), sizeof(*(operand + i)));
        printf("%c\n", operator);
        write(sock, &operator, sizeof(operator));
        fputs("have written loop", stdout);
    }
    operator = '=';
    fputs("have written ", stdout);
    write(sock, (char*)(operand+i), sizeof(*(operand + i)));
    write(sock, &operator, sizeof(operator));
    fputs("have written ", stdout);
    char result[BUFSIZE] = {0};
    int used_bytes = 0;
    int read_bytes = 0;
    int recv_bytes = 0;
    while((recv_bytes = read(sock, result + used_bytes, BUFSIZE)) != 0){
        used_bytes += recv_bytes;
        for(int j = read_bytes; j < used_bytes; ++j){
            if(result[j] == '='){
                //divide the message
                char *expression_begin = result + read_bytes;
                char *expression_end = result + j;
                *expression_end = '\0';
                int answer = atoi(expression_begin);
                printf("Operation result: %d", answer);
            }
        }
    }
    free(operand);
    close(sock);
    return 0;
}
