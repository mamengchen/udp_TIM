#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <iostream>
#include <string>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>

class window {
public:
    window();

    WINDOW* create_newwin(int h, int w, int y, int x);

    void create_title();        //绘制欢迎栏
    void create_output();       //绘制消息输出栏
    void create_friend_list();  //绘制好友列表栏
    void create_input();        //绘制消息输入栏

    void put_str_to_win(WINDOW* w, int y, int x, std::string& msg);    //将消息打印到屏幕上
    void clear_win_line(WINDOW* w, int begin, int lines);       //消去界面中原有的数据
    void welcome_title();                                       //打印滚动的title
    void get_str(WINDOW* w, std::string& outString);            //标准输入读取数据

    ~window();
public:
    WINDOW* title;
    WINDOW* output;
    WINDOW* friend_list;
    WINDOW* input;
    pthread_mutex_t w_lock;
};

window::window()
{
    initscr();
    curs_set(0);    //消去光标
    pthread_mutex_init(&w_lock, NULL);
}

WINDOW* window::create_newwin(int h, int w, int y, int x)
{
    pthread_mutex_lock(&w_lock);
    WINDOW* win = newwin(h, w, y, x);
    box(win, 0, 0);
    pthread_mutex_unlock(&w_lock);
    return win;
}

void window::create_title()
{
    int y = 0;
    int x = 0;
    int h = LINES/5;
    int w = COLS;
    title = create_newwin(h, w, y, x);
    wrefresh(title);
}

void window::create_output()
{
    int y = LINES/5;
    int x = 0;
    int h = (LINES*3)/5 - 1;
    int w = (COLS*3)/4;
    output = create_newwin(h, w, y, x);
    wrefresh(output);
}

void window::create_friend_list()
{
    int y = LINES/5;
    int x = (COLS*3)/4;
    int h = (LINES*3)/5;
    int w = (COLS*3)/4;
    output = create_newwin(h, w, y, x);
    wrefresh(output);
}

void window::create_input()
{
    int y = (LINES*4)/5;
    int x = 0;
    int h = LINES/5;
    int w = COLS;
    input = create_newwin(h, w, y, x);
    wrefresh(input);
}

void window::put_str_to_win(WINDOW* w, int y, int x, std::string& msg)
{
    pthread_mutex_lock(&w_lock);
    mvwaddstr(w, y, x, msg.c_str());
    wrefresh(w);
    pthread_mutex_unlock(&w_lock);
}

void window::clear_win_line(WINDOW* w, int begin, int lines)
{
    pthread_mutex_lock(&w_lock);
    while (lines-- > 0)
    {
        wmove(w, begin++, 0);
        wclrtoeol(w);
    }
    wrefresh(w);
    box(w, 0, 0);
    wrefresh(w);
    pthread_mutex_unlock(&w_lock);
}

void window::welcome_title()
{
    int x, y;
    getmaxyx(title, y, x);
    std::string run = "welcome to chat";
    int i = 1;
    int r_to_l = 0;
    while (1)
    {
        mvwaddstr(title, y/2, i, run.c_str());
        wrefresh(title);
        usleep(123456);
        clear_win_line(title, y/2, 1);
        create_title();     //避免边框也被清除，所以重新绘制
        int len = x - run.size();
        if (i == len - 1)
        {
            r_to_l = 1;
        }

        if (i == 1)
        {
            r_to_l = 0;
        }

        if (r_to_l == 1)
        {
            --i;
        }

        if (r_to_l == 0)
        {
            ++i;
        }
    }
}

void window::get_str(WINDOW* w, std::string& outString)
{
    char msg[1024];
    wgetnstr(w, msg, sizeof(msg));
    outString = msg;
}

window::~window()
{
    delwin(title);
    delwin(output);
    delwin(friend_list);
    delwin(input);
    curs_set(1);
    endwin();
    pthread_mutex_destroy(&w_lock);
}
#endif
