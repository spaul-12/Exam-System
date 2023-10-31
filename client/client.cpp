// C program for the Client Side
#include<bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#define PORT 8080
using namespace std;
// Function to send data to
// server socket.
void* clienthread(void* args)
{

	char buffer[1024]={0};        // to receive string from the server
	int client_request = *((int*)args); 
	int network_socket;

	// Create a stream socket
	network_socket = socket(AF_INET,
							SOCK_STREAM, 0);

	// Initialise port number and address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT);

	// Initiate a socket connection
	int connection_status = connect(network_socket,
									(struct sockaddr*)&server_address,
									sizeof(server_address));

	// Check for connection error
	if (connection_status < 0) {
		perror("Error\n");
		return 0;
	}

	printf("Connection established\n");

	// Send data to the socket
	send(network_socket, &client_request,
		sizeof(client_request), 0);

	printf("data send successfully\n");

	// Receive data from the server
  	int valread = read(network_socket, buffer, 1024 - 1); // subtract 1 for the null terminator at the end
	printf("message received :");
    printf("%s\n", buffer);

	// Close the connection
	close(network_socket);
	pthread_exit(NULL);

	return 0;
}

// Driver Code
int main()
{
	printf("1. Student\n");
	printf("2. Instructor\n");

	// Input
	int choice;
	scanf("%d", &choice);
	pthread_t tid;

	// Create connection
	// depending on the input
	switch (choice) {
	case 1: {
		int client_request = 1;

		// Create thread
		pthread_create(&tid, NULL,
					clienthread,
					&client_request);
		sleep(20);
		break;
	}
	case 2: {
		int client_request = 2;

		// Create thread
		pthread_create(&tid, NULL,
					clienthread,
					&client_request);
		sleep(20);
		break;
	}
	default:
		printf("Invalid Input\n");
		break;
	}

	// Suspend execution of
	// calling thread
	pthread_join(tid, NULL);
}
