#include "UDPSocket.h"
#include <netinet/in.h> // for sockaddr_in and htons
#include <arpa/inet.h>  // for inet_addr

UDPSocket::UDPSocket(MessageReceivedCallback callback)
    : callback(callback)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    listener = std::thread(&UDPSocket::listenForMessages, this);
    listener.detach();
}

UDPSocket::~UDPSocket()
{
    close(sockfd);
}

void UDPSocket::sendMessage(const std::string& message)
{
    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(12000); // Set port number, in network byte order
    destAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set loopback IP address

    const char* cstr = message.c_str();
    int len = sizeof(struct sockaddr_in); // Corrected length of destination address structure

    if (sendto(sockfd, cstr, strlen(cstr), 0, (const struct sockaddr*)&destAddr, len) == -1)
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