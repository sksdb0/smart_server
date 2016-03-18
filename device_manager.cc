#include "device_manager.h"
#include <muduo/base/Logging.h>

DeviceManager::DeviceManager()
{
}

DeviceManager::~DeviceManager()
{
}

void DeviceManager::onHeartBeat()
{
    for (DeviceList::iterator it = devices_.begin(); it != devices_.end(); it++)
    {
        TcpConnectionPtr conn = it->second.lock();
        const Timestamp time = boost::any_cast<const Timestamp&>(conn->getContext());
        LOG_INFO << time.toString().c_str();
    }
}

void DeviceManager::InsertDevice(const int32_t id, const TcpConnectionPtr& device)
{
    devices_.insert(std::pair<int32_t, WeakTcpConnectionPtr>(id, device));
    device->setContext(Timestamp::now());
    device->setid(id);
    device->settype(SMART_DEVICE);
}

void DeviceManager::DeleteDevice(const int32_t id)
{
    for (DeviceList::iterator it = devices_.begin(); it != devices_.end(); it++)
    {
        if (id == it->first)
        {
            devices_.erase(it);
            LOG_INFO << "device disconnect";
        }
    }
}

void DeviceManager::DeleteDevice(const TcpConnectionPtr& device)
{
    for (DeviceList::iterator it = devices_.begin(); it != devices_.end(); it++)
    {
        if (device == it->second.lock())
        {
            devices_.erase(it);
            LOG_INFO << "device disconnect";
        }
    }
}

bool DeviceManager::FindDevice(const int32_t id, TcpConnectionPtr& device)
{
    bool bisfind;
    if (devices_.find(id) != devices_.end())
    {
        device = devices_[id].lock();
        bisfind = true;
    }
    else
    {
        bisfind = false;
    }

    return bisfind;
}
