#include "UDPSocket.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

UDPSocket::UDPSocket(MessageReceivedCallback callback)
    : callback(callback)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET)
    {
        perror("socket creation failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    listener = std::thread(&UDPSocket::listenForMessages, this);
    listener.detach();
}

UDPSocket::~UDPSocket()
{
    closesocket(sockfd);
    WSACleanup();
}

void UDPSocket::sendMessage(const std::string& ip, const std::string& port, const std::string& message)
{
    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(std::atoi(port.c_str())); // Convert port string to integer
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr); // Convert IP string to network format

    const char* cstr = message.c_str();
    size_t messageLength = strlen(cstr);

    if (sendto(sockfd, cstr, messageLength, 0, (const struct sockaddr*)&destAddr, sizeof(destAddr)) == SOCKET_ERROR)
    {
        perror("message send failed");
        // Handle the error gracefully instead of exiting
        // throw std::runtime_error("Failed to send message");
        // or
        // log_error("Failed to send message");
    }
}

void UDPSocket::listenForMessages()
{
    int len, n;
    char buffer[MAXLINE];
    len = sizeof(cliaddr);

    while (true)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0)
        {
            continue;
        }
        
        buffer[n] = '\0';
        callback(buffer, n);
    }
}

uint32_t UDPSocket::ntohl_wrapper(uint32_t net32)
{
    return ntohl(net32);
}
