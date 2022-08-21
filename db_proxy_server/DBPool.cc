#include "DBPool.h"
#include "Logger.h"
#include  "CConfigFileReader.h"
#include  "util.h"
#define MIN_DB_CONN_CNT		2

CDBManager* CDBManager::s_db_manager = NULL;


CResultSet::CResultSet(MYSQL_RES* res)
{
	m_res = res;
	// map table field key to index in the result array
	int num_fields = mysql_num_fields(m_res);
	MYSQL_FIELD* fields = mysql_fetch_fields(m_res);
	for(int i = 0; i < num_fields; i++)
	{
	   m_key_map.insert(make_pair(fields[i].name, i));
	}
}

CResultSet::~CResultSet()
{
	if (m_res) {
		mysql_free_result(m_res);
		m_res = NULL;
	}
}


bool CResultSet::Next()
{
	m_row = mysql_fetch_row(m_res);
	if (m_row) {
		return true;
	} else {
		return false;
	}
}


int CResultSet::_GetIndex(const char* key)
{
	map<string, int>::iterator it = m_key_map.find(key);
	if (it == m_key_map.end()) {
		return -1;
	} else {
		return it->second;
	}
}

int CResultSet::GetInt(const char* key)
{
	int idx = _GetIndex(key);
	if (idx == -1) {
		return 0;
	} else {
		return atoi(m_row[idx]);
	}
}

char* CResultSet::GetString(const char* key)
{
	int idx = _GetIndex(key);
	if (idx == -1) {
		return NULL;
	} else {
		return m_row[idx];
	}
}

/////////////////////////////////////////

CPrepareStatement::CPrepareStatement()
{
	m_stmt = NULL;
	m_param_bind = NULL;
	m_param_cnt = 0;
}

CPrepareStatement::~CPrepareStatement()
{
	if (m_stmt) {
		mysql_stmt_close(m_stmt);
		m_stmt = NULL;
	}

	if (m_param_bind) {
		delete [] m_param_bind;
		m_param_bind = NULL;
	}
}

bool CPrepareStatement::Init(MYSQL* mysql, string& sql)
{
	mysql_ping(mysql);

	m_stmt = mysql_stmt_init(mysql);
	if (!m_stmt) {
		LOG_ERROR("mysql_stmt_init failed");
		return false;
	}

	if (mysql_stmt_prepare(m_stmt, sql.c_str(), sql.size())) {
		LOG_ERROR("mysql_stmt_prepare failed: %s", mysql_stmt_error(m_stmt));
		return false;
	}

	m_param_cnt = mysql_stmt_param_count(m_stmt);
	if (m_param_cnt > 0) {
		m_param_bind = new MYSQL_BIND [m_param_cnt];
		if (!m_param_bind) {
			LOG_ERROR("new failed");
			return false;
		}

		memset(m_param_bind, 0, sizeof(MYSQL_BIND) * m_param_cnt);
	}

	return true;
}

void CPrepareStatement::SetParam(uint32_t index, int& value)
{
	if (index >= m_param_cnt) {
		LOG_ERROR("index too large: %d", index);
		return;
	}

	m_param_bind[index].buffer_type = MYSQL_TYPE_LONG;
	m_param_bind[index].buffer = &value;
}

void CPrepareStatement::SetParam(uint32_t index, uint32_t& value)
{
	if (index >= m_param_cnt) {
		LOG_ERROR("index too large: %d", index);
		return;
	}

	m_param_bind[index].buffer_type = MYSQL_TYPE_LONG;
	m_param_bind[index].buffer = &value;
}

void CPrepareStatement::SetParam(uint32_t index, string& value)
{
	if (index >= m_param_cnt) {
		LOG_ERROR("index too large: %d", index);
		return;
	}

	m_param_bind[index].buffer_type = MYSQL_TYPE_STRING;
	m_param_bind[index].buffer = (char*)value.c_str();
	m_param_bind[index].buffer_length = value.size();
}

void CPrepareStatement::SetParam(uint32_t index, const string& value)
{
    if (index >= m_param_cnt) {
        LOG_ERROR("index too large: %d", index);
        return;
    }
    
    m_param_bind[index].buffer_type = MYSQL_TYPE_STRING;
    m_param_bind[index].buffer = (char*)value.c_str();
    m_param_bind[index].buffer_length = value.size();
}

bool CPrepareStatement::ExecuteUpdate()
{
	if (!m_stmt) {
		LOG_ERROR("no m_stmt");
		return false;
	}

	if (mysql_stmt_bind_param(m_stmt, m_param_bind)) {
		LOG_ERROR("mysql_stmt_bind_param failed: %s", mysql_stmt_error(m_stmt));
		return false;
	}

	if (mysql_stmt_execute(m_stmt)) {
		LOG_ERROR("mysql_stmt_execute failed: %s", mysql_stmt_error(m_stmt));
		return false;
	}

	if (mysql_stmt_affected_rows(m_stmt) == 0) {
		LOG_ERROR("ExecuteUpdate have no effect");
		return false;
	}

	return true;
}

uint32_t CPrepareStatement::GetInsertId()
{
	return mysql_stmt_insert_id(m_stmt);
}

/////////////////////

CDBConn::CDBConn(CDBPool* pPool)
{
	m_pDBPool = pPool;
	m_mysql = NULL;
}


CDBConn::~CDBConn()
{

}

int CDBConn::Init()
{
	m_mysql = mysql_init(NULL);
	if (!m_mysql) {
		LOG_ERROR("mysql_init failed");
		return 1;
	}

	bool reconnect = true;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);
	mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8mb4");

	if (!mysql_real_connect(m_mysql, m_pDBPool->GetDBServerIP(), m_pDBPool->GetUsername(), m_pDBPool->GetPasswrod(),
			m_pDBPool->GetDBName(), m_pDBPool->GetDBServerPort(), NULL, 0)) {
		LOG_ERROR("mysql_real_connect failed: %s", mysql_error(m_mysql));
		return 2;
	}

	return 0;
}


const char* CDBConn::GetPoolName()
{
	return m_pDBPool->GetPoolName();
}

/**
 * 执行具体的query语句 返回CResultSet对象
 * 注意释放问题
*/
CResultSet* CDBConn::ExecuteQuery(const char* sql_query)
{
	mysql_ping(m_mysql);

	if (mysql_real_query(m_mysql, sql_query, strlen(sql_query))) {
		LOG_ERROR("mysql_real_query failed: %s, sql: %s", mysql_error(m_mysql), sql_query);
		return NULL;
	}

	MYSQL_RES* res = mysql_store_result(m_mysql);
	if (!res) {
		LOG_ERROR("mysql_store_result failed: %s", mysql_error(m_mysql));
		return NULL;
	}

	CResultSet* result_set = new CResultSet(res);
	return result_set;
}

/**
 * Mysql 更新操作 返回true false
*/
bool CDBConn::ExecuteUpdate(const char* sql_query)
{
	mysql_ping(m_mysql);

	if (mysql_real_query(m_mysql, sql_query, strlen(sql_query))) {
		LOG_ERROR("mysql_real_query failed: %s, sql: %s", mysql_error(m_mysql), sql_query);
		return false;
	}

	if (mysql_affected_rows(m_mysql) > 0) {
		return true;
	} else {
		return false;
	}
}

char* CDBConn::EscapeString(const char* content, uint32_t content_len)
{
	if (content_len > (MAX_ESCAPE_STRING_LEN >> 1)) {
		m_escape_string[0] = 0;
	} else {
		mysql_real_escape_string(m_mysql, m_escape_string, content, content_len);
	}

	return m_escape_string;
}

uint32_t CDBConn::GetInsertId()
{
	return (uint32_t)mysql_insert_id(m_mysql);
}

////////////////
CDBPool::CDBPool(const char* pool_name, const char* db_server_ip, uint16_t db_server_port,
		const char* username, const char* password, const char* db_name, int max_conn_cnt)
{
	m_pool_name = pool_name;
	m_db_server_ip = db_server_ip;
	m_db_server_port = db_server_port;
	m_username = username;
	m_password = password;
	m_db_name = db_name;
	m_db_max_conn_cnt = max_conn_cnt;
	m_db_cur_conn_cnt = MIN_DB_CONN_CNT;
}


CDBPool::~CDBPool()
{
	for (list<CDBConn*>::iterator it = m_free_list.begin(); it != m_free_list.end(); it++) {
		CDBConn* pConn = *it;
		delete pConn;
	}

	m_free_list.clear();
}
/**
 * 连接池初始化
*/
int CDBPool::Init()
{
	for (int i = 0; i < m_db_cur_conn_cnt; i++) {
		CDBConn* pDBConn = new CDBConn(this);
		int ret = pDBConn->Init();
		if (ret) {
			delete pDBConn;
			return ret;
		}

		m_free_list.push_back(pDBConn);
	}

	LOG_ERROR("db pool: %s, size: %d", m_pool_name.c_str(), (int)m_free_list.size());
	return 0;
}

CDBConn* CDBPool::GetDBConn()
{
   std::unique_lock<std::mutex> lock(mutex_);
   while (m_free_list.empty()) {
    if(m_db_cur_conn_cnt>=m_db_max_conn_cnt)
    {
        cond_.wait(lock);//why ？
    }
    else {
        CDBConn * pDBConn = new CDBConn(this);
        int ret = pDBConn->Init();
        if(ret)
        {
            LOG_ERROR("Init DBConnection failed");
            delete pDBConn;
            return NULL;
        }
        else 
        {
            m_free_list.push_back(pDBConn);//添加到空闲队列
            m_db_cur_conn_cnt++;
            LOG_INFO("new db connection: %s, conn_cnt: %d\n", m_pool_name.c_str(), m_db_cur_conn_cnt);
        }
    }
   }
	CDBConn* pConn = m_free_list.front();
	m_free_list.pop_front();
    return pConn;
}

void CDBPool::RelDBConn(CDBConn* pConn)
{
	 std::unique_lock<std::mutex> lock(mutex_);
	auto it = m_free_list.begin();
	for (; it != m_free_list.end(); it++) {
		if (*it == pConn) {
			break;
		}
	}

	if (it == m_free_list.end()) {
		m_free_list.push_back(pConn);//归还连接
	} 
    cond_.notify_all();

}

/////////////////
CDBManager::CDBManager()
{

}

CDBManager::~CDBManager()
{

}

CDBManager* CDBManager::getInstance()
{
	if (!s_db_manager) {
		s_db_manager = new CDBManager();
		if (s_db_manager->Init()) {
			delete s_db_manager;
			s_db_manager = NULL;
		}
	}

	return s_db_manager;
}

int CDBManager::Init()
{
	CConfigFileReader config_file("dbproxyserver.conf");
	char* db_instances = config_file.GetConfigName("DBInstances");
	if (!db_instances) {
		LOG_ERROR("not configure DBInstances");
		return 1;
	}
	char host[64]={0};
	char port[64]={0};
	char dbname[64]={0};
	char username[64]={0};
	char password[64]={0};
    char maxconncnt[64]={0};
	CStrExplode instances_name(db_instances, ',');

	for (uint32_t i = 0; i < instances_name.GetItemCnt(); i++) {
		char* pool_name = instances_name.GetItem(i);
		snprintf(host, 64, "%s_host", pool_name);
		snprintf(port, 64, "%s_port", pool_name);
		snprintf(dbname, 64, "%s_dbname", pool_name);
		snprintf(username, 64, "%s_username", pool_name);
		snprintf(password, 64, "%s_password", pool_name);
        snprintf(maxconncnt, 64, "%s_maxconncnt", pool_name);

	    char* db_host = config_file.GetConfigName(host);
		char* str_db_port = config_file.GetConfigName(port);
		char* db_dbname = config_file.GetConfigName(dbname);
		char* db_username = config_file.GetConfigName(username);
		char* db_password = config_file.GetConfigName(password);
        char* str_maxconncnt = config_file.GetConfigName(maxconncnt);
		if (!db_host || !str_db_port || !db_dbname || !db_username || !db_password || !str_maxconncnt) {
			LOG_FATAL("not configure db instance: %s", pool_name);
			return 2;
		}
		int db_port = atoi(str_db_port);
        int db_maxconncnt = atoi(str_maxconncnt);
		CDBPool* pDBPool = new CDBPool(pool_name, db_host, db_port, db_username, db_password, db_dbname, db_maxconncnt);
		if (pDBPool->Init()) {
			LOG_FATAL("init db instance failed: %s", pool_name);
			return 3;
		}
		m_dbpool_map.insert(make_pair(pool_name, pDBPool));
	}

	return 0;
}






CDBConn* CDBManager::GetDBConn(const char* dbpool_name)
{
	map<string, CDBPool*>::iterator it = m_dbpool_map.find(dbpool_name);
	if (it == m_dbpool_map.end()) {
		return NULL;
	} else {
		return it->second->GetDBConn();
	}
}

void CDBManager::RelDBConn(CDBConn* pConn)
{
	if (!pConn) {
		return;
	}

	map<string, CDBPool*>::iterator it = m_dbpool_map.find(pConn->GetPoolName());
	if (it != m_dbpool_map.end()) {
		it->second->RelDBConn(pConn);
	}
}