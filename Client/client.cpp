#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
            : stub_(helloworld::Greeter::NewStub(channel)) {}

    std::string SayHello(const std::string &user) {
        HelloRequest request;
        request.set_name(user);

        HelloReply reply;

        ClientContext context;

        Status status = stub_->SayHello(&context, request, &reply);

        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

private:

    std::unique_ptr<helloworld::Greeter::Stub> stub_;
};
