#include<bits/stdc++.h>
using namespace std;

struct userinfo
{
    string username;
    string password;
    string id;
    string usertype;
};

class User{
    string username;
    string password;
    string usertype;
    public:
    User(string username,string password,string usertype)
    {
        this->username = username;
        this->password = password;
        this->usertype = usertype;
    }
};

class Client{
    int client_socket;
    User *client;
    struct sockaddr_in server_address;
    public:
    Client();
    void registeruser(string &usertype);
    User* login(string &);
};