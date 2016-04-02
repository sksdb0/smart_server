#ifndef _SMART_NET_H_
#define _SMART_NET_H_



enum messageType
{   
    // app <--> homecenter
    Search_Homecenter = 0x10,
    Get_Nearby_Wifi,
    Link_Specified_Wifi,
    // server <--> homecenter
    Homecenter_Login = 0x30,
    Get_Homecenter_id,
    Get_Homecenter_Info,
    Heartbeat,
    // app <--> server
    Signup = 0x50,
    User_Login,
    Bind_User,
    Control,
    Get_Homecenter_id_App,
    Get_Homecenter_Info_App
};

enum deviceType
{
    WallSwitch_One_Channel = 0,
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

// message Signup app to server
struct signupNode
{
    char name[20];
    char password[32];
    char phone[13];
    char email[50];
};

// message Get_Homecenter_Info server to app
struct deviceNode
{
    int32_t centerid;
    int32_t type;
    int32_t id;
    char name[3][20];
};

// message Lint_Speceified_Wifi app to homecenter
struct link_specified_wifi_Node
{
    char wifiname[32];
    char wifipassword[32];
    char serverip[20];
    int32_t port;
};
 
struct messageNode
{   
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
        memset(int32_tbuf, 0, sizeof(int32_tbuf));
    };
};

static inline int32_t sendpack(void* buf, const messageNode& node)
{
    memcpy(buf, &node, sizeof(messageNode));
    return sizeof(messageNode);
}

static inline int32_t sendpack(void* buf, const char* name, const char* password)
{
    memcpy((static_cast<char*>(buf)), name, 20);
    memcpy((static_cast<char*>(buf) + 20), password, 32);
    return sizeof(messageNode);
}

static inline int32_t sendpack(void* buf, const char* name,\
        const char* password, const char* phone, const char* email)
{
    memcpy((static_cast<char*>(buf)),  name, 20);
    memcpy((static_cast<char*>(buf) + 20), password, 32);
    memcpy((static_cast<char*>(buf) + 52), phone, 13);
    memcpy((static_cast<char*>(buf) + 65), email, 50);
    return sizeof(signupNode);
}
#endif
