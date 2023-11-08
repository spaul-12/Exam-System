#include "../Templates/template.h"
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#define PORT 8080
using namespace std;

Client::Client()
{
    char buffer[1024] = {0}; // to receive string from the server

    // Create a stream socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Initiate a socket connection
    int connection_status = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // Check for connection error
    if (connection_status < 0)
    {
        perror("Error\n");
        return;
    }

    printf("Connection established\n");

    // Send data to the socket
    int choice;
    system("clear");
    cout << "Enter your choice:\n1)Register\n2)Login\n3)Exit\n Choice: \n";
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        send(client_socket, &choice, sizeof(choice), 0);
        string userType;
        cout << "Are you a student or a teacher?(S for Student, T for Teacher) ";
        cin >> userType;
        registeruser(userType);
        break;
    }
    case 2:
    {
        send(client_socket, &choice, sizeof(choice), 0);
        string userType;
        cout << "Are you a student or a teacher?(S for Student, T for Teacher) ";
        cin >> userType;
        this->client = login(userType);
        break;
    }
    case 3:
    {
        // Close the connection
        cout << "Exiting!!" << endl;
        close(this->client_socket);
        exit(0);
    }
    }
}

void Client::registeruser(string &usertype)
{
    int code;
    userinfo *newUserInfo = new userinfo;

    cout << "Enter the username: " << endl;
    cin >> newUserInfo->username;

    cout << "Enter your password: ";
    cin >> newUserInfo->password;

    cout << "Enter your id: ";
    cin >> newUserInfo->id;

    newUserInfo->usertype = usertype;

    recv(client_socket, &code, sizeof(code), 0);

    if (code == 200)
    {
        if (usertype == "S")
        {
            this->client = new User(newUserInfo->username, newUserInfo->password, newUserInfo->id);
        }
        else
        {
            this->client = new User(newUserInfo->username, newUserInfo->password, newUserInfo->id);
        }

        cout << "Welcome, " << newUserInfo->username << ", to Exam center!!\n";
    }
    else
    {
        cout << "Registration Failed!" << endl;
        return ;
    }
}   