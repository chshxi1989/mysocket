#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE (4096)
#define PORT (6666)
int main(int argc, char** argv)
{
    int listenfd,connectfd;
    struct sockaddr_in serverAddr;
    char buffer[MAXLINE];
    int n;
    // create listen socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1)
    {
        printf("create socket error: %s(errno : %d)\n", strerror(errno), errno);
        return -1;
    }
    
    // bind socket and port
    memset(&serverAddr, 0 , sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    if(bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    
    // listen
    if(listen(listenfd, 10) == -1)
    {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    
    // wait for client's request
    printf("======waiting for client's request=======\n");
    
    // fork process
    pid_t pid = -1;
    struct sockaddr_in clientAddr;
    socklen_t clientLen;
    memset(&clientAddr, 0, sizeof(clientAddr));
    while(1)
    {
        connectfd = accept(listenfd, (struct sockaddr*)&clientAddr, &clientLen);
        if(connectfd == -1)
        {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            return -1;
        }
        pid = fork();
        if(pid == 0)
        {
            // sub process
            close(listenfd);
            pid = getpid();
            char ipAddr[16] = "\0";
            while(1)
            {
                n = recv(connectfd, buffer, MAXLINE, 0);
				printf("receive return %d\n", n);
				if(n == 0)
				{
					printf("client close link\n");
					exit(0);
				}
				else if(n < 0)
				{
					printf("receive client data error: %s(%d)\n", strerror(errno), errno);
					exit(0);
				}
                buffer[n] = '\0';
                inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, 16);
                printf("process %d, receive msg from client(ip: %s): %s", pid, ipAddr, buffer);
                if(strncmp(buffer, "exit\n", strlen("exit\n")) == 0)
                {
                    printf("process %d, close with client\n", pid); 
                    break;
                }
            }
            close(connectfd);
            exit(0);
        }
        // father process
        close(connectfd);
    }
    close(listenfd);
    return 0;;
}
