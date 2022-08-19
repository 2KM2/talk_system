#include <iostream>
#include <hiredis/hiredis.h>
#include <thread>
using namespace std;



int main()
{

    cout << "Test_Redis" << endl;
    redisContext *context=redisConnect("192.168.1.72",6379);

    if(context==nullptr)
    {
        cerr << "connect redis failed" << endl;
    }
    cout<<"Connect success"<< endl;
    cout<<"Test_Redis endl"<< endl;
    return 0;
}