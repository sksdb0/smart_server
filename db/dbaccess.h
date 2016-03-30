#ifndef _DB_ACCESS_H_
#define _DB_ACCESS_H_

#include <mysql/mysql.h>

class DBAccess
{
public:
    DBAccess();
    ~DBAccess();

    bool Connect(const char* server, const char* user, const char* password, const char* database);
    void Close();	
    
    int Query(const char* szSQL);    
    char** FetchRow();
private:
    void CleanResult();
    bool GetResult(); 

    MYSQL *_conn;
    MYSQL_RES *_res;
    MYSQL_ROW _row;
    bool _bIsConnect;
};

#endif
