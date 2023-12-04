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

void User::user_specific_functions(int client_socket)
{
    cout<<"Not necessary\n";
    return ;
}

Client::Client()
{
    // Create a stream socket
    this->client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Initiate a socket connection
    int connection_status = connect(this->client_socket, (struct sockaddr *)&(server_address), sizeof(server_address));

    // Check for connection error
    if (connection_status < 0)
    {
        perror("Error\n");
        return;
    }

    printf("Connection established\n");

    // Send data to the socket
    int choice,code;
    system("clear");
    cout << "Enter your choice:\n1) Register\n2) Login\n3) Exit\n";
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        code = REGISTRATION_CODE;
        send(this->client_socket, &code, sizeof(code), 0);
        char userType;
        cout << "Are you a student or a teacher?(S for Student, T for Teacher) ";
        cin >> userType;
        registeruser(userType);
        break;
    }
    case 2:
    {
        code = LOGIN_CODE;
        send(this->client_socket, &code, sizeof(code), 0);
        char userType;
        cout << "Are you a student or a teacher?(S for Student, T for Teacher) ";
        cin >> userType;
        //login(userType);
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

void Client::registeruser(char &usertype)
{
    send(client_socket,&usertype,sizeof(usertype),0);  // to call specific registration func

    int code;

    ClientUserInfo *clientInfo = new ClientUserInfo;
    if(usertype =='S')
    {
        StudentUserInfo *newUserInfo = new StudentUserInfo;
        cout<<student_register_menu;
        cin>>newUserInfo->username;
        cin>>newUserInfo->password;
        cin>>newUserInfo->rollno;
        cin>>newUserInfo->department;

        send(client_socket,newUserInfo,sizeof(* newUserInfo),0);

        clientInfo->username = newUserInfo->username;
        clientInfo->password = newUserInfo->password;
        clientInfo->usertype = "S";
    }
    else
    {
        TeacherUserInfo *newUserInfo = new TeacherUserInfo;
        cout<<teacher_register_menu;
        cin>>newUserInfo->username;
        cin>>newUserInfo->password;
        cin>>newUserInfo->teacherid;
        cin>>newUserInfo->department;
        
        
        send(client_socket,newUserInfo,sizeof(* newUserInfo),0);

        clientInfo->username = newUserInfo->username;
        clientInfo->password = newUserInfo->password;
        clientInfo->usertype = "T";
    }

    recv(client_socket, &code, sizeof(code), 0);

    if (code == 200)
    {
        this->client = new User(clientInfo->username, clientInfo->password, clientInfo->usertype);
        cout << "Welcome, " << clientInfo->username << ", to Exam center!!\n";

        int code = END_CONNECTION_CODE;
        send(client_socket,&code,sizeof(code),0);
        /* if (usertype == 'S')
        {
            Student *student = dynamic_cast<Student *>(this->client);
            student->user_specific_functions(client_socket);
        }
        else
        {
            Teacher *teacher = dynamic_cast<Teacher *>(this->client);
            teacher->user_specific_functions(client_socket);
        } */

    }
    else
    {
        cout << "Registration Failed!" << endl;
        return ;
    }
}  

void Student::user_specific_functions(int client_socket)
{
    while(1)
    {
        int code ;
        int ch;
        cout<<"1) start exam \n 2) exit\n";
        cin>>ch;
        switch (ch)
        {
            case 1:
            {
                code = START_EXAM_CODE;
                send(client_socket,&code,sizeof(code),0);
            }
        }
    }
}

void Teacher::user_specific_functions(int client_socket)
{
    while(1)
    {
        int code ;
        int ch;
        cout<<"1) set exam questions\n 2) exit\n";
        cin>>ch;
        switch (ch)
        {
            case 1:
            {
                code = START_EXAM_CODE;
                send(client_socket,&code,sizeof(code),0);
            }
        }
    }
}