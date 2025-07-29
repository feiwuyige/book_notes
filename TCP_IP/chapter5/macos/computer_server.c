#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>

#define BUFSIZE 1024

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void splitString(const char *str, char delim, int *nums, int *count){
    int i = 0;
    int index = 0;
    while(*(str + i) != '\0'){
        if(isdigit(*(str + i))){
            int num = 0;
            while(isdigit(*(str + i))){
                num = num * 10 + (*(str + i) - '0');
                i++;
            }
            *(nums + index) = num;
            index++;
        }
        i++;
    }
    *count = index;
    return;
}

int compute(char *begin, char *end){
    char operator;
    char *p = begin;
    while(p < end){
        if(*p == '+' || *p == '-' || *p == '*'){
            operator = *p;
            break;
        }
        ++p;
    }

    *end = '\0';
    int nums[100] = {0};
    int count = 0;
    splitString(begin, operator, nums, &count);
    int result;
    switch(operator){
        case '+':
            for(int i = 0;i < count; ++i) result += nums[i];
            break;
        case '-':
            for(int i = 0;i < count; ++i) result -= nums[i];
            break;
        case '*':
            for(int i = 0;i < count; ++i) result *= nums[i];
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
    char message[BUFSIZE];
    for(int i = 0;i < 5;++i){
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1){
            error_handling("accept() error");
        }
        else{
            printf("Connected client %d \n", i + 1);
        }
        //use the "=" to divide the message
        int used_bytes = 0;
        int read_bytes = 0;
        int recv_bytes = 0;
        while((recv_bytes = read(clnt_sock, message + used_bytes, BUFSIZE)) != 0){
            used_bytes += recv_bytes;
            for(int j = read_bytes; j < used_bytes; ++j){
                if(message[j] == '='){
                    //divide the message
                    char *expression_begin = message + read_bytes;
                    char *expression_end = message + j;
                    int answer = compute(expression_begin, expression_end);
                    read_bytes = j + 1;
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}