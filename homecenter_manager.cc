#include "homecenter_manager.h"
#include <muduo/base/Logging.h>

HomeCenterManager::HomeCenterManager()
{
}

HomeCenterManager::~HomeCenterManager()
{
}

void HomeCenterManager::onHeartBeat()
{
    for (HomeCenterList::iterator it = homecenters_.begin(); it != homecenters_.end(); it++)
    {
        TcpConnectionPtr conn = it->second.lock();
        const Timestamp time = boost::any_cast<const Timestamp&>(conn->getContext());
        LOG_INFO << time.toString().c_str();
    }
}

void HomeCenterManager::InsertHomeCenter(const int32_t id, const TcpConnectionPtr& homecenter)
{
    homecenters_.insert(std::pair<int32_t, WeakTcpConnectionPtr>(id, homecenter));
    homecenter->setContext(Timestamp::now());
    homecenter->setid(id);
    homecenter->settype(SMART_DEVICE);
}

void HomeCenterManager::DeleteHomeCenter(const int32_t id)
{
    for (HomeCenterList::iterator it = homecenters_.begin(); it != homecenters_.end(); it++)
    {
        if (id == it->first)
        {
            homecenters_.erase(it);
            LOG_INFO << "device disconnect";
        }
    }
}

void HomeCenterManager::DeleteHomeCenter(const TcpConnectionPtr& homecenter)
{
    for (HomeCenterList::iterator it = homecenters_.begin(); it != homecenters_.end(); it++)
    {
        if (homecenter == it->second.lock())
        {
            homecenters_.erase(it);
            LOG_INFO << "device disconnect";
        }
    }
}

bool HomeCenterManager::FindHomeCenter(const int32_t id, TcpConnectionPtr& homecenter)
{
    bool bisfind;
    if (homecenters_.find(id) != homecenters_.end())
    {
        homecenter = homecenters_[id].lock();
        bisfind = true;
    }
    else
    {
        bisfind = false;
    }

    return bisfind;
}
