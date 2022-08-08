#include "time_stamp.h"

CTimeStamp::CTimeStamp():microSecondsSinceEpoch_(0)
{

}



CTimeStamp::CTimeStamp(int64_t microSecondsSinceEpoch)
:microSecondsSinceEpoch_(microSecondsSinceEpoch)
{

}

CTimeStamp CTimeStamp::now()
{
    return CTimeStamp(time(NULL));
}



std::string CTimeStamp::toString() const
{
    char buf[128] = {0};
    tm *tm_time = localtime(&microSecondsSinceEpoch_);
    snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d", 
        tm_time->tm_year + 1900,
        tm_time->tm_mon + 1,
        tm_time->tm_mday,
        tm_time->tm_hour,
        tm_time->tm_min,
        tm_time->tm_sec);
    return buf;
}
