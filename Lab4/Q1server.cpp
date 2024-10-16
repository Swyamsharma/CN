#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <thread>
#include <poll.h>

#define BUFFER_SIZE 8
#define TIMEOUT 2 // seconds

struct Frame
{
    int seq_no;
    char data[BUFFER_SIZE];
};

void send_frame(int socket, Frame &frame)
{
    send(socket, &frame, sizeof(frame), 0);
    std::cout << "Sent frame with sequence number: " << frame.seq_no << std::endl;
}

bool receive_ack(int socket, int expected_ack, char *buffer)
{
    int ack;
    int valread = read(socket, &ack, sizeof(ack));
    if (valread > 0 && ack == expected_ack)
    {
        std::cout << "Received ACK for sequence number: " << ack << std::endl;
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the network address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Accept a connection from a client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Open the file to be sent
    std::ifstream file("data.txt", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file" << std::endl;
        close(new_socket);
        close(server_fd);
        return -1;
    }

    int seq_no = 0;
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0)
    {
        Frame frame;
        frame.seq_no = seq_no;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::memcpy(frame.data, buffer, file.gcount());
        std::cout << "Read " << file.gcount() << " bytes from file" << "Data: " << frame.data << std::endl; 
        // frame.data[file.gcount()] = '\0';
        bool ack_received = false;
        pollfd fds[1];
        fds[0].fd = new_socket;
        fds[0].events = POLLIN; // Check for read events

        int retval = poll(fds, 1, 0);
        while (!ack_received)
        {
            send_frame(new_socket, frame);
            auto start = std::chrono::steady_clock::now();
            while ((std::chrono::steady_clock::now() - start) < std::chrono::seconds(TIMEOUT))
            {
                int retval = poll(fds, 1, 0);
                if (retval==0)
                {
                    continue;
                }
                receive_ack(new_socket, seq_no, buffer);
                ack_received = true;
                break;
            }
            if (!ack_received)
            {
                std::cout << "Timeout, resending frame with sequence number: " << seq_no << std::endl;
            }
        }
        seq_no++;
    }

    file.close();
    close(new_socket);
    close(server_fd);
    return 0;
}