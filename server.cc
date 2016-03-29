#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>

#include <set>
#include <stdio.h>

#include "codec.h"
#include "dbmanager.h"
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
        InitDB(); 
    }
  
    void start()
    {
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
            if (DeviceLogin(message, deviceid))
            {
                device_.InsertDevice(deviceid, conn);
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
            if (UserLogin(message, userid))
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
            if (GetDeviceByUserID(conn->getid(), devices))
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

    int32_t DeviceLogin(const messageNode& message, int32_t& deviceid)
    {
        char sql[256] = {0};
        sprintf(sql, "select dev_id, dev_password from smart_device where dev_sn = '%s'", message.loginInfo.name);
        if (db_.Query(sql))
        {
            char** row = db_.FetchRow();
            if (row == NULL) return false;
            printf("id : %d password :%s\n", atoi(row[0]), row[1]);
            if (strcmp(row[1], message.loginInfo.password) == 0)
            {
                deviceid = atoi(row[0]);
                return true;
            }
        }
        return false;
    }

    bool UserLogin(const messageNode& message, int32_t& userid)
    {
        char sql[256] = {0};
        sprintf(sql, "select id, password from smart_user where username = '%s'", message.loginInfo.name);
        if (db_.Query(sql))
        {
            char** row = db_.FetchRow();
            printf("id : %d password :%s\n", atoi(row[0]), row[1]);
            if (strcmp(row[1], message.loginInfo.password) == 0)
            {
                userid = atoi(row[0]);
                return true;
            }
        }
        return false;
    }

    bool GetDeviceByUserID(int32_t userid, std::vector<int32_t>& devices)
    {
        char sql[256] = {0};
        sprintf(sql, "SELECT dev_id FROM smart_user_dev_r WHERE user_id = %d", userid);
        if (db_.Query(sql))
        {
            char** row = NULL;
            do
            {
                row = db_.FetchRow();
                if (!row) break;
                printf("devid %s", row[0]);
                devices.push_back(atoi(row[0]));
            }while(row);
            return true;
        }
        return false;    
    }

    bool InitDB()
    {
        char host[20] = "127.0.0.1";
        char user[20] = "root";
        char password[20] = "rockrobo";
        char database[20] = "smart";
        if (!db_.Connect(host, user, password, database))
        {
            LOG_INFO << "Init DB failed!"; 
            return false;
        }
        return true;
    }
  
    typedef std::set<TcpConnectionPtr> ConnectionList;

    TcpServer server_;
    LengthHeaderCodec codec_;
    ConnectionList connections_;
    DBManager db_;
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

