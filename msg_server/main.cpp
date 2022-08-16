
#include "msgserver.h"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << std::endl;
        exit(-1);
    }

    MsgServer msg_server;


    return 0;
}