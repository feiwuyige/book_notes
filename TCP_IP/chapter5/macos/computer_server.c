#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdint.h>

#define BUFSIZE 1024
#define OPSIZE 4

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


int32_t calculate(int8_t op_num, int32_t *operands, char op){
    int32_t result = operands[0];
    switch(op){
        case '+':
            for(int8_t i = 1;i < op_num; ++i){
                result += operands[i];
            }
            break;
        case '-':
            for(int8_t i = 1;i < op_num; ++i){
                result -= operands[i];
            }
            break;
        case '*':
            for(int8_t i = 1;i < op_num; ++i){
                result *= operands[i];
            }
            break;
        default:
            break;
    }
    return result;
}



int main(int argc, char *argv[]){
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    if(argc != 2){
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    if(serv_sock == -1){
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    //bind
    if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("bind() error");
    }
    //listen
    if(listen(serv_sock, 5) == -1){
        error_handling("listen() error");
    }

    clnt_addr_size = sizeof(clnt_addr);
    //accept
    for(int i = 0;i < 5;++i){
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1){
            error_handling("accept() error");
        }
        else{
            printf("Connected client %d \n", i + 1);
        }
        int8_t opnt;
        read(clnt_sock, &opnt, 1);
        char opinfo[BUFSIZE];
        int recv_len = 0, recv_cnt;
        while((opnt * OPSIZE + 1) > recv_len){
            recv_cnt = read(clnt_sock, &opinfo[recv_len], BUFSIZE - 1);
            recv_len += recv_cnt;
        }
        int32_t result = calculate(opnt, (int32_t*)opinfo, opinfo[recv_len - 1]);
        write(clnt_sock, (char*)&result, sizeof(result));
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}
