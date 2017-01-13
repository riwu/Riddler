#include "Threading.hpp"

std::uint32_t Threads::StartThread(std::function<void()> &FunctionToPass)
{
    std::lock_guard<std::mutex> guard(map_lock);
    threadmap.insert(std::pair<std::uint32_t, std::thread>(++thread_count, std::thread(FunctionToPass)));
    return thread_count;
}

void Threads::Detach(std::uint32_t ThreadID)
{
    std::lock_guard<std::mutex> guard(map_lock);
    auto it = this->threadmap.find(ThreadID);
    if (it != this->threadmap.end())
    {
        if (it->second.joinable())
        {
            it->second.detach();
            this->threadmap.erase(it);
        }
    }
}

void Threads::Join(std::uint32_t ThreadID)
{
    std::lock_guard<std::mutex> guard(map_lock);
    auto it = this->threadmap.find(ThreadID);
    if (it != this->threadmap.end())
    {
        if (it->second.joinable())
        {
            it->second.join();
        }
    }
}

void Threads::Clear(bool Join)
{
    //std::lock_guard<std::mutex> guard(map_lock);
    for (auto &it : this->threadmap)
    {
        if (it.second.joinable())
        {
            if (Join)
            {
                it.second.join();
            }
            else
            {
                it.second.detach();
            }
        }
        else
        {
            it.second.detach();
        }
    }

    this->threadmap.clear();
    decltype(threadmap)().swap(threadmap);
    thread_count = 0;
}

std::uint32_t Threads::HardwareConcurrency()
{
    return std::thread::hardware_concurrency();
}

std::thread::native_handle_type Threads::NativeID(std::uint32_t ThreadID)
{
    std::lock_guard<std::mutex> guard(map_lock);
    auto it = this->threadmap.find(ThreadID);
    if (it != this->threadmap.end())
    {
        return it->second.native_handle();
    }
    return 0;
}
