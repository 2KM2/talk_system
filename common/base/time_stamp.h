#pragma once

#include <iostream>
#include <string>

using namespace std;
//时间类
class CTimeStamp
{
public:
    CTimeStamp();
    explicit CTimeStamp(int64_t microSecondsSinceEpoch);
    static CTimeStamp now();
    std::string toString() const;
private:
    int64_t microSecondsSinceEpoch_;
};