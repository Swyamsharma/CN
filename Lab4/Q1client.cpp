#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 8

struct Frame
{
    int seq_no;
    char data[BUFFER_SIZE] = {0};
};

void send_ack(int socket, int ack_no)
{
    send(socket, &ack_no, sizeof(ack_no), 0);
    std::cout << "Sent ACK for sequence number: " << ack_no << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    // Open the file to write received data
    std::ofstream file("received_data.txt", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file" << std::endl;
        close(sock);
        return -1;
    }

    int expected_seq_no = 0;
    bool skip_ack = true; // Simulate lost ACK for the first frame

    while (true)
    {
        Frame frame;
        memset(&frame.data, 0, sizeof(frame.data));
        int valread = read(sock, &frame, sizeof(frame));
        if (valread <= 0)
        {
            break;
        }
        std::cout << "Reading frame: " << frame.data << std::endl;

        std::cout << "Received frame with sequence number: " << frame.seq_no << std::endl;
        if (frame.seq_no == expected_seq_no)
        {
            // Calculate the size of the data to write based on the actual bytes read
            int data_size = 0; // Assuming frame.seq_no is an int
            int i = 0;
            while (i < valread && frame.data[i] != '.')
            {
                i++;
            }
            data_size = i;
            if (frame.data[i] == '.')
            {
                data_size = ++i;
            }
            // i now holds the actual data length excluding the terminator
            std::string data_to_write(frame.data, data_size);
            std::cout << "Writing data to file: " << data_to_write << std::endl;
            file.write(data_to_write.c_str(), data_size);
            if (skip_ack)
            {
                std::cout << "Simulating lost ACK for sequence number: " << frame.seq_no << std::endl;
                skip_ack = false;
            }
            else
            {
                send_ack(sock, expected_seq_no);
            }
            expected_seq_no++;
        }
        else
        {
            send_ack(sock, expected_seq_no - 1);
        }
    }

    file.close();
    close(sock);

    // Compare the received file with the original file
    std::ifstream original("data.txt", std::ios::binary);
    std::ifstream received("received_data.txt", std::ios::binary);
    if (original && received)
    {
        std::string original_content((std::istreambuf_iterator<char>(original)), std::istreambuf_iterator<char>());
        std::cout << "Original file content: " << original_content << std::endl;
        std::string received_content((std::istreambuf_iterator<char>(received)), std::istreambuf_iterator<char>());
        std::cout << "Received file content: " << received_content << std::endl;
    }
    else
    {
        std::cerr << "Failed to open files for comparison" << std::endl;
    }

    return 0;
}