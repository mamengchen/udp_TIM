#ifndef _BASE_JSON_HPP__
#define _BASE_JSON_HPP__
#include <iostream>
#include "./json/json.h"

//序列化：value ----> string;


void serialize(Json::Value& val, std::string& outString)
{
    Json::StyledWriter w;
    outString = w.write(val);
}

//反序列化：string ----> value;
void deserialize(std::string& inString, Json::Value& val)
{
    Json::Reader r;
    r.parse(inString, val, false);
}
#endif
