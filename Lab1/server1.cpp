#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(server_fd, 1) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true)
    {
        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0)
        {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "Connected with client socket number " << client_socket << std::endl;
        while (true)
        {
            char buffer[1024] = {0};
            int valread = read(client_socket, buffer, 1024);
            if (valread < 0)
            {
                std::cerr << "Read failed" << std::endl;
                close(client_socket);
                continue;
            }

            std::string message(buffer);
            std::cout << "Client socket " << client_socket << " sent message: " << message << std::endl;

            std::string reversed_message(message.rbegin(), message.rend());
            std::cout << "Sending reply: " << reversed_message << std::endl;

            send(client_socket, reversed_message.c_str(), reversed_message.length(), 0);
        }

        close(client_socket);
    }
    close(server_fd);
    return 0;
}