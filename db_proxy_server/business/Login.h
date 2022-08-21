#pragma once
#include "ImPduBase.h"
/**
 * 登录函数
*/
namespace DB_PROXY
{

/**
 * CImPdu:消息包
 * conn_uuid:连接的socketID
*/
void doLogin(CImPdu* pPdu, uint32_t conn_uuid);

}