#include "user_manager.h"
#include <muduo/base/Logging.h>

UserManager::UserManager()
{
}

UserManager::~UserManager()
{
}

void UserManager::InsertUser(int32_t id, const TcpConnectionPtr& user)
{
    devices_.insert(std::pair<int32_t, WeakTcpConnectionPtr>(id, device));
}

void UserManager::UserUser(int32_t id)
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

bool UserManager::FindUser(int32_t id, const TcpConnectionPtr& user)
{
    return false;
}
