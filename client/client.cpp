// C program for the Client Side
#include "../Templates/template.h"
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
void *clienthread(void *args)
{

	Client new_client;        // calls client class constructor
	pthread_exit(NULL);
	return 0;
}

// Driver Code
int main()
{
	pthread_t tid;

	pthread_create(&tid, NULL,clienthread,NULL);
	pthread_join(tid, NULL);
}
