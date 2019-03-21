#ifndef __UDP_SERVER_HPP__
#define __UDP_SERVER_HPP__

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <semaphore.h>
#include "../data_pool/data_pool.hpp"
#include "../comm/data.hpp"

class udp_server {
public:
    udp_server(int _port);      //构造函数中不做任何有风险的创建动作
    int InitServer();
    void addUser(struct sockaddr_in& client);
    void delUser(struct sockaddr_in& client);
    int RecvData(std::string& outString);
    int SendData(const std::string& inString, struct sockaddr_in& peer, const socklen_t& len);
    int broadcast();            //服务器收到消息以后，转发给所有在线的用户
    ~udp_server();
private:
    std::string ip;
    int port;
    int sock;

    std::map<int, struct sockaddr_in> online; //维护一个用户表
    data_pool pool;             //维护一个存放数据的数据池
};

udp_server::udp_server(int _port)
    :port(_port), sock(-1), pool(256)
{}

int udp_server::InitServer()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "服务器创建套接字失败" << std::endl;
        return -1;
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        std::cerr << "绑定失败" << std::endl;
        return 2;
    }
    return 0;
}

void udp_server::addUser(struct sockaddr_in& client)
{
    online.insert(std::pair<int, struct sockaddr_in>(client.sin_addr.s_addr, client));
}

void udp_server::delUser(struct sockaddr_in& client)
{
    std::map<int, struct sockaddr_in>::iterator it = online.find(client.sin_addr.s_addr);
    if (it != online.end())
    {
        online.erase(it);
    }
}

int udp_server::RecvData(std::string& outString)
{
    char buf[1024] = {0};
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int ret = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&client, &len);

    //只有接收数据成功了以后，我们才需要对数据进行处理
    if (ret > 0)
    {
        buf[ret] = '\0';
        outString = buf;
        pool.PutData(outString);
        data d;
        d.str_to_val(outString);
        if (d.cmd == "QUIT")
        {
            delUser(client);
        } else {
            addUser(client);
        }
        return 0;
    }
    return -1;
}

int udp_server::SendData(const std::string& inString, struct sockaddr_in& peer, const socklen_t& len)
{
    int ret = sendto(sock, inString.c_str(), inString.size(), 0, (struct sockaddr*)&peer, len);

    if (ret < 0)
    {
        std::cerr << "发送失败" << std::endl;
    }
    return 0;
}

int udp_server::broadcast()
{
    std::string data;
    pool.GetData(data);
    std::map<int, struct sockaddr_in>::iterator it = online.begin();
    for (; it != online.end(); ++it)
    {
        SendData(data, it->second, sizeof(it->second));
    }
    return 0;
}

udp_server::~udp_server()
{
    if (sock >= 0)
    {
        close(sock);
        sock = -1;
    }
}
#endif
