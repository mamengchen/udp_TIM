#ifndef __DATA_HPP__
#define __DATA_HPP__
#include <iostream>
#include <string>
#include "base_json.hpp"

class data
{
public:

    void val_to_str(std::string& outString);
    void str_to_val(std::string& inString);
public:
    std::string nickname;
    std::string school;
    std::string msg;
    std::string cmd;
};

void data::val_to_str(std::string& outString)
{
    Json::Value v;
    v["nickname"] = nickname;
    v["school"] = school;
    v["msg"] = msg;
    v["cmd"] = cmd;
    serialize(v, outString);
}

void data::str_to_val(std::string& inString)
{
    Json::Value v;
    deserialize(inString, v);
    nickname = v["nickname"].asString();
    school = v["school"].asString();
    msg = v["msg"].asString();
    cmd = v["cmd"].asString();
}
#endif
