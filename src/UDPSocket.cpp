#include "UDPSocket.h"

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