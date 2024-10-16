#include <iostream>
#include <cstring>

using namespace std;

const int MAX_HEADER_SIZE = 1024;

void application_layer(char* message, int size);
void presentation_layer(char* message, int size);
void session_layer(char* message, int size);
void transport_layer(char* message, int size);
void network_layer(char* message, int size);
void data_link_layer(char* message, int size);
void physical_layer(char* message, int size);

void application_layer(char* message, int size) {
    cout << "-->Application Layer: Received message: " << message << endl;
    presentation_layer(message, size);
}

void presentation_layer(char* message, int size) {
    char header[MAX_HEADER_SIZE] = "MIME Type: text/plain\n";
    strcat(header, message);

    cout << "-->Presentation Layer: Message with header: " << header << endl;
    session_layer(header, strlen(header));
}

void session_layer(char* message, int size) {
    char header[MAX_HEADER_SIZE] = "Session ID: 1234567890\n";
    strcat(header, message);

    cout << "-->Session Layer: Message with header: " << header << endl;
    transport_layer(header, strlen(header));
}

void transport_layer(char* message, int size) {
    char header[MAX_HEADER_SIZE] = "Source Port: 1234\nDestination Port: 5678\n";
    strcat(header, message);

    cout << "-->Transport Layer: Message with header: " << header << endl;
    network_layer(header, strlen(header));
}

void network_layer(char* message, int size) {
    char header[MAX_HEADER_SIZE] = "Source Address: 192.168.1.100\nDestination Address: 192.168.1.200\n";
    strcat(header, message);

    cout << "-->Network Layer: Message with header: " << header << endl;
    data_link_layer(header, strlen(header));
}

void data_link_layer(char* message, int size) {
    char header[MAX_HEADER_SIZE] = "Source MAC Address: 00:11:22:33:44:55\nDestination MAC Address: 00:22:33:44:55:66\nFrame Type: 0x0800\n";
    strcat(header, message);

    cout << "-->Data Link Layer: Message with header: " << header << endl;
    physical_layer(header, strlen(header));
}

void physical_layer(char* message, int size) {
    cout << "-->Physical Layer: Sending message: " << message << endl;
}

int main() {
    char message[MAX_HEADER_SIZE] = "Hello from the application layer!";
    application_layer(message, strlen(message));
    return 0;
}