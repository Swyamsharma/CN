#include <iostream>
#include <chrono>
#include <thread>

void perform_action_with_timeout(int timeout_seconds) {
    auto start = std::chrono::steady_clock::now();
    bool action_completed = false;

    while (!action_completed) {
        // Simulate an action that might complete before the timeout
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate work
        action_completed = false; // Set to true to simulate action completion

        // Check if timeout has occurred
        if (std::chrono::steady_clock::now() - start >= std::chrono::seconds(timeout_seconds)) {
            std::cout << "Timeout occurred!" << std::endl;
            break;
        }
    }

    if (action_completed) {
        std::cout << "Action completed before timeout." << std::endl;
    }
}

int main() {
    int timeout_seconds = 5;
    perform_action_with_timeout(timeout_seconds);
    return 0;
}