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

bool validUsertype(char &userType)
{
    return (userType == 'S' || userType == 'T');
}

bool isValidAnswer(string answer)
{
    return (answer[0]>='a' and answer[0]<='d');
}

User::User(string username, string password, string usertype)
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
    cout << "Overrideable function \n";
    return;
}

string User::getUsername()
{
    return username;
}

Student::Student(string username, string password, string usertype, string rollno, string department) : User(username, password, usertype)
{
    this->rollno = rollno;
    this->department = department;
}

Student::Student(string id, string dept, string password) : User(password)
{
    this->rollno = id;
    this->department = dept;
}

Teacher::Teacher(string username, string password, string usertype, string id, string department) : User(username, password, usertype)
{
    this->teacherid = id;
    this->department = department;
}

Teacher::Teacher(string id, string dept, string password) : User(password)
{
    this->teacherid = id;
    this->department = dept;
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
    int choice, code;
    char userType;
    system("clear");
    while (1)
    {
        cout << "Are you a student or a teacher?(S for Student, T for Teacher) \n";
        cin >> userType;
        if(validUsertype(userType))
        {
            break;
        }
        else
        {
            cout<<"\nInvalid input :( . Enter again..\n\n";
        }
    }
    cout << "Enter your choice:\n 1) Register\n 2) Login\n 3) Exit\n";
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
    send(client_socket, &usertype, sizeof(usertype), 0); // to call specific registration func

    int code;
    string id, department;

    ClientUserInfo *clientInfo = new ClientUserInfo;
    if (usertype == 'S')
    {
        StudentUserInfo *newUserInfo = new StudentUserInfo;
        cout << student_register_menu;
        cin >> newUserInfo->username;
        cin >> newUserInfo->password;
        cin >> newUserInfo->rollno;
        cin >> newUserInfo->department;

        send(client_socket, newUserInfo, sizeof(*newUserInfo), 0);

        clientInfo->username = newUserInfo->username;
        clientInfo->password = newUserInfo->password;
        clientInfo->usertype = "S";
        id = newUserInfo->rollno;
        department = newUserInfo->department;
    }
    else
    {
        TeacherUserInfo *newUserInfo = new TeacherUserInfo;
        cout << teacher_register_menu;
        cin >> newUserInfo->username;
        cin >> newUserInfo->password;
        cin >> newUserInfo->teacherid;
        cin >> newUserInfo->department;

        send(client_socket, newUserInfo, sizeof(*newUserInfo), 0);

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
            if (student == NULL)
            {
                cout << "downcasting failed\n"
                     << endl;
                code = END_CONNECTION_CODE;
                send(client_socket, &code, sizeof(code), 0);
                close(this->client_socket);
                exit(0);
            }
            student->user_specific_functions(client_socket);
        }
        else
        {
            this->client = new Teacher(clientInfo->username, clientInfo->password, clientInfo->usertype, id, department);
            Teacher *teacher = dynamic_cast<Teacher *>(this->client);
            if (teacher == NULL)
            {
                cout << "downcasting failed\n"
                     << endl;
                code = END_CONNECTION_CODE;
                send(client_socket, &code, sizeof(code), 0);
                close(this->client_socket);
                exit(0);
            }
            teacher->user_specific_functions(client_socket);
        }
    }
    else
    {
        cout << "Registration Failed!" << endl;
    }
    close(this->client_socket);
    exit(1);
}

void Client::login(char &usertype)
{
    send(client_socket, &usertype, sizeof(usertype), 0); // to call specific login func
    int code;

    loginInfo *userInfo = new loginInfo;

    while (1)
    {
        cout << login_menu;
        cin >> userInfo->id;
        cin >> userInfo->password;

        send(client_socket, userInfo, sizeof(*userInfo), 0);

        recv(client_socket, &code, sizeof(code), 0);

        if (code == SUCCESSFUL_CODE)
        {
            cout << "Welcome to Exam center!!\n";
            string department = parseDepartment(userInfo->id);
            if (usertype == 'S')
            {
                this->client = new Student(userInfo->id, department, userInfo->password);
                Student *student = dynamic_cast<Student *>(this->client);
                student->user_specific_functions(client_socket);
            }
            else
            {
                this->client = new Teacher(userInfo->id, department, userInfo->password);
                Teacher *teacher = dynamic_cast<Teacher *>(this->client);
                teacher->user_specific_functions(client_socket);
            }
        }
        else if (code == LOGIN_FAIL_CODE)
        {
            cout << "Invalid credentials..";
        }
        else if (code == SERVER_ERROR_CODE)
        {
            cout << "Internal Server Error. Try again later\n";
            break;
        }
    }
    close(this->client_socket);
    exit(1);
}

void Student::user_specific_functions(int client_socket)
{
    while (1)
    {
        int code;
        int ch;
        cout << " 1) start exam \n 2) see leaderboard \n 3) exit \n";
        cin >> ch;
        bool endflag = false;
        switch (ch)
        {
        case 1:
        {
            code = START_EXAM_CODE;
            send(client_socket, &code, sizeof(code), 0);
            char dept[10];
            strcpy(dept, this->department.c_str());
            send(client_socket, &dept, sizeof(dept), 0);
            char id[100];
            cout << "Enter your roll number: \n";
            cin.ignore();
            cin.getline(id, 100);
            send(client_socket, &id, sizeof(id), 0);
            while (1)
            {
                recv(client_socket, &code, sizeof(code), 0);
                if (code == RECIEVE_QUESTION_CODE)
                {
                    StudentQuestion *question = new StudentQuestion;
                    recv(client_socket, question, sizeof(*question), 0);
                    cout << question->que << endl;
                    cout << "a: " << question->opt1 << endl;
                    cout << "b: " << question->opt2 << endl;
                    cout << "c: " << question->opt3 << endl;
                    cout << "d: " << question->opt4 << endl;
                    cout << "Marks: " << question->marks << endl;
                    char answer[5];
                    while(1)
                    {
                        cout << "Enter your answer (a/b/c/d) :" << endl;
                        cin.getline(answer, 5);
                        if(isValidAnswer(answer)) break;
                        else
                        {
                            cout<<"\nInvalid input. Try again\n\n";
                        }
                    }
                    send(client_socket, &answer, sizeof(answer), 0);
                }
                else if(code == END_EXAM_CODE)
                {
                    cout << "\nThanks for taking the exam\n\n";
                    int marksObtained;
                    recv(client_socket, &marksObtained, sizeof(marksObtained), 0);
                    cout << "Marks obtained: " << marksObtained << endl;
                    break;
                }
                else if(code == EMPTY_QUESTIONBANK_CODE)
                {
                    cout<<" \nTeacher did not add any questions. Try later\n\n";
                    break;
                }
            }
            break;
        }
        case 2:
        {
            int code = LEADERBOARD_CODE;
            send(client_socket, &code, sizeof(code), 0);
            char dept[10];
            strcpy(dept, this->department.c_str());
            send(client_socket, &dept, sizeof(dept), 0);
            while (1)
            {
                recv(client_socket, &code, sizeof(code), 0);
                if (code == LEADERBOARD_CODE)
                {
                    leaderboardInfo *leaderboard = new leaderboardInfo;
                    recv(client_socket, leaderboard, sizeof(*leaderboard), 0);
                    cout << "Roll: " << leaderboard->id << "Marks: " << leaderboard->marks << endl;
                }
                else if(code == END_OF_LEADERBOARD_CODE)
                {
                    cout << "\nEnd of Leaderboard\n\n";
                    break;
                }
                else
                {
                    cout<<"\nCould not get leaderboard details. Try again later.\n\n";
                    break;
                }
            }
            break;
        }
        case 3:
        {
            code = END_CONNECTION_CODE;
            send(client_socket, &code, sizeof(code), 0);
            endflag = true;
            break;
        }
        }
        if (endflag)
            break;
    }
    cout << "Connection ended :(" << endl;
    close(client_socket);
    exit(0);
}

void Teacher::user_specific_functions(int client_socket)
{
    while (1)
    {
        bool endmenu = false;
        int code;
        int ch;
        cout << " 1) set exam questions\n 2) see leaderboard \n 3) see question \n 4) exit\n";
        cin >> ch;
        switch (ch)
        {
        case 1:
        {
            code = SET_QUESTION_CODE;
            send(client_socket, &code, sizeof(code), 0);
            sleep(1);
            char dept[10];
            strcpy(dept, this->department.c_str());
            send(client_socket, &dept, sizeof(dept), 0); // to open specific department question bank
            while (1)
            {
                int choice;
                bool endflag = false;
                cout << "Do you want to add Question?\n 1) Yes\n 2) No\n";
                cin >> choice;
                if (choice == 1)
                {
                    code = send(client_socket, &code, sizeof(code), 0);
                    QuestionInfo *question = new QuestionInfo;
                    cin.ignore();
                    cout << set_question_menu;
                    cin.getline(question->que, 2048);
                    cin >> question->opt1;
                    cin >> question->opt2;
                    cin >> question->opt3;
                    cin >> question->opt4;
                    cin >> question->answer;
                    cin >> question->marks;
                    send(client_socket, question, sizeof(*question), 0);
                }
                else
                {
                    code = END_OF_QUESTION_SETTING;
                    send(client_socket, &code, sizeof(code), 0);
                    endflag = true;
                }
                if (endflag)
                    break;
            }
            break;
        }
        case 2:
        {
            int code = LEADERBOARD_CODE;
            send(client_socket, &code, sizeof(code), 0);
            char dept[10];
            cout << "Enter which department leaderboard you want to see (department in abbreviated form)\n";
            cin >> dept;
            send(client_socket, &dept, sizeof(dept), 0);
            while (1)
            {
                recv(client_socket, &code, sizeof(code), 0);
                if (code == LEADERBOARD_CODE)
                {
                    leaderboardInfo *leaderboard = new leaderboardInfo;
                    recv(client_socket, leaderboard, sizeof(*leaderboard), 0);
                    cout << "Roll: " << leaderboard->id << "Marks: " << leaderboard->marks << endl;
                }
                else
                {
                    cout << "\n-----------End of Leaderboard------------\n\n";
                    break;
                }
            }
            break;
        }
        case 3:
        {
            int code = SEE_QUESTION_CODE;
            send(client_socket, &code, sizeof(code), 0);
            char dept[10];
            cout << "Enter which department questions you want to see (department in abbreviated form)\n";
            cin >> dept;
            send(client_socket, &dept, sizeof(dept), 0);
            int i=1;
            while (1)
            {
                recv(client_socket, &code, sizeof(code), 0);
                if (code == SEE_QUESTION_CODE)
                {
                    QuestionInfo *question = new QuestionInfo;
                    recv(client_socket, question, sizeof(*question), 0);
                    cout << "\nQ"<<i<<") "<<question->que << endl;
                    cout << "a: " << question->opt1 << endl;
                    cout << "b: " << question->opt1 << endl;
                    cout << "c: " << question->opt1 << endl;
                    cout << "d: " << question->opt1 << endl;
                    cout << "Correct Option: " << question->answer << endl;
                    cout << "Marks: " << question->marks << endl;
                    i++;
                }
                else
                {
                    cout << "\n---------End of questions------------\n\n";
                    break;
                }
            }
            break;
        }
        case 4:
        {
            endmenu = true;
            break;
        }
        }
        if (endmenu)
        {
            code = END_CONNECTION_CODE;
            send(client_socket, &code, sizeof(code), 0);
            break;
        }
    }
    close(client_socket);
    exit(1);
}

string parseDepartment(string id)
{
    vector<string> department;
    department.push_back("CS");
    department.push_back("EE");
    department.push_back("ECE");
    department.push_back("ME");

    for (auto it : department)
    {
        size_t index = id.find(it);
        if (index != string::npos)
            return it;
    }
    return "";
}