#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <arpa/inet.h>

struct Route {
    std::string cidr; // CIDR notation
    std::string line; // Line to use
};

// Function to convert an IP address from string to binary format
std::string ipToBinary(const std::string& ip) {
    struct in_addr addr;
    inet_pton(AF_INET, ip.c_str(), &addr);
    return std::bitset<32>(ntohl(addr.s_addr)).to_string();
}

// Function to get the prefix length from CIDR notation
int getPrefixLength(const std::string& cidr) {
    size_t slashPos = cidr.find('/');
    return std::stoi(cidr.substr(slashPos + 1));
}

// Function to convert hexadecimal IP to decimal format
std::string hexToDecimalIP(const std::string& hexIP) {
    std::stringstream ss;
    for (size_t i = 0; i < hexIP.length(); i += 2) {
        std::string byte = hexIP.substr(i, 2);
        int decimal = std::stoi(byte, nullptr, 16);
        ss << decimal;
        if (i < hexIP.length() - 2) {
            ss << ".";
        }
    }
    return ss.str();
}

// Function to check if an IP address matches a CIDR
bool matchesCIDR(const std::string& ip, const std::string& cidr) {
    std::string ipBinary = ipToBinary(ip);
    std::string cidrIp = cidr.substr(0, cidr.find('/'));
    std::string cidrBinary = ipToBinary(hexToDecimalIP(cidrIp)); // Convert CIDR IP from hex to decimal
    int prefixLength = getPrefixLength(cidr);
    
    return ipBinary.substr(0, prefixLength) == cidrBinary.substr(0, prefixLength);
}

// Function to perform IP forwarding
std::string forwardIP(const std::string& ip, const std::vector<Route>& routingTable) {
    std::string bestLine;
    int maxPrefixLength = -1;

    for (const auto& route : routingTable) {
        if (matchesCIDR(ip, route.cidr)) {
            int prefixLength = getPrefixLength(route.cidr);
            if (prefixLength > maxPrefixLength) {
                maxPrefixLength = prefixLength;
                bestLine = route.line;
            }
        }
    }

    return bestLine;
}

int main() {
    // Example routing table
    std::vector<Route> routingTable = {
        {"C4.5E.2.0/23", "A"},
        {"C4.5E.4.0/22", "B"},
        {"C4.5E.C0.0/19", "C"},
        {"C4.5E.40.0/18", "D"},
        {"C4.4C.0.0/14", "E"},
        {"C0.0.0.0/2", "F"},
        {"80.0.0.0/1", "G"}
    };

    // Test IP addresses in hexadecimal format
    std::vector<std::string> testIPs = {
        "C4.4B.00.00",
        "C4.5E.05.09",
        "C4.4D.31.2E",
        "C4.5E.03.87",
        "C4.5E.7F.12",
        "C4.5E.D1.02"
    };

    // Store unmatched IPs
    std::vector<std::string> unmatchedIPs;

    for (const auto& hexIP : testIPs) {
        std::string decimalIP = hexToDecimalIP(hexIP);
        std::string nextHop = forwardIP(decimalIP, routingTable);
        if (!nextHop.empty()) {
            std::cout << "Forwarding IP " << hexIP << " to line: " << nextHop << std::endl;
        } else {
            unmatchedIPs.push_back(hexIP);
        }
    }

    // Display unmatched IPs
    if (!unmatchedIPs.empty()) {
        std::cout << "Unmatched IPs: ";
        for (const auto& ip : unmatchedIPs) {
            std::cout << ip << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
