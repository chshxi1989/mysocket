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

typedef struct
{
	int connectfd;
	struct sockaddr_in clientAddr;
} socket_thread_data;

socket_thread_data socketThreadData;

void dump_mem(char* pStart, int length)
{
    int i;
    for(i = 0; i < length; i++)
    {
        printf("0x%x ", pStart[i]);
        if((i+1)%16 == 0)
        {
             printf("\n");
        }
    }
    printf("\n");
}

void* socket_handle(void* cookie)
{
    printf("[%s][%d]\n", __FUNCTION__, __LINE__);

	// copy data from main function
	socket_thread_data hsocketThreadData;
	pthread_mutex_lock(&data_mutex);
	memcpy(&hsocketThreadData, &socketThreadData, sizeof(socket_thread_data));
	pthread_mutex_unlock(&data_mutex);
	
	pthread_t tid = pthread_self();
	pthread_detach(tid);
	int connectfd = hsocketThreadData.connectfd;
	char ipAddr[16] = "\0";
    printf("connectfd : %d\n", connectfd);
	inet_ntop(AF_INET, &(hsocketThreadData.clientAddr.sin_addr), ipAddr, 16);
	char buffer[MAXLINE] = "\0";
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
        
        printf("thread 0x%lx, receive msg from client(ip: %s): %s", (unsigned long)tid, ipAddr, buffer);
        if(strncmp(buffer, "exit\n", strlen("exit\n")) == 0)
        {
            printf("thread 0x%lx, close with client\n", (unsigned long)tid); 
            return NULL;
        }
		// send response to client
		send(connectfd, buffer, strlen(buffer), 0);
	}
    return NULL;
}

int main(int argc, char** argv)
{
    int listenfd,connectfd;
    struct sockaddr_in serverAddr;
    pthread_t thread;
    
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
	struct sockaddr_in clientAddr;
	socklen_t clientLen;
    while(1)
    {
        connectfd = accept(listenfd, (struct sockaddr*)&clientAddr, &clientLen);
        if(connectfd == -1)
        {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            return -1;
        }
        printf("[%s][%d]connectfd: %d\n", __FUNCTION__, __LINE__, connectfd);
        dump_mem((char*)&(clientAddr.sin_addr), sizeof(clientAddr.sin_addr));
		
        pthread_mutex_lock(&data_mutex);
		socketThreadData.connectfd = connectfd;
		memcpy(&(socketThreadData.clientAddr), &clientAddr, sizeof(clientAddr));
		pthread_mutex_unlock(&data_mutex);
		
		//create thread to handle task
		pthread_create(&thread, NULL, socket_handle, NULL);
	    
    }
    close(listenfd);
    return 0;;
}
