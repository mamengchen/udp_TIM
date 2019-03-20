#ifndef __UDP_CLIENT_HPP__
#define __UDP_CLIENT_HPP__
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class Client{
public:
    Client(const std::string _ip, const int _port);
    int InitClient();
    int RecvData(std::string& outString);
    int SendData(std::string& inString);
    ~Client();
private:
    struct sockaddr_in server;
    int sock;
};

Client::Client(const std::string _ip, const int _port)
    :sock(-1)
{
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr = inet_addr(_ip.c_str());
}

int Client::InitClient()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "创建套接字失败" << std::endl;
        return -1;
    }
    return 0;
}

int Client::RecvData(std::string& outString)
{
    char buf[1024] = {0};
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);

    int ret = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&peer, &len);
    if (ret <= 0)
    {
        std::cerr << "数据读取失败" << std::endl;
        return -1;
    }

    buf[ret] = '\0';
    outString = buf;
    return 0;
}

int Client::SendData(std::string& inString)
{
    int ret = sendto(sock, inString.c_str(), inString.size(), 0, (struct sockaddr*)&server, sizeof(server));
    if (ret < 0)
    {
        std::cerr << "数据发送失败" << std::endl;
        return -1;
    }
    return 0;
}

Client::~Client()
{
    if (sock >= 0)
    {
        close(sock);
        sock = -1;
    }
}
#endif
