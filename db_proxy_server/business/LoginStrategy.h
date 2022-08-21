#pragma once

#include <iostream>
#include "IM.BaseDefine.pb.h"

class CLoginStrategy
{
public:
    virtual bool doLogin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user) = 0;
};