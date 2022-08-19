#pragma once


#include <string>

//User表的ORM类

typedef struct {
    uint32_t msg_id;
    uint32_t from_id;
    uint64_t timestamp;
} msg_ack_t;

class User
{
public:

protected:
    int id_;
    std::string name_;
    std::string password_;
};