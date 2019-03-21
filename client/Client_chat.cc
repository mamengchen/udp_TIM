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
    Client* clientp = nw->cp;

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
        msg += "> ";
        msg += d.msg;

        if (d.cmd == "QUIT")
        {
            del_user(online_info);
        } else {
            add_user(online_info);
            winp->put_str_to_win(winp->output, step++, 2, msg);

            //判断当前的output框有没有满
            getmaxyx(winp->output, y, x);
            if (step == y - 1)
            {
                step = 1;
                sleep(1);
                winp->clear_win_line(winp->output, 1, y-1);
                winp->create_output();
            }
        }

        //显示好友列表
        int j = 1;
        std::vector<std::string>::iterator it = online.begin();
        for (; it != online.end(); ++it)
        {
            winp->put_str_to_win(winp->friend_list, j++, 2, *it);
            wrefresh(winp->friend_list);
            if (j == y-1)
            {
                j = 1;
                winp->clear_win_line(winp->friend_list, 1, y-1);
                winp->create_friend_list();
            }
        }
    }
    return NULL;
}

void* ShowInput(void* arg)
{
    net_win* nw = (net_win*)arg;
    window* winp = nw->wp;
    Client* clientp = nw->cp;

    data d;
    d.nickname = name;
    d.school = school;

    std::string mark = "> ";
    std::string outString;
    while (1)
    {
        winp->create_input();
        winp->put_str_to_win(winp->input, 1, 2, mark);
        winp->get_str(winp->input, d.msg);

        //序列化
        d.val_to_str(outString);
        clientp->SendData(outString);
        winp->clear_win_line(winp->input, 1, 1);
        wrefresh(winp->input);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " [ip]" << " [port]" << std::endl;
        return 1;
    }

    signal(SIGINT, quit);

    std::cout << "Please input your name> ";
    std::cin >> name;
    std::cout << "Please input your school> ";
    std::cin >> school;

    Client client(argv[1], atoi(argv[2]));
    client.InitClient();

    window win;
    net_win nw = {&client, &win};
    qclient = &client;

    //客户端需要创建三个线程，完成每一模块的工作
    pthread_t title, output, input;
    pthread_create(&title, NULL, ShowTitle, &nw);
    pthread_create(&input, NULL, ShowInput, &nw);
    pthread_create(&output, NULL, ShowOutput, &nw);
    pthread_join(title, NULL);
    pthread_join(input, NULL);
    pthread_join(output, NULL);
    return 0;
}

