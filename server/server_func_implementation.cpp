#include "../Templates/template.h"
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#define PORT 8080
using namespace std;


void server_side_student_registration(int newSocket)
{
    string uname, password, rollno,department;
    StudentUserInfo *newUserInfo = new StudentUserInfo;
    
    recv(newSocket, newUserInfo, sizeof(*newUserInfo), 0);
    cout<<"Recieved.. ";
    uname = newUserInfo->username;
    password = newUserInfo->password;
    rollno = newUserInfo->rollno;
    department = newUserInfo->department;
    cout<<"Received info..\n";
    fstream file;
    file.open("student_database.txt", ios::app);
    file<<rollno<<"|"<<uname<<"|"<<password<<"|"<<department<<endl;
    file.close();
    return ;
}

void server_side_teacher_registration(int newSocket)
{
    string uname,password,teacher_id,department;
    TeacherUserInfo *newUserInfo = new TeacherUserInfo;
    recv(newSocket,newUserInfo,sizeof(* newUserInfo),0);

    uname = newUserInfo->username;
    password = newUserInfo->password;
    department = newUserInfo->department;
    teacher_id = newUserInfo->teacherid;

    fstream file;
    file.open("teacher_database.txt",ios::app);
    file<<teacher_id<<"|"<<uname<<"|"<<password<<"|"<<department;
    file.close();
    return ;
}