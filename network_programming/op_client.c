#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char * message);

int main(int argc, char * argv[])
{
    int sock;
    struct sockaddr_in serv_addr;

    if(argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
    else
        puts("Connected...........");
        
    fputs("Operand count: ", stdout);
    int count;
    scanf("%d", &count);
    write(sock, &count, sizeof(count));
    int num;
    for(int i = 0; i < count; i ++)
    {
        printf("Operand %d:", i + 1);
        scanf("%d", &num);
        write(sock, &num, sizeof(num));
    }
    getchar();
    char op;
    printf("Operator: ");
    scanf("%c", &op);
    write(sock, &op, sizeof(op));
    
    int ans = 0;
    read(sock, &ans, sizeof ans);
    printf("Operation result: %d\n", ans);
    
    
    close(sock);
    return 0;
}

void error_handling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

