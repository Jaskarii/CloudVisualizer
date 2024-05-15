#pragma once

#include <string>
#include <thread> // Include the thread header

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/in.h>
#endif

#define MAXLINE 4096

// Define the callback type
typedef void (*MessageReceivedCallback)(const char*, size_t size);

class UDPSocket
{
public:
    UDPSocket(MessageReceivedCallback callback);
    ~UDPSocket();
    void sendMessage(const std::string& ip, const std::string& port, const std::string& message);
    static uint32_t ntohl_wrapper(uint32_t net32);


private:
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    std::thread listener;
    MessageReceivedCallback callback;

    void listenForMessages();
};
