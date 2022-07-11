#include "simple_log.h"
#include  "time_stamp.h"
#include <iostream>
CSimpleLogger &CSimpleLogger::instance()
{
    static CSimpleLogger logger;
    return logger;
}



//设置日志级别
void CSimpleLogger::setLogLevel(int level)
{
    logLevel_ = level;
}


//写日志

void  CSimpleLogger::log(std::string msg)
{
    switch (logLevel_)
    {
    case INFO:
        std::cout << "[INFO]";
        break;
    case ERROR:
        std::cout << "[ERROR]";
        break;
    case FATAL:
        std::cout << "[FATAL]";
        break;
    case DEBUG:
        std::cout << "[DEBUG]";
        break;
    default:
        break;
    }


        // 打印时间和msg
    std::cout << CTimeStamp::now().toString() << " : " << msg << std::endl;
}