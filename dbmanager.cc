#include "dbmanager.h"
#include <stdio.h>
#include <muduo/base/Logging.h>

DBManager::DBManager() : _conn(0),
                         _res(0),
                         _row(0),
                         _bIsConnect(false)
{
    _conn = mysql_init(_conn);
}

DBManager::~DBManager()
{
    CleanResult();
    if (_bIsConnect) Close();
}

bool DBManager::Connect(char* server, char* user, char* password, char* database)
{
    if (_bIsConnect) return true;
	
    if (!mysql_real_connect(_conn, server, user, password, database, 0, 0, 0))
    {
        LOG_INFO << "connect faild";
        return false;
    }
    _bIsConnect = true;
    return true;
}

void DBManager::Close()
{
    mysql_close(_conn);
    _conn = 0;
    _bIsConnect = false;
}

int DBManager::Query(const char* szSQL)
{
    if (szSQL == 0 || strlen(szSQL) == 0) return false;
    if (!_bIsConnect) return false;
    try
    {
        if (mysql_real_query(_conn, szSQL, strlen(szSQL)) != 0)
        {
            printf("query faild : %s", szSQL);
            return false;
        } 
        CleanResult();
        _res = mysql_store_result(_conn);
    }
    catch (...)
    {
        printf("Query exception");
    }
    return true;
}

void DBManager::CleanResult()
{
    if (_res != 0)
    {
        mysql_free_result(_res);
        _res = 0;
    }
}

bool DBManager::GetResult()
{
    CleanResult();
    printf("Query exception");
    _res = mysql_store_result(_conn);

    return true;
}

char** DBManager::FetchRow()
{
    if (_res == NULL) return NULL;
    _row = mysql_fetch_row(_res);
    if (_row == NULL) return NULL;
    return _row;
}
