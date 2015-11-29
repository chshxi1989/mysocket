#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT (6666)
#define MAXLINE (128)
int main(int argc, char** argv)
{
    int socketfd = 0;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;
    
    // check args
    if(argc != 2)
    {
        printf("args error:\n");
        printf("          myclient ipaddr[x.x.x.x]\n");
        return -1;
    }
    
    // init socket
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if( socketfd == -1)
    {
        printf("create socket failed: %s(errno : %d)\n)", strerror(errno), errno);
        return -1;
    }
    
    // create dest socket addr
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        printf("get server IP address error, %s\n", argv[1]);
        return -1;
    }

    // connect server
    if(connect(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect to dest error:%s(errno :%d)", strerror(errno), errno);
        return -1;
    }
    
    //send msg to server
    while(1)
    {
        fgets(buffer, sizeof(buffer), stdin);
        if(strcmp(buffer, "exit") == 0)
        {
            break;
        }
        if(send(socketfd, buffer, strlen(buffer), 0) == -1)
        {
            printf("send msg to server error: %s(errno: %d)", strerror(errno), errno);
            return -1;
        }
		// receive response from server
		recv(socketfd, buffer, MAXLINE, 0);
		printf("receive msg from server: %s\n", buffer);
    }
    close(socketfd);
    return 0;
}

