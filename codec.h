#ifndef MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
#define MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H

#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Endian.h>
#include <muduo/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <stdio.h>

#include "smart_net.h"

class LengthHeaderCodec : boost::noncopyable
{
public:
    typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
                                  const int32_t type,
                                  const messageNode& message,
                                  muduo::Timestamp)> MessageCallback;

    typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
                                  const signupNode& signup,
                                  muduo::Timestamp)> SignUpCallback;

    explicit LengthHeaderCodec(const MessageCallback& messagecb,
                               const SignUpCallback& signupcb)
      : message_(messagecb), signup_(signupcb)
    {
    }
 
    bool IsHeader(const void* ptr)
    {
        if (memcmp(header, ptr, sizeof(header)) == 0) return true;
        return false;
    }

    int32_t GetType(muduo::net::Buffer* buf)
    {
        int32_t type = 0;
        memcpy(&type, buf->peek(), sizeof(int32_t));
        return type;
    }
  
    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime)
    {
        while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4
        {
            // FIXME: use Buffer::peekInt32()
            const void* data = buf->peek();
            const void* temp = static_cast<const char*>(data) + 3;
            const int16_t len = *static_cast<const int16_t*>(temp);
         //   printf("%d\n", len);
            if (len > 16384 || len < 0)
            {
                LOG_ERROR << "Invalid length " << len;
                conn->shutdown();  // FIXME: disable reading
                break;
            }
            else if (buf->readableBytes() >= len + kHeaderLen)
            {
                buf->retrieve(kHeaderLen + sizeof header + sizeof ender + 1);
                if (IsHeader(data))
                {
                    temp = static_cast<const char*>(data) + 2;
                    uint8_t type = *static_cast<const uint8_t*>(temp);
                    switch (type)
                    {
                        case Signup:
                        {
                            signupNode n;
                            memcpy(&n, buf->peek(), sizeof(signupNode));
                            signup_(conn, n, receiveTime);
                            break;
                        }
                        default:
                        {
                            messageNode n;
                            memcpy(&n, buf->peek(), sizeof(messageNode));
                            message_(conn, type, n, receiveTime);
                        }
                    }
                }
                buf->retrieve(len);
            }
            else
            {
                break;
            }
        }
    }
  
    // FIXME: TcpConnectionPtr
    void send(muduo::net::TcpConnection* conn,
              const uint8_t type,
              const muduo::StringPiece& message)
    {
        muduo::net::Buffer buf;
        buf.append(message.data(), message.size());
        int16_t len = static_cast<int16_t>(message.size());
    //    printf("%d\n", len);
        buf.prepend(ender, sizeof ender);
        buf.prepend(&len, sizeof len);
        buf.prepend(&type, sizeof type);
        buf.prepend(header, sizeof header);
        conn->send(&buf);
    }
  
    // FIXME: TcpConnectionPtr
    void send1(muduo::net::TcpConnection* conn,
              const uint8_t type,
              const muduo::StringPiece& message)
    {
        muduo::net::Buffer buf;
        buf.append("", 0);
        int16_t len = static_cast<int16_t>(message.size());
    //    printf("%d\n", len);
        buf.prepend(ender, sizeof ender);
        buf.prepend(&len, sizeof len);
        buf.prepend(&type, sizeof type);
        buf.prepend(header, sizeof header);
        conn->send(&buf);
    }
   
private:
    const static unsigned char header[2];
    const static unsigned char ender[1];
    MessageCallback message_;
    SignUpCallback signup_;
    const static size_t kHeaderLen = sizeof(int16_t);
};

const unsigned char LengthHeaderCodec::ender[1] = {0x0};
const unsigned char LengthHeaderCodec::header[2] = {0xAA, 0x55}; 
#endif  // MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
