#include "Locking.hpp"


Locks::Locks() : lock_count(0), global_lock(false, std::unique_ptr<std::mutex>(new std::mutex())) {}

std::uint32_t Locks::AddLock()
{
    locklist.insert(std::make_pair(++lock_count, std::make_tuple(false, std::unique_ptr<std::mutex>(new std::mutex()))));
    return lock_count;
}

void Locks::RemoveLock(std::uint32_t ID)
{
    auto it = locklist.find(ID);
    if (it != locklist.end())
    {
        if (std::get<0>(it->second))
        {
            std::get<1>(it->second)->unlock();
        }
        locklist.erase(it);
    }
}

void Locks::Lock(std::uint32_t ID)
{
    auto it = this->locklist.find(ID);
    if (it != this->locklist.end())
    {
        std::get<1>(it->second)->lock();
        std::get<0>(it->second) = true;
    }
}

bool Locks::TryLock(std::uint32_t ID)
{
    auto it = this->locklist.find(ID);
    if (it != this->locklist.end())
    {
        bool result = std::get<1>(it->second)->try_lock();
        if (result)
        {
            std::get<0>(it->second) = true;
        }
        return result;
    }
    return false;
}

void Locks::Unlock(std::uint32_t ID)
{
    auto it = this->locklist.find(ID);
    if (it != this->locklist.end())
    {
        if (std::get<0>(it->second))
        {
            std::get<1>(it->second)->unlock();
            std::get<0>(it->second) = false;
        }
    }
}

void Locks::GlobalLock()
{
    global_lock.second->lock();
    global_lock.first = true;
}

bool Locks::GlobalTryLock()
{
    bool result = global_lock.second->try_lock();
    if (result)
    {
        global_lock.first = true;
    }
    return result;
}

void Locks::GlobalUnlock()
{
    if (global_lock.first)
    {
        global_lock.second->unlock();
        global_lock.first = false;
    }
}

void Locks::Clear()
{
    this->GlobalUnlock();
    for (auto &it : locklist)
    {
        if (std::get<0>(it.second))
        {
            std::get<1>(it.second)->unlock();
        }
    }
    this->locklist.clear();
    decltype(locklist)().swap(locklist);
    lock_count = 0;
}
