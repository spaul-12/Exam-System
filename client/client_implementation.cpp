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

User::User(string username,string password,string usertype)
{
    this->username = username;
    this->password = password;
    this->usertype = usertype;
}

User::User(string password)
{
    this->password = password;
}

void User::user_specific_functions(int client_socket)
{
    cout<<"Overrideable function \n";
    return ;
}

string User::getUsername()
{
    return username;
}

Student::Student(string username, string password, string usertype, string rollno, string department): User(username, password, usertype)
{
    this->rollno = rollno;
    this->department = department;
}

Student::Student(string id, string password): User(password)
{
    this->rollno = id;
}

Teacher::Teacher(string username, string password, string usertype, string id, string department): User(username, password, usertype)
{
    this->teacherid = id;
    this->department = department;
}

Teacher::Teacher(string id, string password): User(password)
{
    this->teacherid = id;
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
    char userType;
    cout << "Are you a student or a teacher?(S for Student, T for Teacher) \n";
    cin >> userType;
    cout << "Enter your choice:\n1) Register\n2) Login\n3) Exit\n";
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        code = REGISTRATION_CODE;
        send(this->client_socket, &code, sizeof(code), 0);
        registeruser(userType);
        break;
    }
    case 2:
    {
        code = LOGIN_CODE;
        send(this->client_socket, &code, sizeof(code), 0);
        login(userType);
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
    string id,department;

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
        id = newUserInfo->rollno;
        department = newUserInfo->department;
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
        id = newUserInfo->teacherid;
        department = newUserInfo->department;
    }

    recv(client_socket, &code, sizeof(code), 0);

    if (code == 200)
    {
        cout << "Welcome, " << clientInfo->username << ", to Exam center!!\n";

        if (usertype == 'S')
        {
            this->client = new Student(clientInfo->username, clientInfo->password, clientInfo->usertype, id, department);
            Student *student = dynamic_cast<Student *>(this->client);
            if(student==NULL)
            {
                cout<<"downcasting failed\n"<<endl;
                code = END_CONNECTION_CODE;
                send(client_socket,&code,sizeof(code),0);
                exit(0);
            }
            student->user_specific_functions(client_socket);
        }
        else
        {
            this->client = new Teacher(clientInfo->username, clientInfo->password, clientInfo->usertype, id, department);
            Teacher *teacher = dynamic_cast<Teacher *>(this->client);
            if(teacher==NULL)
            {
                cout<<"downcasting failed\n"<<endl;
                code = END_CONNECTION_CODE;
                send(client_socket,&code,sizeof(code),0);
                exit(0);
            }
            teacher->user_specific_functions(client_socket);
        }

    }
    else
    {
        cout << "Registration Failed!" << endl;
        return ;
    }
}  

void Client::login(char &usertype)
{
    send(client_socket,&usertype,sizeof(usertype),0);  // to call specific login func
    int code;

    loginInfo *userInfo = new loginInfo;

    while(1)
    {
        cout<<login_menu;
        cin>>userInfo->id;
        cin>>userInfo->password;
        
        send(client_socket,userInfo,sizeof(* userInfo),0);

        recv(client_socket,&code,sizeof(code),0);

        if(code == SUCCESSFUL_CODE)
        {
            cout << "Welcome to Exam center!!\n";
            if(usertype=='S')
            {
                this->client = new Student(userInfo->id, userInfo->password);
                Student *student = dynamic_cast<Student*>(this->client);
                student->user_specific_functions(client_socket);
            }
            else
            {
                this->client = new Teacher(userInfo->id, userInfo->password);
                Teacher *teacher = dynamic_cast<Teacher *>(this->client);
                teacher->user_specific_functions(client_socket);
            }
        }
        else
        {
            cout<<"Invalid credentials..";
        }

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
        bool endflag = false;
        switch (ch)
        {
            case 1:
            {
                code = START_EXAM_CODE;
                send(client_socket,&code,sizeof(code),0);
                break;
            }
            case 2:
            {
                code = END_CONNECTION_CODE;
                send(client_socket,&code,sizeof(code),0);
                endflag = true;
                break;
            }
        }
        if(endflag)
        break;
    }
    cout<<"Connection ended :("<<endl;
    exit(0);
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