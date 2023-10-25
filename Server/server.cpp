#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "server.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerWriter;

using server::Message;
using server::MessageRequest;
using server::MessageReply;
using server::MessageService;

using namespace std;

// Global variables
mutex mtx;
vector<string> messages;
vector<string> users;
vector<string> passwords;
vector<string> files;
vector<string> fileNames;
vector<string> fileOwners;
vector<string> filePermissions;
vector<string> fileContents;
vector<string> fileReaders;
vector<string> fileWriters;
vector<string> fileExecutors;
vector<string> fileReadersNames;
vector<string> fileWritersNames;
vector<string> fileExecutorsNames;
vector<string> fileReadersPermissions;
vector<string> fileWritersPermissions;
vector<string> fileExecutorsPermissions;

// Class definition
class MessageServiceImpl final : public MessageService::Service {
    Status Login(ServerContext* context, const MessageRequest* request, MessageReply* reply) override {
        mtx.lock();
        string username = request->username();
        string password = request->password();
        bool found = false;
        for (int i = 0; i < users.size(); i++) {
            if (users[i] == username) {
                found = true;
                if (passwords[i] == password) {
                    reply->set_message("Login successful!");
                } else {
                    reply->set_message("Incorrect password!");
                }
                break;
            }
        }
        if (!found) {
            reply->set_message("Username not found!");
        }
        mtx.unlock();
        return Status::OK;
    }

    Status Register(ServerContext* context, const MessageRequest* request, MessageReply* reply) override {
        mtx.lock();
        string username = request->username();
        string password = request->password();
        bool found = false;
        for (int i = 0; i < users.size(); i++) {
            if (users[i] == username) {
                found = true;
                reply->set_message("Username already exists!");
                break;
            }
        }
        if (!found) {
            users.push_back(username);
            passwords.push_back(password);
            reply->set_message("Registration successful!");
        }
        mtx.unlock();
        return Status::OK;
    }

    Status SendMessage(ServerContext* context, const MessageRequest* request, MessageReply* reply) override {
        mtx.lock();
        string username = request->username();
        string message = request->message();
        messages.push_back(username + ": " + message);
        reply->set_message("Message sent!");
        mtx.unlock();
        return Status::OK;
    }

    Status ReceiveMessage(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < messages.size(); i++) {
            MessageReply reply;
            reply.set_message(messages[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status SendFile(ServerContext* context, const MessageRequest* request, MessageReply* reply) override {
        mtx.lock();
        string username = request->username();
        string fileName = request->filename();
        string fileOwner = request->fileowner();
        string filePermission = request->filepermission();
        string fileContent = request->filecontent();
        string fileReader = request->filereader();
        string fileWriter = request->filewriter();
        string fileExecutor = request->fileexecutor();
        files.push_back(fileContent);
        fileNames.push_back(fileName);
        fileOwners.push_back(fileOwner);
        filePermissions.push_back(filePermission);
        fileContents.push_back(fileContent);
        fileReaders.push_back(fileReader);
        fileWriters.push_back(fileWriter);
        fileExecutors.push_back(fileExecutor);
        fileReadersNames.push_back("");
        fileWritersNames.push_back("");
        fileExecutorsNames.push_back("");

        if (fileReader != "") {
            fileReadersNames[fileReadersNames.size() - 1] = fileReader;
            fileReadersPermissions.push_back(filePermission);
        }

        if (fileWriter != "") {
            fileWritersNames[fileWritersNames.size() - 1] = fileWriter;
            fileWritersPermissions.push_back(filePermission);
        }

        if (fileExecutor != "") {
            fileExecutorsNames[fileExecutorsNames.size() - 1] = fileExecutor;
            fileExecutorsPermissions.push_back(filePermission);
        }

        reply->set_message("File sent!");
        mtx.unlock();
        return Status::OK;
    }


    Status ReceiveFile(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < files.size(); i++) {
            MessageReply reply;
            reply.set_message(files[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ReadFile(ServerContext* context, const MessageRequest* request, MessageReply* reply) override {
        mtx.lock();
        string username = request->username();
        string fileName = request->filename();
        string fileOwner = request->fileowner();
        string filePermission = request->filepermission();
        string fileContent = request->filecontent();
        string fileReader = request->filereader();
        string fileWriter = request->filewriter();
        string fileExecutor = request->fileexecutor();
        bool found = false;
        for (int i = 0; i < fileNames.size(); i++) {
            if (fileNames[i] == fileName) {
                found = true;
                if (fileReaders[i] == username) {
                    reply->set_message(fileContents[i]);
                } else {
                    reply->set_message("You do not have permission to read this file!");
                }
                break;
            }
        }
        if (!found) {
            reply->set_message("File not found!");
        }
        mtx.unlock();
        return Status::OK;
    }

    Status WriteFile(ServerContext* context, const MessageRequest* request, MessageReply* reply) override {
        mtx.lock();
        string username = request->username();
        string fileName = request->filename();
        string fileOwner = request->fileowner();
        string filePermission = request->filepermission();
        string fileContent = request->filecontent();
        string fileReader = request->filereader();
        string fileWriter = request->filewriter();
        string fileExecutor = request->fileexecutor();
        bool found = false;
        for (int i = 0; i < fileNames.size(); i++) {
            if (fileNames[i] == fileName) {
                found = true;
                if (fileWriters[i] == username) {
                    fileContents[i] = fileContent;
                    reply->set_message("File updated!");
                } else {
                    reply->set_message("You do not have permission to write to this file!");
                }
                break;
            }
        }
        if (!found) {
            reply->set_message("File not found!");
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ExecuteFile(ServerContext* context, const MessageRequest* request, MessageReply* reply) override {
        mtx.lock();
        string username = request->username();
        string fileName = request->filename();
        string fileOwner = request->fileowner();
        string filePermission = request->filepermission();
        string fileContent = request->filecontent();
        string fileReader = request->filereader();
        string fileWriter = request->filewriter();
        string fileExecutor = request->fileexecutor();
        bool found = false;
        for (int i = 0; i < fileNames.size(); i++) {
            if (fileNames[i] == fileName) {
                found = true;
                if (fileExecutors[i] == username) {
                    reply->set_message("File executed!");
                } else {
                    reply->set_message("You do not have permission to execute this file!");
                }
                break;
            }
        }
        if (!found) {
            reply->set_message("File not found!");
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ListFiles(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < fileNames.size(); i++) {
            MessageReply reply;
            reply.set_message(fileNames[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ListUsers(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < users.size(); i++) {
            MessageReply reply;
            reply.set_message(users[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ListPermissions(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < fileNames.size(); i++) {
            MessageReply reply;
            reply.set_message(filePermissions[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ListReaders(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < fileNames.size(); i++) {
            MessageReply reply;
            reply.set_message(fileReadersNames[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ListWriters(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < fileNames.size(); i++) {
            MessageReply reply;
            reply.set_message(fileWritersNames[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ListExecutors(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < fileNames.size(); i++) {
            MessageReply reply;
            reply.set_message(fileExecutorsNames[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }

    Status ListFilePermissions(ServerContext* context, const MessageRequest* request, ServerWriter<MessageReply>* writer) override {
        mtx.lock();
        string username = request->username();
        for (int i = 0; i < fileNames.size(); i++) {
            MessageReply reply;
            reply.set_message(filePermissions[i]);
            writer->Write(reply);
        }
        mtx.unlock();
        return Status::OK;
    }


