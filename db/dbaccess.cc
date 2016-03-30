#include "dbaccess.h"
#include <stdio.h>
#include <muduo/base/Logging.h>

DBAccess::DBAccess() : _conn(0),
                       _res(0),
                       _row(0),
                       _bIsConnect(false)
{
    _conn = mysql_init(_conn);
}

DBAccess::~DBAccess()
{
    CleanResult();
    if (_bIsConnect) Close();
}

bool DBAccess::Connect(const char* server, const char* user, const char* password, const char* database)
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

void DBAccess::Close()
{
    mysql_close(_conn);
    _conn = 0;
    _bIsConnect = false;
}

int DBAccess::Query(const char* szSQL)
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

void DBAccess::CleanResult()
{
    if (_res != 0)
    {
        mysql_free_result(_res);
        _res = 0;
    }
}

bool DBAccess::GetResult()
{
    CleanResult();
    printf("Query exception");
    _res = mysql_store_result(_conn);

    return true;
}

char** DBAccess::FetchRow()
{
    if (_res == NULL) return NULL;
    _row = mysql_fetch_row(_res);
    if (_row == NULL) return NULL;
    return _row;
}
