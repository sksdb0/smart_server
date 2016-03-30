#ifndef DB_MANAGER_H_
#define DB_MANAGER_H_

#include "dbaccess.h"
#include <vector>
class DBManager
{
public:
    DBManager();
    ~DBManager();

    bool DeviceLogin(const char* id, const char* password, int32_t& deviceid);
    bool UserLogin(const char* id, const char* password, int32_t& userid);
    bool GetDeviceByUserID(int32_t userid, std::vector<int32_t>& devices); 
    bool Connect();
private:
    DBAccess _dbaccess;
};

#endif
