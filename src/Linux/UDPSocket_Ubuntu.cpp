#include "UDPSocket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>

UDPSocket::UDPSocket(MessageReceivedCallback callback)
    : callback(callback)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    listener = std::thread(&UDPSocket::listenForMessages, this);
    listener.detach();
}

UDPSocket::~UDPSocket()
{
    close(sockfd);
}

void UDPSocket::sendMessage(const std::string& ip, const std::string& port, const std::string& message)
{
    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(std::atoi(port.c_str())); // Convert port string to integer
    destAddr.sin_addr.s_addr = inet_addr(ip.c_str()); // Convert IP string to network format

    const char* cstr = message.c_str();
    size_t messageLength = strlen(cstr);

    if (sendto(sockfd, cstr, messageLength, 0, (const struct sockaddr*)&destAddr, sizeof(destAddr)) == -1)
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
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, (socklen_t *)&len);
        buffer[n] = '\0';
        callback(buffer, n);
    }
}

uint32_t UDPSocket::ntohl_wrapper(uint32_t net32)
{
    return ntohl(net32);
}
