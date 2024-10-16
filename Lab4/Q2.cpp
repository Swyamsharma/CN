#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <bitset>

const uint8_t FLAG = 0x6C;
const uint8_t ESC = 0x00;

std::vector<std::string> string_to_sequences(const std::string &input, size_t frame_size) {
    std::string binary_string;
    for (char c : input) {
        binary_string += std::bitset<8>(c).to_string();
    }

    std::vector<std::string> sequences;
    for (size_t i = 0; i < binary_string.size(); i += frame_size) {
        sequences.push_back(binary_string.substr(i, frame_size));
    }

    return sequences;
}
std::string sequences_to_string(const std::vector<std::string> &input) {
    std::string binary_string;
    for (const std::string &frame : input) {
        binary_string += frame;
    }

    std::string output;
    for (size_t i = 0; i < binary_string.size(); i += 8) {
        std::bitset<8> bits(binary_string.substr(i, 8));
        output += static_cast<char>(bits.to_ulong());
    }

    return output;
}
std::vector<uint8_t> string_to_uint8_vector(const std::string &input) {
    std::vector<uint8_t> uint8_vector;
    for (char c : input) {
        uint8_vector.push_back(static_cast<uint8_t>(c));
    }
    return uint8_vector;
}
std::vector<std::string> uint8_vector_to_sequences(const std::vector<uint8_t> &input, size_t frame_size) {
    std::string binary_string;
    for (uint8_t byte : input) {
        binary_string += std::bitset<8>(byte).to_string();
    }

    std::vector<std::string> sequences;
    for (size_t i = 0; i < binary_string.size(); i += frame_size) {
        sequences.push_back(binary_string.substr(i, frame_size));
    }

    return sequences;
}
std::vector<uint8_t> byte_stuffing(const std::vector<uint8_t> &data){
    std::vector<uint8_t> stuffed_data;
    stuffed_data.push_back(FLAG);
    for (uint8_t byte : data){
        if (byte == FLAG || byte == ESC){
            stuffed_data.push_back(ESC);
            stuffed_data.push_back(byte);
        }
        else{
            stuffed_data.push_back(byte);
        }
    }
    stuffed_data.push_back(FLAG);
    return stuffed_data;
}

std::vector<uint8_t> byte_unstuffing(const std::vector<uint8_t> &stuffed_data){
    std::vector<uint8_t> unstuffed_data;
    bool escape_next = false;
    for (size_t i = 1; i < stuffed_data.size() - 1; ++i) { 
        uint8_t byte = stuffed_data[i];
        if (escape_next) {
            unstuffed_data.push_back(byte);
            escape_next = false;
        } else if (byte == ESC) {
            escape_next = true;
        } else {
            unstuffed_data.push_back(byte);
        }
    }
    return unstuffed_data;
}
int main()
{
    std::string input = "";
    std::cout << "Enter the input: ";
    std::getline(std::cin, input);
    size_t frame_size = 8; 

    std::vector<std::string> sequences = string_to_sequences(input, frame_size);

    // original data
    std::vector<uint8_t> data = string_to_uint8_vector(input);
    std::cout << "Original Data: ";
    for (uint8_t byte : data) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
    std::cout << "original frame: " << std::endl;
    for (const std::string &sequence : sequences) {
        std::cout << sequence << std::endl;
    }
    // Byte stuffing
    std::vector<uint8_t> stuffed_data = byte_stuffing(data);
    std::cout << "Stuffed Data: ";
    for (uint8_t byte : stuffed_data) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    std::cout << "Stuffed frame: " << std::endl;
    std::vector<std::string> stuffed_sequences = uint8_vector_to_sequences(stuffed_data, frame_size);
    for (const std::string &sequence : stuffed_sequences) {
        std::cout << sequence << std::endl;
    }
    // Byte unstuffing
    std::vector<uint8_t> unstuffed_data = byte_unstuffing(stuffed_data);
    std::cout << "Unstuffed Data: ";
    for (uint8_t byte : unstuffed_data) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
    std::cout << "Unstuffed frame: " << std::endl;
    std::vector<std::string> unstuffed_sequences = uint8_vector_to_sequences(unstuffed_data, frame_size);
    for (const std::string &sequence : unstuffed_sequences) {
        std::cout << sequence << std::endl;
    }
    std::string output = sequences_to_string(sequences);
    std::cout << "Output: " << output << std::endl;
    return 0;
}