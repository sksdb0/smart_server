#include "codec.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpClient.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace muduo;
using namespace muduo::net;

class ChatClient : boost::noncopyable
{
public:
    ChatClient(EventLoop* loop, const InetAddress& serverAddr)
      : client_(loop, serverAddr, "ChatClient"),
        codec_(boost::bind(&ChatClient::onMessage, this, _1, _2, _3),
        boost::bind(&ChatClient::onSignUp, this, _1, _2, _3))
    {
        client_.setConnectionCallback(boost::bind(&ChatClient::onConnection, this, _1));
        client_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
        client_.enableRetry();
    }
  
    void connect()
    {
        client_.connect();
    }
  
    void disconnect()
    {
        client_.disconnect();
    }
  
    void write(const StringPiece& message)
    {
        MutexLockGuard lock(mutex_);
        if (connection_)
        {
            codec_.send(get_pointer(connection_), message);
        }
    }
    void login()
    {
        char buf[256];
        sendpack(buf, User_Login, "xp", "7731939xp");
        write(StringPiece(buf, sizeof(messageNode)));
    }
  
private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");
    
        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }

    void onSignUp(const TcpConnectionPtr&,
                   const signupNode& message,
                   Timestamp)
    {
    }

    void onMessage(const TcpConnectionPtr&,
                   const messageNode& message,
                   Timestamp)
    {
        if (message.type == Get_Homecenter_id)
        {
            for (int i = 0; i < 10; i++)
            {
                LOG_INFO << message.int32_tbuf[i];
            }
        }
    }
  
    TcpClient client_;
    LengthHeaderCodec codec_;
    MutexLock mutex_;
    TcpConnectionPtr connection_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 2)
    {
        EventLoopThread loopThread;
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        InetAddress serverAddr(argv[1], port);
    
        ChatClient client(loopThread.startLoop(), serverAddr);
        client.connect();
        LOG_INFO << "login...";
        sleep(1);
        client.login();
        std::string line;
        while (std::getline(std::cin, line))
        {
  //          char buf[256];
  //          sendpack(buf, Control, "appclient", line.c_str());
  //          client.write(StringPiece(buf, sizeof(messageNode)));
  //          char buf[256];
  //          sendpack(buf, Get_Homecenter_id, "appclient", line.c_str());
  //          client.write(StringPiece(buf, sizeof(messageNode)));
            char buf[256];
            sendpack(&buf, Signup, "XPXP", "11", "22", "33");
            client.write(StringPiece(buf, sizeof(signupNode)));
        }
        client.disconnect();
        CurrentThread::sleepUsec(1000*1000);  // wait for disconnect, see ace/logging/client.cc
    }
    else
    {
        printf("Usage: %s host_ip port\n", argv[0]);
    }
}

