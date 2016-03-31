#ifndef _SMART_NET_H_
#define _SMART_NET_H_

enum messageType
{   
    SIGNUP = 0,
    HOMECENTERLOGIN,
    USERLOGIN,
    CONTROL,
    GETCENTERINFO,
    HEARTBEAT
};

enum terminalType
{
    WALLSWITCH = 0,
};

enum controlType
{
    RESET = 0,
    OPEN,
    CLOSE
};

enum timerType
{
};
 
struct messageNode
{   
    int32_t type;
    union
    {
        struct
        {
            char name[20];
            char password[32];
        }loginInfo;
        struct
        {
            int32_t terminalid;
            int32_t terminaltype;
            int32_t controltype;
        }terminalControl;
    };
};

static inline int32_t sendpack(void* buf, const messageNode& node)
{
    memcpy(buf, &node, sizeof(messageNode));
    return sizeof(messageNode);
}

static inline int32_t sendpack(void* buf, const int32_t type, const char* name, const char* password)
{
    memcpy(buf, &type, 4);
    memcpy((static_cast<char*>(buf) + 4), name, 20);
    memcpy((static_cast<char*>(buf) + 24), password, 32);
    return sizeof(messageNode);
}
#endif
