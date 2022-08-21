#pragma once
/**
 * Mysql 连接管理 
*/
#include "ThreadPool.h"
#include <mysql/mysql.h>
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <list>
#include <string.h>
#define MAX_ESCAPE_STRING_LEN	10240
using namespace std;
/**
 * 数据库查询的结果封装
*/
class CResultSet
{
public:
    CResultSet(MYSQL_RES *res);
    virtual ~CResultSet();
    bool Next();
    int GetInt(const char *key);
    char *GetString(const char *key);
private:
    int _GetIndex(const char *key);

    MYSQL_RES*    m_res;
    MYSQL_ROW     m_row;
    std::map<std::string,int> m_key_map;
};

/**
 * 用MySQL的prepare statement接口来防止SQL注入
*/
class CPrepareStatement
{

public:
	CPrepareStatement();
	virtual ~CPrepareStatement();

	bool Init(MYSQL* mysql, string& sql);

	void SetParam(uint32_t index, int& value);
	void SetParam(uint32_t index, uint32_t& value);
    void SetParam(uint32_t index, string& value);
    void SetParam(uint32_t index, const string& value);

	bool ExecuteUpdate();
	uint32_t GetInsertId();
private:
	MYSQL_STMT*	m_stmt;
	MYSQL_BIND*	m_param_bind;
	uint32_t	m_param_cnt;
};
class CDBPool;

/**
 * Mysql具体的一个连接
*/
class CDBConn {
public:
	CDBConn(CDBPool* pDBPool);
	virtual ~CDBConn();
	int Init();

	CResultSet* ExecuteQuery(const char* sql_query);
	bool ExecuteUpdate(const char* sql_query);
	char* EscapeString(const char* content, uint32_t content_len);

	uint32_t GetInsertId();

	const char* GetPoolName();
	MYSQL* GetMysql() { return m_mysql; }
private:
	CDBPool* 	m_pDBPool;	// to get MySQL server information
	MYSQL* 		m_mysql;
	//MYSQL_RES*	m_res;
	char		m_escape_string[MAX_ESCAPE_STRING_LEN + 1];
};



class CDBPool {
public:
	CDBPool(const char* pool_name, const char* db_server_ip, uint16_t db_server_port,
			const char* username, const char* password, const char* db_name, int max_conn_cnt);
	virtual ~CDBPool();

	int Init();
	CDBConn* GetDBConn();
	void RelDBConn(CDBConn* pConn);

	const char* GetPoolName() { return m_pool_name.c_str(); }
	const char* GetDBServerIP() { return m_db_server_ip.c_str(); }
	uint16_t GetDBServerPort() { return m_db_server_port; }
	const char* GetUsername() { return m_username.c_str(); }
	const char* GetPasswrod() { return m_password.c_str(); }
	const char* GetDBName() { return m_db_name.c_str(); }
private:
	string 		m_pool_name;
	string 		m_db_server_ip;
	uint16_t	m_db_server_port;
	string 		m_username;
	string 		m_password;
	string 		m_db_name;
	int			m_db_cur_conn_cnt;
	int 		m_db_max_conn_cnt;
	list<CDBConn*>	m_free_list;//空闲队列
	//CThreadNotify	m_free_notify;
    std::mutex  mutex_;
    std::condition_variable cond_;
};


class CDBManager {
public:
	virtual ~CDBManager();

	static CDBManager* getInstance();

	int Init();

	CDBConn* GetDBConn(const char* dbpool_name);
	void RelDBConn(CDBConn* pConn);
private:
	CDBManager();

private:
	static CDBManager*		s_db_manager;
	map<string, CDBPool*>	m_dbpool_map;//连接管理
};