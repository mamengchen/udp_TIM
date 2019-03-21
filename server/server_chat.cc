#include "udp_server.hpp"
#include "../comm/data.hpp"

void* ProductEntry(void* arg)
{
    udp_server* server = (udp_server *)arg;
    while (1)
    {
        std::string str;
        server->RecvData(str);
        std::cout << str << std::endl;
    }
    return NULL;
}

void* ConsumeEntry(void* arg)
{
    udp_server* server = (udp_server *)arg;
    while (1)
    {
        server->broadcast();
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage : " << argv[0] << "[port]" << std::endl;
        return 1;
    }
    udp_server server(atoi(argv[1]));
    server.InitServer();

    //利用多线程，客户端发送数据，服务器读取数据并写入到pool中
    //进入事件循环
    pthread_t p,c;
    pthread_create(&p, NULL, ProductEntry, (void *)&server);    //负责接受数据
    pthread_create(&p, NULL, ConsumeEntry, (void *)&server);    //负责广播数据
    pthread_join(c, NULL);
    pthread_join(p, NULL);
    return 0;
}
