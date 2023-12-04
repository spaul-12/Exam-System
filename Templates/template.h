#include<bits/stdc++.h>
#include<sys/socket.h>
using namespace std;

#define student_register_menu "Enter the following\n 1) username \n 2) password \n 3) roll number \n 4) department\n" 
#define teacher_register_menu "Enter the following\n 1) username \n 2) password \n 3) teacher_id \n 4) department\n"
#define login_menu "Enter the following\n 1) Id \n 2) password\n"
#define REGISTRATION_CODE 121
#define LOGIN_CODE 122
#define LOGIN_FAIL_CODE 123
#define START_EXAM_CODE 124
#define END_CONNECTION_CODE 125
#define SUCCESSFUL_CODE  200
// Semaphore names
#define SEMAPHORE_NAME1 "/sem-mutex1"
#define SEMAPHORE_NAME2 "/sem-mutex2"
#define SEMAPHORE_NAME3 "/sem-mutex3"

struct StudentUserInfo
{
    char username[100];
    char password[100];
    char rollno[100];
    char department[100];
};

struct TeacherUserInfo
{
    char username[100];
    char password[100];
    char teacherid[100];
    char department[100];
};

struct ClientUserInfo
{
    string username;
    string password;
    string usertype;
};

struct loginInfo
{
    char id[100];
    char password[100];
};

class User{
    string username;
    string password;
    string usertype;

    public:
    User(string username,string password,string usertype);
    User(string password);
    virtual void user_specific_functions(int);
    string getUsername();
};

class Student:public User{
    private:
    string rollno="null";
    string department="null";

    public:
    Student(string ,string,string, string, string);
    Student(string ,string);
    string getRollNumber();
    void user_specific_functions(int client_socket);
};

class Teacher:public User{
    private:
    string department;
    string teacherid;

    public:
    Teacher(string, string, string, string, string);
    Teacher(string, string);
    string getId();
    void user_specific_functions(int client_socket);
};

class Client{
    int client_socket;
    User *client;

    public:
    Client();
    void registeruser(char &);
    void login(char &);
};

// generic functions
void server_side_student_registration(int newSocket);
void server_side_teacher_registration(int newSocket);
void server_side_login(int newSocket);