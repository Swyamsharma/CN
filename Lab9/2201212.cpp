#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <algorithm>

using namespace std;

struct RouteEntry
{
    std::string cidr;
    std::string line;
};

void cidrToNetworkAndMask(const std::string &cidr, in_addr &network, in_addr &mask)
{
    size_t pos = cidr.find('/');
    std::string ip = cidr.substr(0, pos);
    int prefixLength = std::stoi(cidr.substr(pos + 1));

    std::stringstream ss(ip);
    std::string segment;
    std::string decimalIp;
    while (std::getline(ss, segment, '.'))
    {
        unsigned int decimalSegment;
        std::stringstream converter(segment);
        converter >> std::hex >> decimalSegment;
        decimalIp += std::to_string(decimalSegment) + ".";
    }

    if (!decimalIp.empty())
    {
        decimalIp.pop_back();
    }

    inet_pton(AF_INET, decimalIp.c_str(), &network);
    mask.s_addr = htonl(~((1 << (32 - prefixLength)) - 1));
}

bool isInNetwork(const in_addr &ip, const in_addr &network, const in_addr &mask)
{
    return (ip.s_addr & mask.s_addr) == (network.s_addr & mask.s_addr);
}

std::string forward(const std::string &ipAddress, const std::vector<RouteEntry> &routingTable)
{
    in_addr ip;
    inet_pton(AF_INET, ipAddress.c_str(), &ip);

    std::string bestMatch;
    int bestPrefixLength = -1;

    for (const auto &entry : routingTable)
    {
        in_addr network, mask;
        cidrToNetworkAndMask(entry.cidr, network, mask);

        if (isInNetwork(ip, network, mask))
        {
            int prefixLength = std::stoi(entry.cidr.substr(entry.cidr.find('/') + 1));

            if (prefixLength > bestPrefixLength)
            {
                bestPrefixLength = prefixLength;
                bestMatch = entry.line;
            }
        }
    }

    if (bestMatch.empty())
    {
        return "No match found";
    }
    else
    {
        return bestMatch;
    }
}

string process(const string &ip)
{
    if (ip.find('.') != string::npos)
    {
        std::stringstream ss(ip);
        std::string segment;
        std::string decimalIp;

        while (std::getline(ss, segment, '.'))
        {
            unsigned int decimalSegment;
            std::stringstream converter(segment);
            converter >> std::hex >> decimalSegment;
            decimalIp += std::to_string(decimalSegment) + ".";
        }

        if (!decimalIp.empty())
        {
            decimalIp.pop_back();
        }
        return decimalIp;
    }

    return ip;
}

int main()
{
    std::vector<RouteEntry> routingTable = {
        {"C4.5E.2.0/23", "A"},
        {"C4.5E.4.0/22", "B"},
        {"C4.5E.C0.0/19", "C"},
        {"C4.5E.40.0/18", "D"},
        {"C4.4C.0.0/14", "E"},
        {"C0.0.0.0/2", "F"},
        {"80.0.0.0/1", "G"},
    };

    vector<string> ips = {"C4.4B.31.2E", "C4.5E.05.09", "C4.4D.31.2E", "C4.5E.03.87", "C4.5E.7F.12", "C4.5E.D1.02"};
    for (const auto &ip : ips)
    {
        cout << "Given IP: " << ip << " is processed as: " << process(ip) << endl;
        string result = forward(process(ip), routingTable);
        cout << "The line to use for IP: " << ip << " is: " << result << endl;
    }

    while (1)
    {
        cout << "\nEnter an IP to know its forwarding line and -1 to exit: " << endl;
        string ip;
        cin >> ip;
        if (ip == "-1")
            return 0;

        cout << "Given IP: " << ip << " is processed as: " << process(ip) << endl;
        string result = forward(process(ip), routingTable);
        cout << "The line to use for IP: " << ip << " is: " << result << endl;
    }

    return 0;
}