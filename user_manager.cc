#include "user_manager.h"
#include <muduo/base/Logging.h>
#include "smart_net.h"

UserManager::UserManager()
{
}

UserManager::~UserManager()
{
}

void UserManager::InsertUser(int32_t id, const TcpConnectionPtr& conn)
{
    users_.insert(std::pair<int32_t, WeakTcpConnectionPtr>(id, conn));
    conn->setid(id);
    conn->settype(SMART_USER);
}

void UserManager::DeleteUser(int32_t id)
{
    for (UserList::iterator it = users_.begin(); it != users_.end(); it++)
    {
        if (id == it->first)
        {
            users_.erase(it);
            LOG_INFO << "user disconnect";
        }
    }
}

void UserManager::DeleteUser(const TcpConnectionPtr& conn)
{
    for (UserList::iterator it = users_.begin(); it != users_.end(); it++)
    {
        if (conn == it->second.lock())
        {
            users_.erase(it);
            LOG_INFO << "user disconnect";
        }
    }
}

bool UserManager::FindUser(int32_t id, TcpConnectionPtr& user)
{
    bool bisfind = false;
    if (users_.find(id) != users_.end())
    {
        user = users_[id].lock();
        if (user) bisfind = true;
    }
    return bisfind;
}
