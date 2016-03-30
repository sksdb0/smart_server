#include "dbmanager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

DBManager::DBManager()
{

}

DBManager::~DBManager()
{

}

bool DBManager::DeviceLogin(const char* id, const char* password, int32_t& deviceid)
{
    char sql[256] = {0};
    sprintf(sql, "SELECT dev_id, dev_password FROM smart_device WHERE dev_sn = '%s'", id);
    if (_dbaccess.Query(sql))
    {
        char** row = _dbaccess.FetchRow();
        if (row == NULL) return false;
        if (strcmp(row[1], password) == 0)
        {
            deviceid = atoi(row[0]);
            return true;
        }
    }
    return false;
}

bool DBManager::UserLogin(const char* id, const char* password, int32_t& userid)
{
    char sql[256] = {0};
    sprintf(sql, "SELECT id, password FROM smart_user WHERE username = '%s'", id);
    if (_dbaccess.Query(sql))
    {
        char** row = _dbaccess.FetchRow();
        if (row == NULL) return false;
        if (strcmp(row[1], password) == 0)
        {
            userid = atoi(row[0]);
            return true;
        }
    }
    return false;
}

bool DBManager::GetDeviceByUserID(int32_t userid, std::vector<int32_t>& devices)
{
    char sql[256] = {0};
    sprintf(sql, "SELECT dev_id FROM smart_user_dev_r WHERE user_id = %d", userid);
    if (_dbaccess.Query(sql))
    {
        char** row = NULL;
        do
        {
            row = _dbaccess.FetchRow();
            if (!row) break;
            devices.push_back(atoi(row[0]));
        }while(row);
        return true;
    }
    return false;
}

bool DBManager::Connect()
{
    char host[20] = {"127.0.0.1"};
    char user[20] = {"root"};
    char password[20] = {"rockrobo"};
    char database[20] = {"smart"};

    if (_dbaccess.Connect(host, user, password, database))
    {
        return false;
    }
    return true;
}
