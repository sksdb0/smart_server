#ifndef DB_MANAGER_H_
#define DB_MANAGER_H_

#include "dbaccess.h"
#include "../smart_net.h"
#include <vector>
class DBManager
{
public:
    DBManager();
    ~DBManager();

    bool User_IsExist(const char* username);
    bool User_Insert(const char* username, const char* password, const char* email, const char* phone);
    bool User_Login(const char* id, const char* password, int32_t& userid);
    bool User_DeleteByName(const char* name);
    bool User_ModifyPassword(const char* username, const char* password);

    bool HomeCenter_Login(const char* id, const char* password, int32_t& homecenterid);

    bool GetHomeCenterIDByUserID(int32_t userid, std::vector<int32_t>& homecenters); 
    bool GetHomeCenterInfoByID(int32_t userid, homecenterNode& homecenters); 
    bool Connect();
private:
    DBAccess _dbaccess;
};

#endif
