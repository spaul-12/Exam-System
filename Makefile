# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall

# The build target
TARGET = server login client

all: $(TARGET)

server: server.cpp
    $ echo "Building server..";
    $ $(CC) $(CFLAGS) -o server ./server/server.cpp

login: login.cpp
    $ $(CC) $(CFLAGS) -o login ./server/login.cpp

clean:
    $ echo "Clean build files..";
    $ $(RM) $(TARGET)

client: client.cpp
    $ echo "Building client..";
    $ $(CC) $(CFLAGS) -o client ./client/client.cpp