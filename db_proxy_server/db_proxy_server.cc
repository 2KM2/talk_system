#include "DBPool.h"
#include "Logger.h"
#include "version.h"
#include "CachePool.h"
#include "CConfigFileReader.h"
#include "TcpServer.h"
#include "EventLoop.h"
#include "DBServer.h"


int main(int argc,char *argv[])
{

	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: DBProxyServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}


    //init mysql 
 	CDBManager* pDBManager = CDBManager::getInstance();
	if (!pDBManager) {
		LOG_ERROR("DBManager init failed\n");
		return -1;
	}
    std::cout <<"msyql init success"<<std::endl;
    // init redis
	CacheManager* pCacheManager = CacheManager::getInstance();
	if (!pCacheManager) {
		LOG_ERROR("CacheManager init failed");
		return -1;
	}
    std::cout <<"redis init success"<<std::endl;

	CConfigFileReader config_file("dbproxyserver.conf");

	char* listen_ip = config_file.GetConfigName("ListenIP");
	char* str_listen_port = config_file.GetConfigName("ListenPort");
	char* str_thread_num = config_file.GetConfigName("ThreadNum");
    char* str_file_site = config_file.GetConfigName("MsfsSite");
    char* str_aes_key = config_file.GetConfigName("aesKey");


		if (!listen_ip || !str_listen_port || !str_thread_num || !str_file_site || !str_aes_key) {
		LOG_ERROR("missing ListenIP/ListenPort/ThreadNum/MsfsSite/aesKey, exit...");
		return -1;
	}

    if(strlen(str_aes_key) != 32)
    {
        LOG_ERROR("aes key is invalied");
        return -2;
    }
	
	uint16_t listen_port = atoi(str_listen_port);
	uint32_t thread_num = atoi(str_thread_num);



	EventLoop loop;
	InetAddress addr(listen_port,listen_ip);

	CDBServer  dbserver(&loop,addr,"DBServer");
	dbserver.start();
	loop.loop(); 
	return 0;
}