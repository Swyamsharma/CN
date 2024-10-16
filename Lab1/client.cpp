#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port>" << std::endl;
        return 1;
    }

    const char *server_ip = argv[1];
    int port = std::stoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    while (true) {
        std::string message;
        std::cout << "Please enter the message to the server: ";
        std::getline(std::cin, message);

        if (send(sock, message.c_str(), message.length(), 0) < 0) {
            std::cerr << "Send failed" << std::endl;
            break;
        }

        char buffer[1024] = {0};
        int valread = read(sock, buffer, 1024);
        if (valread < 0) {
            std::cerr << "Read failed" << std::endl;
            break;
        }

        std::cout << "Server reply: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}