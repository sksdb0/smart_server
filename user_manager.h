#ifndef _MUDUO_EXAMPLE_ASIO_CHAT_DEVICE_MANAGER_H_
#define _MUDUO_EXAMPLE_ASIO_CHAT_DEVICE_MANAGER_H_

#include <map>
#include <muduo/net/TcpConnection.h>

using namespace muduo::net;

class UserManager : boost::noncopyable
{
public:
    UserManager();
    ~UserManager();

    void InsertUser(int32_t id, const TcpConnectionPtr& user);
    void DeleteUser(int32_t id);
    bool FindUser(int32_t id, const TcpConnectionPtr& user);
private:
    typedef std::map<int, WeakTcpConnectionPtr> UserList;

    UserList users_;
};

#endif
