#include<iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <map>
#include <sys/socket.h>
#include <algorithm>
using namespace std;

#define student_register_menu "Enter the following\n 1) username \n 2) password \n 3) roll number \n 4) department (abbreviated)\n" 
#define teacher_register_menu "Enter the following\n 1) username \n 2) password \n 3) teacher_id \n 4) department (abbreviated)\n"
#define login_menu "Enter the following\n 1) Id \n 2) password\n"
#define set_question_menu "Enter the following\n 1) Question\n 2) Option 1\n 3) Option 2\n 4) Option 3\n 5) Option 4\n 6) Correct Answer (a\\b\\c\\d)\n 7) Marks\n"
#define REGISTRATION_CODE 121
#define LOGIN_CODE 122
#define LOGIN_FAIL_CODE 123
#define START_EXAM_CODE 124
#define RECIEVE_QUESTION_CODE 125
#define END_EXAM_CODE 126
#define SET_QUESTION_CODE 127
#define END_OF_QUESTION_SETTING 128
#define LEADERBOARD_CODE 129
#define END_OF_LEADERBOARD_CODE 130
#define SEE_QUESTION_CODE 131
#define END_QUESTION_SEEING_CODE 132
#define EMPTY_QUESTIONBANK_CODE 133
#define END_CONNECTION_CODE 400
#define SUCCESSFUL_CODE  200
#define SERVER_ERROR_CODE 500
// Semaphore names
#define SEMAPHORE_NAME1 "/sem-mutex1"
#define SEMAPHORE_NAME2 "/sem-mutex2"
#define SEMAPHORE_NAME3 "/sem-mutex3"
#define SEMAPHORE_NAME4 "/sem-mutex4"


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

struct QuestionInfo
{
    char que[2048];
    char opt1[100];
    char opt2[100];
    char opt3[100];
    char opt4[100];
    char answer[5];
    char marks[10];
};

struct StudentQuestion
{
    char que[2048];
    char opt1[100];
    char opt2[100];
    char opt3[100];
    char opt4[100];
    char marks[10];
};

struct leaderboardInfo
{
    char id[100];
    char marks[10];
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
    Student(string ,string, string);
    string getRollNumber();
    void user_specific_functions(int client_socket);
};

class Teacher:public User{
    private:
    string department;
    string teacherid;

    public:
    Teacher(string, string, string, string, string);
    Teacher(string, string,string);
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

class Question
{
    vector<QuestionInfo*> questionBank;
    public:
    Question();
    void insertQuestion(QuestionInfo *);
    int startExam(int );
    void sendQuestions(int newSocket);
};

// generic functions
void server_side_student_registration(int newSocket);
void server_side_teacher_registration(int newSocket);
void server_side_login(int newSocket);
string parseDepartment(string id);
void setQuestion(int newSocket, string department, Question&);
void updateResult(string id,string dept,int marksObtained);
void getLeaderboard(int newSocket, string dept);
void addQuestionFromFile(string dept,Question&);
