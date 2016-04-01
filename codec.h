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
            int32_t be32 = *(static_cast<const int32_t*>(data) + 1); // SIGBUS
            const int32_t len = muduo::net::sockets::networkToHost32(be32);
//            printf("%d\n", len);
            if (len > 65536 || len < 0)
            {
                LOG_ERROR << "Invalid length " << len;
                conn->shutdown();  // FIXME: disable reading
                break;
            }
            else if (buf->readableBytes() >= len + kHeaderLen)
            {
                buf->retrieve(kHeaderLen + sizeof(header));
                if (IsHeader(data))
                {
                    switch (GetType(buf))
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
                            message_(conn, n, receiveTime);
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
              const muduo::StringPiece& message)
    {
        muduo::net::Buffer buf;
        buf.append(message.data(), message.size());
        int32_t len = static_cast<int32_t>(message.size());
        int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof be32);
        buf.prepend(header, sizeof header);
        conn->send(&buf);
    }
  
private:
    const static unsigned char header[4];
    MessageCallback message_;
    SignUpCallback signup_;
    const static size_t kHeaderLen = sizeof(int32_t);
};

const unsigned char LengthHeaderCodec::header[4] = {0xAA, 0x55, 0, 0}; 
#endif  // MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
