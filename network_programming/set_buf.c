#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char * message);

int main()
{
    int sock;
    int snd_buf = 1024 * 3, rcv_buf = 1024 * 3;
    socklen_t len;
    
    sock = socket(PF_INET, SOCK_STREAM, 0);

    int state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf));
    if(state)
        error_handling("setsockopt() error");

    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF,
                       (void*)&rcv_buf, &len);
    if(state)
        error_handling("getsockopt() error");
    
    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));
    if(state)
        error_handling("setsockopt() error");
    
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF,
                       (void*)&snd_buf, &len);
    if(state)
        error_handling("getsockopt() error");

    printf("Input buf size : %d \n", rcv_buf);
    printf("Output buf size : %d \n", snd_buf);

    return 0;
}

void error_handling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

