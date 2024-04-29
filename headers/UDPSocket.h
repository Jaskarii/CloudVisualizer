#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>

#define PORT 8080
#define MAXLINE 32000

// Define the callback type
typedef void (*MessageReceivedCallback)(const char*, size_t size);

class UDPSocket
{
public:
    UDPSocket(MessageReceivedCallback callback);
    ~UDPSocket();
    void sendMessage(const std::string& message);

private:
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    std::thread listener;
    MessageReceivedCallback callback;

    void listenForMessages();
};