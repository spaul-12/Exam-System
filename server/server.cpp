#include "../Templates/template.h"
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
using namespace std;

//semaphore variables
sem_t *student_regFileSemaphore;
sem_t *teacher_regFileSemaphore;



void *clientConnection(void *param)
{
	cout<<"Connected to server\n";
	int success_code = SUCCESSFUL_CODE;
	int newSocket = *((int *)param);
	bool endflag = false;
	pthread_t ptid = pthread_self();
	int choice;
	while(1)
	{
		// Receive a request code from the client
        recv(newSocket, &choice, sizeof(choice), 0);
        //cout << choice << endl;
		cout<<choice<<endl;
        switch (choice)
        {
        	case END_CONNECTION_CODE:
        	{
        	    endflag = true;
        	    break;
        	}
        	
        	case REGISTRATION_CODE:
        	{
        	    // Handle user registration request
				cout<<"Registration started..\n";
				char usertype;
        	    recv(newSocket,&usertype,sizeof(usertype),0);
				cout<<usertype<<endl;
				if(usertype=='S')
				{
					sem_post(student_regFileSemaphore);
					server_side_student_registration(newSocket);
					sem_wait(student_regFileSemaphore);
				}
				else
				{
					sem_post(teacher_regFileSemaphore);
					server_side_teacher_registration(newSocket);
					sem_wait(teacher_regFileSemaphore);
				}
				send(newSocket,&success_code,sizeof(success_code),0);
        	    break;
        	}
        	case LOGIN_CODE:
        	{
        	    // Handle user login request
        	    string usertype;
        	    recv(newSocket,&usertype,sizeof(usertype),0);
				if(usertype=="S")
				{
					sem_post(student_regFileSemaphore);
					//server_side_student_login(newSocket);
					sem_wait(student_regFileSemaphore);
				}
				else
				{
					sem_post(teacher_regFileSemaphore);
					//server_side_teacher_login(newSocket);
					sem_wait(teacher_regFileSemaphore);
				}
				send(newSocket,&success_code,sizeof(success_code),0);
        	    break;
        	}
        }
        if (endflag)
            break;
	
    }
	pthread_exit(&ptid);	
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
	
    //  binary semaphores with an initial value 1
    student_regFileSemaphore = sem_open (SEMAPHORE_NAME1, O_CREAT, 0660, 1);
	teacher_regFileSemaphore = sem_open(SEMAPHORE_NAME2,O_CREAT,0660,1);

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
