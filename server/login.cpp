#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

const std::string USER_DATA_FILE = "user_data.txt";

struct User {
    std::string username;
    std::string password; // Hashed password
    bool isAdmin;
};

// Function to generate SHA-256 hash
std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

// Function to register a new user
void registerUser() {
    User newUser;
    std::cout << "Enter username: ";
    std::cin >> newUser.username;

    std::cout << "Enter password: ";
    std::string password;
    std::cin >> password;
    newUser.password = sha256(password); // Hash the password

    std::cout << "Register as admin? (y/n): ";
    char isAdminOption;
    std::cin >> isAdminOption;
    newUser.isAdmin = (isAdminOption == 'y' || isAdminOption == 'Y');

    std::ofstream file(USER_DATA_FILE, std::ios::app);
    if (file.is_open()) {
        file << newUser.username << " " << newUser.password << " " << newUser.isAdmin << std::endl;
        std::cout << "Registration successful!\n";
    } else {
        std::cout << "Error opening file.\n";
    }
}

// Function to authenticate a user
bool authenticateUser() {
    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    std::string hashedPassword = sha256(password);

    std::ifstream file(USER_DATA_FILE);
    if (file.is_open()) {
        std::string storedUsername, storedPassword;
        bool isAdmin;

        while (file >> storedUsername >> storedPassword >> isAdmin) {
            if (storedUsername == username && storedPassword == hashedPassword) {
                file.close();
                std::cout << "Login successful!\n";
                if (isAdmin) {
                    std::cout << "Admin logged in.\n";
                } else {
                    std::cout << "User logged in.\n";
                }
                return true;
            }
        }

        file.close();
    }

    std::cout << "Invalid credentials.\n";
    return false;
}

int main() {
    int choice;

    do {
        std::cout << "1. Register\n2. Login\n3. Quit\nEnter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                registerUser();
                break;

            case 2:
                authenticateUser();
                break;

            case 3:
                std::cout << "Exiting...\n";
                break;

            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);

    return 0;
}