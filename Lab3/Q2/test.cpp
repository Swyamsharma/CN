#include <iostream>

void print_message(const char* message) {
    std::cout << message << std::endl;
}

int main() {
    const char* greeting = "Hello, World!";
    print_message(greeting);

    // Uncommenting the next line would cause a compilation error
    // greeting[0] = 'h'; // Error: assignment of read-only location

    return 0;
}