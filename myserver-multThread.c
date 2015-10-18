#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXLINE (128)
#define PORT (6666)

pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

struct socket_thread_data
{
	int connectfd;
	struct sockaddr_in clientAddr;
};

void* socket_handle(void* pSocket_thread_data)
{
	// copy data from main function
	struct socket_thread_data socketThreadData;
	pthread_mutex_lock(&data_mutex);
	memcpy(&socketThreadData, (struct socket_thread_data*)pSocket_thread_data, sizeof(struct socket_thread_data));
	pthread_mutex_unlock(&data_mutex);
	
	pthread_t tid = pthread_self();
	pthread_detach(tid);
	int connectfd = socketThreadData.connectfd;
	char ipAddr[16] = "\0";
	inet_ntop(AF_INET, &(socketThreadData.clientAddr.sin_addr), ipAddr, 16);
	printf("connectfd : %d\n", connectfd);
	printf("connectfd : %d\n", connectfd);
	char buffer[MAXLINE] = "\0";
	printf("debug\n");
	printf("debug\n");
	int n = -1;
	while(1)
	{
		n = recv(connectfd, buffer, MAXLINE, 0);
		printf("receive return %d\n", n);
		if(n == 0)
		{
			printf("client close link\n");
			return NULL;
		}
		else if(n < 0)
		{
			printf("receive client data error: %s(%d)\n", strerror(errno), errno);
			return NULL;
		}
        buffer[n] = '\0';
        
        printf("thread %lu, receive msg from client(ip: %s): %s", (unsigned long)tid, ipAddr, buffer);
        if(strncmp(buffer, "exit\n", strlen("exit\n")) == 0)
        {
            printf("thread %lu, close with client\n", (unsigned long)tid); 
            return NULL;
        }
	}
}

int main(int argc, char** argv)
{
    int listenfd,connectfd;
    struct sockaddr_in serverAddr;
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
	struct socket_thread_data socketThreadData;
	struct sockaddr_in clientAddr;
	socklen_t clientLen;
	memset(&socketThreadData, 0, sizeof(struct socket_thread_data));
    while(1)
    {
        connectfd = accept(listenfd, (struct sockaddr*)&clientAddr, &clientLen);
        if(connectfd == -1)
        {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            return -1;
        }
		socketThreadData.connectfd = connectfd;
		memcpy(&(socketThreadData.clientAddr), &clientAddr, sizeof(struct sockaddr));
		
		//create thread to handle task
		pthread_create(NULL, NULL, &socket_handle, &socketThreadData);
	    
    }
    close(listenfd);
    return 0;;
}
