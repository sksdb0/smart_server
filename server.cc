#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>

#include <set>
#include <stdio.h>

#include "codec.h"
#include "db/dbaccess.h"
#include "db/dbmanager.h"
#include "device_manager.h"

using namespace muduo;
using namespace muduo::net;

class ChatServer : boost::noncopyable
{
public:
    ChatServer(EventLoop* loop, const InetAddress& listenAddr)
          : server_(loop, listenAddr, "ChatServer"),
      codec_(boost::bind(&ChatServer::onMessage, this, _1, _2, _3))
    {
        server_.setConnectionCallback(boost::bind(&ChatServer::onConnection, this, _1));
        server_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
        loop->runEvery(3.0, boost::bind(&DeviceManager::onHeartBeat, &device_));
        _db.Connect();
    }
  
    void start()
    {
//        bool a = _db.User_IsExist("XP");
//        bool b = _db.User_IsExist("XP1");
//        bool c = _db.User_IsExist("XP2");
//        bool d = _db.User_IsExist("XP3");
//        bool e = _db.User_Insert("XP11", "1", "1", "1");
//        bool g = _db.User_ModifyPassword("XP11", "2");
//        bool f = _db.User_DeleteByName("XP11");
//        printf("%d %d %d %d %d %d\n", a, b, c, d, e, g);
        server_.start();
    }
private:

    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");
    
        if (conn->connected())
        {   
            connections_.insert(conn);
        }
        else 
        {
            if (conn->gettype() == SMART_DEVICE) device_.DeleteDevice(conn);
            connections_.erase(conn);
        }
    }
  
    void onMessage(const TcpConnectionPtr& conn,
                   const messageNode& message,
                   Timestamp)
    {
        if (message.type == SIGNUP)
        {

        }
        else if (message.type == DEVICELOGIN)
        {
            int32_t deviceid = 0;
            if (_db.DeviceLogin(message.loginInfo.name, message.loginInfo.password, deviceid))
            {
                device_.InsertDevice(deviceid, conn);
                conn->settype(SMART_DEVICE);
                // answer client
                int type = message.type;
                char sztype[5] = {0};
                memcpy(sztype, &type, sizeof(type));
                codec_.send(get_pointer(conn), sztype);
            }
            else
            {
                LOG_INFO << "device password incorrect!";
            }
        }
        else if (message.type == USERLOGIN)
        {
            int32_t userid = 0;
            if (_db.User_Login(message.loginInfo.name, message.loginInfo.password, userid))
            {
                conn->setid(userid);
                LOG_INFO << "user login pass!";
            }
            else
            {
                LOG_INFO << "user password incorrect!";
            }
        }
        else if (message.type == CONTROL)
        {
            std::vector<int32_t> devices;
            if (_db.GetDeviceByUserID(conn->getid(), devices))
            {
                for (std::vector<int32_t>::iterator it = devices.begin(); it != devices.end(); it++)
                {
                    LOG_INFO << *it;
                    TcpConnectionPtr device;
                    if (device_.FindDevice(*it, device))
                    {
                        char buf[128] = {0};
                        sendpack(buf, CONTROL, "222", "111");
                        codec_.send(get_pointer(device), StringPiece(buf, sizeof(messageNode)));
                    }
                }
            }
        }
        else if (message.type == HEARTBEAT)
        {
            *boost::any_cast<muduo::Timestamp>(conn->getMutableContext()) = Timestamp::now(); 
        }
        else
        {
            LOG_INFO << "undefine message";
        }
    }

    typedef std::set<TcpConnectionPtr> ConnectionList;
    TcpServer server_;
    LengthHeaderCodec codec_;
    ConnectionList connections_;
    DBManager _db;
    DeviceManager device_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 1)
    {
        EventLoop loop;
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        InetAddress serverAddr(port);
        ChatServer server(&loop, serverAddr);
        server.start();
        loop.loop();
    }
    else
    {
        printf("Usage: %s port\n", argv[0]);
    }
}

