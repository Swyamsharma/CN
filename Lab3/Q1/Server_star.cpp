#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

std::vector<int> client_sockets;

void handle_client(int client_socket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_socket, buffer, sizeof(buffer) - 1);
        if (valread <= 0)
        {
            std::cerr << "Read failed or client disconnected" << std::endl;
            close(client_socket);
            break;
        }

        std::string message(buffer);
        std::cout << "Received message: " << message << std::endl;

        {
            for (int sock : client_sockets)
            {
                if (sock != client_socket)
                {
                    send(sock, message.c_str(), message.length(), 0);
                }
            }
        }
    }
}

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

    if (listen(server_fd, 5) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true)
    {
        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_socket < 0)
        {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        {
            client_sockets.push_back(client_socket);
        }
        std::thread(handle_client, client_socket).detach();
    }

    close(server_fd);
    return 0;
}