#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// Structure to represent a station
struct Station {
    bool isTransmitting;
    bool hasFrame;
    int backoffTime;
};

// Function to simulate the CSMA/CD protocol
void simulateCSMA_CD(int N, int totalSlots) {
    // Initialize stations
    std::vector<Station> stations(N);
    for (int i = 0; i < N; i++) {
        stations[i].isTransmitting = false;
        stations[i].hasFrame = true; // Assume all stations have a frame to transmit
        stations[i].backoffTime = 0;
    }

    // Initialize current time and slot
    int currentTime = 0;

    // Simulate the CSMA/CD protocol
    while (currentTime < totalSlots) {
        // Check if any station is transmitting
        bool isAnyTransmitting = false;
        for (int i = 0; i < N; i++) {
            if (stations[i].isTransmitting) {
                isAnyTransmitting = true;
                break;
            }
        }

        // If no station is transmitting, check if any station wants to transmit
        if (!isAnyTransmitting) {
            int numStationsTryingToTransmit = 0;
            for (int i = 0; i < N; i++) {
                if (stations[i].hasFrame && stations[i].backoffTime == 0) {
                    numStationsTryingToTransmit++;
                }
            }

            if (numStationsTryingToTransmit > 1) {
                // Collision detected, send jamming signal and backoff
                std::cout << "Slot " << currentTime << ": Collision detected. Stations trying to transmit: ";
                for (int i = 0; i < N; i++) {
                    if (stations[i].hasFrame && stations[i].backoffTime == 0) {
                        std::cout << i << " ";
                    }
                }
                std::cout << std::endl;
                for (int i = 0; i < N; i++) {
                    if (stations[i].hasFrame && stations[i].backoffTime == 0) {
                        stations[i].backoffTime = rand() % 16 + 1; // Random backoff time between 1 and 16 slots
                    }
                }
            } else if (numStationsTryingToTransmit == 1) {
                // No collision, start transmitting
                int transmittingStation = -1;
                for (int i = 0; i < N; i++) {
                    if (stations[i].hasFrame && stations[i].backoffTime == 0) {
                        transmittingStation = i;
                        break;
                    }
                }
                std::cout << "Slot " << currentTime << ": Station " << transmittingStation << " starts transmitting" << std::endl;
                stations[transmittingStation].isTransmitting = true;
                stations[transmittingStation].hasFrame = false; // Frame transmitted, no more frames to transmit
            } else {
                std::cout << "Slot " << currentTime << ": No stations trying to transmit" << std::endl;
            }
        } else {
            // A station is transmitting, check if it's done
            for (int i = 0; i < N; i++) {
                if (stations[i].isTransmitting) {
                    std::cout << "Slot " << currentTime << ": Station " << i << " is still transmitting" << std::endl;
                    stations[i].isTransmitting = false;
                    break;
                }
            }
        }

        // Update backoff times
        for (int i = 0; i < N; i++) {
            if (stations[i].backoffTime > 0) {
                stations[i].backoffTime--;
            }
        }

        // Update current time
        currentTime++;
    }
}

int main() {
    srand(time(0)); 

    int N;
    std::cout << "Enter the number of stations: ";
    std::cin >> N;

    int totalSlots;
    std::cout << "Enter the total number of time slots to simulate: ";
    std::cin >> totalSlots;

    simulateCSMA_CD(N, totalSlots);

    return 0;
}
