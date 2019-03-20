#include "data.hpp"
#include "base_json.hpp"
#include <iostream>

int main()
{
    data d;
    d.nickname = "zhangsan";
    d.school = "sust";
    d.msg = "hello";

    std::string s;
    d.val_to_str(s);
    std::cout << s << std::endl;
}
