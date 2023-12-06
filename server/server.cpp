#include "../Templates/template.h"
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>
#include <string>
#define PORT 8080
using namespace std;

// semaphore variables
sem_t *student_regFileSemaphore;
sem_t *teacher_regFileSemaphore;
sem_t *readFileSemaphore;
sem_t *queFileSemaphores[4];
sem_t *resultFileSemaphores[4];
sem_t *readResultFile;

map<string,int>deptIndex ;
void initializeDeptIndex()
{
	deptIndex["CS"] = 0;
	deptIndex["ECE"] = 1;
	deptIndex["EEE"] = 2;
	deptIndex["MECH"] = 3;
}
Question deptQuestionBank[4];

void parseQuestionFiles()
{
	for(auto it:deptIndex)
	{
		addQuestionFromFile(it.first,deptQuestionBank[it.second]);
	}
}

void *clientConnection(void *param)
{
	cout << "Connected to server\n";
	int success_code = SUCCESSFUL_CODE;
	int newSocket = *((int *)param);
	bool endflag = false;
	pthread_t ptid = pthread_self();
	int choice;
	while (1)
	{
		// Receive a request code from the client
		recv(newSocket, &choice, sizeof(choice), 0);
		cout << choice << endl;
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
			cout << "Registration started..\n";
			char usertype;
			recv(newSocket, &usertype, sizeof(usertype), 0);
			if (usertype == 'S')
			{
				sem_wait(student_regFileSemaphore);
				server_side_student_registration(newSocket);
				sem_post(student_regFileSemaphore);
			}
			else
			{
				sem_wait(teacher_regFileSemaphore);
				server_side_teacher_registration(newSocket);
				sem_post(teacher_regFileSemaphore);
			}
			send(newSocket, &success_code, sizeof(success_code), 0);
			break;
		}
		case LOGIN_CODE:
		{
			// Handle user login request
			sem_post(readFileSemaphore);
			server_side_login(newSocket);
			sem_post(readFileSemaphore);
			send(newSocket, &success_code, sizeof(success_code), 0);
			break;
		}
		case SET_QUESTION_CODE:
		{
			// Handle question setting
			char dept[10];
			recv(newSocket, &dept, sizeof(dept), 0);
			int index = deptIndex[dept];
			sem_wait(queFileSemaphores[index]);
			setQuestion(newSocket,dept, deptQuestionBank[index]);
			sem_post(queFileSemaphores[index]);
			break;
		}
		case START_EXAM_CODE:
		{
			char dept[10];
			recv(newSocket, &dept,sizeof(dept),0);
			int index = deptIndex[dept];
			char id[100];
			recv(newSocket,&id,sizeof(id),0);
			int marksObtained = deptQuestionBank[index].startExam(newSocket);
			sem_wait(resultFileSemaphores[index]);
			updateResult(id,dept,marksObtained);
			sem_post(resultFileSemaphores[index]);
			break;
		}
		case LEADERBOARD_CODE:
		{
			char dept[10];
			recv(newSocket, &dept, sizeof(dept), 0);
			sem_wait(readResultFile);
			getLeaderboard(newSocket,dept);
			sem_post(readResultFile);
			
			break;
		}
		case SEE_QUESTION_CODE:
		{
			char dept[10];
			recv(newSocket, &dept, sizeof(dept), 0);
			int index = deptIndex[dept];
			deptQuestionBank[index].sendQuestions(newSocket);
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
	initializeDeptIndex();
	parseQuestionFiles();
	// Initialize variables
	pthread_t thread[10000];
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;

	//  binary semaphores with an initial value 1
	char semaphoreName[16];
	char resultSemaphoreName[25];
	student_regFileSemaphore = sem_open(SEMAPHORE_NAME1, O_CREAT, 0660, 1);
	teacher_regFileSemaphore = sem_open(SEMAPHORE_NAME2, O_CREAT, 0660, 1);
	readFileSemaphore = sem_open(SEMAPHORE_NAME3, O_CREAT, 0660, 1);
	readResultFile = sem_open(SEMAPHORE_NAME4, O_CREAT, 0660, 1);
	for (int i = 0; i < 4; i++)
	{
		sprintf(semaphoreName, "my_semaphore_%d", i);
		sprintf(resultSemaphoreName,"result_semaphore_%d",i);
		queFileSemaphores[i] = sem_open(semaphoreName, O_CREAT, 0660, 1);
		resultFileSemaphores[i] = sem_open(resultSemaphoreName, O_CREAT, 0660, 1);
	}

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
	if (::bind(serverSocket,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
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
		newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
		if (newSocket < 0)
		{
			printf("accept failed\n");
			exit(0);
		}

		if (pthread_create(&thread[i++], NULL, clientConnection, &newSocket) != 0)
		{
			printf("Failed to create client thread\n");
		}
	}

	return 0;
}
