#include "InterlLogin.h"
#include "DBPool.h"

bool CInterLoginStrategy::doLogin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user)
{
    bool bRet = false;

    CDBManager* pDBManger = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManger->GetDBConn("teamtalk_slave");//从库获取一个连接
    if(pDBConn)
    {
        string strSql = "select * from IMUser where name='" + strName + "' and status=0";
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            std::string strResult,strSalt;
            uint32_t nId, nGender, nDeptId, nStatus;
            std::string strNick, strAvatar, strEmail, strRealName, strTel, strDomain,strSignInfo;

            while (pResultSet->Next()) {
                nId = pResultSet->GetInt("id");
                strResult = pResultSet->GetString("password");
                strSalt = pResultSet->GetString("salt");
                
                strNick = pResultSet->GetString("nick");
                nGender = pResultSet->GetInt("sex");
                strRealName = pResultSet->GetString("name");
                strDomain = pResultSet->GetString("domain");
                strTel = pResultSet->GetString("phone");
                strEmail = pResultSet->GetString("email");
                strAvatar = pResultSet->GetString("avatar");
                nDeptId = pResultSet->GetInt("departId");
                nStatus = pResultSet->GetInt("status");
                strSignInfo = pResultSet->GetString("sign_info");

            }
            
            string strOutPass = strPass + strSalt;
            //密码校验成功
            if(strOutPass == strResult)
            {
                bRet = true;
                user.set_user_id(nId);
                user.set_user_nick_name(strNick);
                user.set_user_gender(nGender);
                user.set_user_real_name(strRealName);
                user.set_user_domain(strDomain);
                user.set_user_tel(strTel);
                user.set_email(strEmail);
                user.set_avatar_url(strAvatar);
                user.set_department_id(nDeptId);
                user.set_status(nStatus);
  	            user.set_sign_info(strSignInfo);
            }
            delete  pResultSet;//资源释放
        }
         pDBManger->RelDBConn(pDBConn);//归还连接

    }
    return bRet;
}