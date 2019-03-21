#include <vector>
#include <signal.h>
#include "Client.hpp"
#include "../comm/data.hpp"
#include "../window.hpp"

typedef struct net_win {
    Client* cp;
    window* wp;
}net_win;

std::string name;
std::string school;
std::vector<std::string> online;
Client *qclient = NULL;

void quit(int sig)
{
    std::string out;
    data d;
    d.nickname = name;
    d.school = school;
    d.cmd = "QUIT";
    d.msg = "";
    d.val_to_str(out);
    qclient->SendData(out);
    endwin();
    _exit(0);
}

//构建一个vector维护当前在线的人数
//用户上线以后，添加至在线表
void add_user(std::string& user)
{
    std::vector<std::string>::iterator it = online.begin();
    while (it != online.end())
    {
        if (user == *it)
        {
            return;
        }
    }
    online.push_back(user);
}

//用户离线以后，从在线表删除
void del_user(std::string& user)
{
    std::vector<std::string>::iterator it = online.begin();
    while (it != online.end())
    {
        if (user == *it)
        {
            it = online.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

void* ShowTitle(void* arg)
{
    net_win* nw = (net_win*)arg;
    window* winp = nw->wp;
    winp->create_title();
    winp->welcome_title();
    return NULL;
}

void* ShowOutput(void* arg)
{
    net_win* nw = (net_win*)arg;
    window* winp = nw->wp;
    udp_client* clientp = nw->cp;

    //显式窗口数据的时候需要反序列化
    data d;
    std::string rstr;       //接收到的字符串
    std::string msg;        //要合成的信息
    std::string online_info;    //要在加入线表中的用户的信息
    int step = 1;
    int x, y;

    winp->create_output();
    winp->create_friend_list();
    while (1)
    {
        clientp->RecvData(rstr);
    
        //反序列化
        d.str_to_val(rstr);
        rstr.clear();

        msg = d.nickname;
        msg += "-";
        msg += d.school;
        online_info = msg; //这里在线列表只显示
    }
}

