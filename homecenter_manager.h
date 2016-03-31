#ifndef _MUDUO_EXAMPLE_ASIO_CHAT_HOMECENTER_MANAGER_H_
#define _MUDUO_EXAMPLE_ASIO_CHAT_HOMECENTER_MANAGER_H_

#include <map>
#include <muduo/net/TcpConnection.h>

using namespace muduo::net;
using namespace muduo;

class HomeCenterManager : boost::noncopyable
{
public:
    HomeCenterManager();
    ~HomeCenterManager();

    void onHeartBeat();
    void InsertHomeCenter(const int32_t id, const TcpConnectionPtr& homecenter);
    void DeleteHomeCenter(const int32_t id);
    void DeleteHomeCenter(const TcpConnectionPtr& device);
    bool FindHomeCenter(const int32_t id, TcpConnectionPtr& homecenter);
private:
    typedef std::map<int, WeakTcpConnectionPtr> HomeCenterList;

    HomeCenterList homecenters_;
};

#endif
