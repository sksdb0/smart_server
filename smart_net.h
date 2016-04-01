#ifndef _SMART_NET_H_
#define _SMART_NET_H_



enum messageType
{   
    // app <--> homecenter
    Search_Homecenter = 0x10,
    Get_Nearby_Wifi,
    Link_Specified_Wifi,
    // server <--> homecenter
    Homecenter_Login,
    Heartbeat,
    // app <--> server
    Signup = 1,
    User_Login,
    Bind_User = 0x30,
    Control,
    Get_Homecenter_id,
    Get_Homecenter_Info
};

enum deviceType
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

// message SIGNUP
// app to server
struct signupNode
{
    int32_t type;
    char name[20];
    char password[32];
    char phone[13];
    char email[50];
};

// message GETCENTERINFO
// server to app
struct deviceNode
{
    int32_t centerid;
    int32_t type;
    int32_t id;
    char name[3][20];
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
        int32_t int32_tbuf[13];
        unsigned char ucharbuf[52];
    };
    messageNode()
    {
        type = 0;
        memset(int32_tbuf, 0, sizeof(int32_tbuf));
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

static inline int32_t sendpack(void* buf, const int32_t type, const char* name,\
        const char* password, const char* phone, const char* email)
{
    memcpy(buf, &type, 4);
    memcpy((static_cast<char*>(buf) + 4),  name, 20);
    memcpy((static_cast<char*>(buf) + 24), password, 32);
    memcpy((static_cast<char*>(buf) + 56), phone, 13);
    memcpy((static_cast<char*>(buf) + 69), email, 50);
    return sizeof(signupNode);
}
#endif
