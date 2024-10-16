#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port> <node_id>" << std::endl;
        return 1;
    }

    const char *server_ip = argv[1];
    int port = std::stoi(argv[2]);
    int node_id = std::stoi(argv[3]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    std::thread([sock, node_id]()
                {
        char buffer[1024] = {0};
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(sock, buffer, 1024);
            if (valread <= 0) {
                std::cerr << "Read failed or server disconnected" << std::endl;
                break;
            }

            std::string message(buffer);
            int dest_id = std::stoi(message.substr(0, message.find(' ')));
            std::string actual_message = message.substr(message.find(' ') + 1);

            if (dest_id == node_id) {
                std::cout << "Accepted message: " << actual_message << std::endl;
            } else {
                std::cout << "Discarded message: " << actual_message << std::endl;
            }
        }
        close(sock); })
        .detach();

    while (true)
    {
        std::string message;
        std::cout << "Please enter the destination node and message (format: <dest_id> <message>): ";
        std::getline(std::cin, message);

        if (send(sock, message.c_str(), message.length(), 0) < 0)
        {
            std::cerr << "Send failed" << std::endl;
            break;
        }
    }

    close(sock);
    return 0;
}