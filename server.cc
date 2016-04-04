#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>

#include <set>
#include <stdio.h>

#include "codec.h"
#include "db/dbmanager.h"
#include "homecenter_manager.h"

using namespace muduo;
using namespace muduo::net;

class ChatServer : boost::noncopyable
{
public:
    ChatServer(EventLoop* loop, const InetAddress& listenAddr)
          : server_(loop, listenAddr, "ChatServer"),
      codec_(boost::bind(&ChatServer::onMessage, this, _1, _2, _3, _4),
             boost::bind(&ChatServer::onSignUp, this, _1, _2, _3))
    {
        server_.setConnectionCallback(boost::bind(&ChatServer::onConnection, this, _1));
        server_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
        loop->runEvery(3.0, boost::bind(&HomeCenterManager::onHeartBeat, &homecentermanager_));
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
            if (conn->gettype() == SMART_DEVICE) homecentermanager_.DeleteHomeCenter(conn);
            connections_.erase(conn);
        }
    }
  
    void onSignUp(const TcpConnectionPtr& conn,
                  const signupNode& signup,
                   Timestamp)
    {
        if (_db.User_Insert(signup.name, signup.password, signup.phone, signup.email))
        {
            // signup success
        }
    }

    void onMessage(const TcpConnectionPtr& conn,
                   const uint8_t type,
                   const messageNode& message,
                   Timestamp)
    {
        if (type == Homecenter_Login)
        {
            int32_t homecenterid = 0;
            if (_db.HomeCenter_Login(message.loginInfo.name, message.loginInfo.password, homecenterid))
            {
                homecentermanager_.InsertHomeCenter(homecenterid, conn);
                conn->settype(SMART_DEVICE);
                // answer client
                char sztype[5] = {0};
                memcpy(sztype, &type, sizeof(type));
                codec_.send(get_pointer(conn), type, sztype);
            }
            else
            {
                LOG_INFO << "homecenter password incorrect!";
            }
        }
        else if (type == User_Login)
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
        else if (type == Control)
        {
            std::vector<int32_t> homecenters;
            if (_db.GetHomeCenterIDByUserID(conn->getid(), homecenters))
            {
                for (std::vector<int32_t>::iterator it = homecenters.begin(); it != homecenters.end(); it++)
                {
                    LOG_INFO << *it;
                    TcpConnectionPtr homecenter;
                    if (homecentermanager_.FindHomeCenter(*it, homecenter))
                    {
                        char buf[128] = {0};
                        sendpack(buf, "control", "111");
                        codec_.send(get_pointer(homecenter), type, StringPiece(buf, sizeof(messageNode)));
                    }
                }
            }
        }
        else if (type == Get_Homecenter_Info)
        {
                        
        }
        else if (type == Get_Homecenter_id)
        {
            std::vector<int32_t> homecenters;
            if (_db.GetHomeCenterIDByUserID(conn->getid(), homecenters))
            {
                char buf[256] = {0};
                int32_t count = 0;
                for (std::vector<int32_t>::iterator it = homecenters.begin(); it != homecenters.end(); it++)
                {
                    LOG_INFO << *it;
                    homecenterNode node;
                    if (_db.GetHomeCenterInfoByID(*it, node))
                    {
                        node.state = homecentermanager_.IsHomeCenterOnline(node.id);
                        memcpy(buf + sizeof(homecenterInfo::count) + sizeof(node) * count++, &node, sizeof(node));
                        LOG_INFO << node.id;
                        LOG_INFO << node.name;
                        LOG_INFO << node.state;
                    }
                }
                if (count)
                {
                    memcpy(buf, &count, sizeof(int32_t));
                    codec_.send(get_pointer(conn), Get_Homecenter_id_App, StringPiece(buf,\
                        static_cast<int32_t>(sizeof(homecenterInfo::count) + sizeof(homecenterNode) * count)));
                }
            }
        }
        else if (type == Heartbeat)
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
    HomeCenterManager homecentermanager_;
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

