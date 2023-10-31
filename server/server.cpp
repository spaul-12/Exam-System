#include<bits/stdc++.h>
// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080



// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

// Reader Function
void* reader(void* param)
{
	// Lock the semaphore
	sem_wait(&x);
	readercount++;

	if (readercount == 1)
		sem_wait(&y);

	// Unlock the semaphore
	sem_post(&x);

	printf("\n%d reader is inside",
		readercount);

	sleep(5);

	// Lock the semaphore
	sem_wait(&x);
	readercount--;

	if (readercount == 0) {
		sem_post(&y);
	}

	// Lock the semaphore
	sem_post(&x);

	printf("\n%d Reader is leaving",
		readercount + 1);
	pthread_exit(NULL);
}

// Writer Function
void* writer(void* param)
{
	printf("\nWriter is trying to enter");

	// Lock the semaphore
	sem_wait(&y);

	printf("\nWriter has entered");

	// Unlock the semaphore
	sem_post(&y);

	printf("\nWriter is leaving");
	pthread_exit(NULL);
}

// Driver Code
int main()
{
	// Initialize variables
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;
	sem_init(&x, 0, 1);
	sem_init(&y, 0, 1);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket<0)
	{
		printf("connection failed\n");
		exit(0);
	}
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);

	// Bind the socket to the
	// address and port number.
	if(bind(serverSocket,
		(struct sockaddr*)&serverAddr,
		sizeof(serverAddr))<0){
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

	while (1) {
		addr_size = sizeof(serverStorage);

		// Extract the first
		// connection in the queue
		newSocket = accept(serverSocket,
						(struct sockaddr*)&serverStorage,
						&addr_size);
		if(newSocket<0)
		{
			printf("accept failed\n");
			exit(0);
		}
		int choice = 0;
		recv(newSocket,
			&choice, sizeof(choice), 0);

		/*if (choice == 1) {
			// Creater readers thread
			if (pthread_create(&readerthreads[i++], NULL,
							reader, &newSocket)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
		}
		else if (choice == 2) {
			// Create writers thread
			if (pthread_create(&writerthreads[i++], NULL,
							writer, &newSocket)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
		}
		*/
		printf("data received\n");
		printf("val received is %d",choice);

		/*if (i >= 50) {
			// Update i
			i = 0;

			while (i < 50) {
				// Suspend execution of
				// the calling thread
				// until the target
				// thread terminates
				pthread_join(writerthreads[i++],
							NULL);
				pthread_join(readerthreads[i++],
							NULL);
			}

			// Update i
			i = 0;
		}*/

		printf("\n moving to receive the next connection \n");
	}

	return 0;
}
