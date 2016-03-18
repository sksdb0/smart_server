#ifndef _DB_MANAGER_H_
#define _DB_MANAGER_H_

#include <mysql/mysql.h>

class DBManager
{
public:
    DBManager();
    ~DBManager();

    bool Connect(char* server, char* user, char* password, char* database);
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
