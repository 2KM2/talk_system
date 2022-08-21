#pragma once

#include "LoginStrategy.h"

/**
 * 内部数据库验证策略
*/
class CInterLoginStrategy :public CLoginStrategy
{
public:
    virtual bool doLogin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user);
};