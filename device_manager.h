#ifndef _MUDUO_EXAMPLE_ASIO_CHAT_DEVICE_MANAGER_H_
#define _MUDUO_EXAMPLE_ASIO_CHAT_DEVICE_MANAGER_H_

#include <map>
#include <muduo/net/TcpConnection.h>

using namespace muduo::net;
using namespace muduo;

class DeviceManager : boost::noncopyable
{
public:
    DeviceManager();
    ~DeviceManager();

    void onHeartBeat();
    void InsertDevice(const int32_t id, const TcpConnectionPtr& device);
    void DeleteDevice(const int32_t id);
    void DeleteDevice(const TcpConnectionPtr& device);
    bool FindDevice(const int32_t id, TcpConnectionPtr& device);
private:
    typedef std::map<int, WeakTcpConnectionPtr> DeviceList;

    DeviceList devices_;
};

#endif
