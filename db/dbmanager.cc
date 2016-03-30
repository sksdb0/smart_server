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

bool DBManager::User_IsExist(const char* username)
{
    char sql[256] = {0};
    sprintf(sql, "SELECT * FROM smart_user WHERE username = '%s'", username);
    if (_dbaccess.Query(sql))
    {
        char** row = _dbaccess.FetchRow();
        if (row == NULL) return false;
        return true;
    }
    return false;
}

bool DBManager::User_Insert(const char* username, const char* password, const char* phone, const char* email)
{
    if (username == NULL || password == NULL || email == NULL || phone == NULL) return false;
    if (strlen(username) == 0 || strlen(password) == 0 ||\
        strlen(email) == 0 || strlen(phone) == 0) return false;

    char sql[256] = {0};
    sprintf(sql, "INSERT INTO smart_user (username, password, phone, email)\
                  VALUES ('%s', '%s', '%s', '%s')", \
                  username, password, phone, email);
    return _dbaccess.Query(sql);
}

bool DBManager::User_Login(const char* id, const char* password, int32_t& userid)
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

bool DBManager::User_DeleteByName(const char* name)
{
    char sql[256] = {0};
    sprintf(sql, "DELETE FROM smart_user WHERE username = '%s'", name);
    return _dbaccess.Query(sql);
}

bool DBManager::User_ModifyPassword(const char* username, const char* password)
{
    char sql[256] = {0};
    sprintf(sql, "UPDATE smart_user SET password = '%s' WHERE username = '%s'", password, username);
    return _dbaccess.Query(sql);
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
