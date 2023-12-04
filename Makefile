# the compiler: gcc for C program, define as g++ for C++
CC=g++
CFLAGS=-g -Wall -I Templates
BUILD=build

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings

# set tab size to 8 spaces

# The build target
TARGET=always server client

all: $(TARGET)

server: always
	@echo "Building server..."
	$(CC) $(CFLAGS) -o $(BUILD)/server ./server/server_func_implementation.cpp ./server/server.cpp

client: always
	@echo "Building client..."
	$(CC) $(CFLAGS) -o $(BUILD)/client ./client/client_implementation.cpp ./client/client.cpp

always:
	mkdir -p $(BUILD)

clean:
	@echo "Clean build files.."
	rm -rf $(BUILD)/*