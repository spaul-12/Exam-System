#include "../Templates/template.h"
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
using namespace std;
// Semaphore variables
sem_t x, y;

// Reader Function

void Student(int clientSocket)
{
	

}

// Writer Function
void Instructor(void *param)
{
	
	pthread_exit(NULL);
}
void *clientConnection(void *param)
{
	int clientSocket = *((int *)param);
	int choice;
	recv(clientSocket,&choice,sizeof(choice),0);
	if(choice==1)
	{
		server_side_registration(clientSocket);
	}

}
// Driver Code
int main()
{
	// Initialize variables
    pthread_t thread[10000];
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;
	sem_init(&x, 0, 1);
	sem_init(&y, 0, 1);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		printf("connection failed\n");
		exit(0);
	}
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);

	// Bind the socket to the
	// address and port number.
	if (bind(serverSocket,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
	{
		printf("bind failed\n");
		exit(0);
	}

	// Listen on the socket,
	// with 50 max connection
	// requests queued
	if (listen(serverSocket, 50) == 0)
		printf("Listening....\n");
	else
		printf("Error\n");

	// Array for thread
	pthread_t tid[60];

	int i = 0;

	while (1)
	{
		addr_size = sizeof(serverStorage);

		// Extract the first
		// connection in the queue
		newSocket = accept(serverSocket,(struct sockaddr *)&serverStorage,&addr_size);
		if (newSocket < 0)
		{
			printf("accept failed\n");
			exit(0);
		}
		
		if(pthread_create(&thread[i++],NULL,clientConnection,&newSocket)!=0)
		{
			printf("Failed to create client thread\n");
		}

	}

	return 0;
}
