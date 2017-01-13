#include "Async.hpp"

std::uint32_t Async::StartAsync(std::function<void()> &FunctionToPass)
{
    std::lock_guard<std::mutex> guard(map_lock);
    threadmap.insert(std::pair<std::uint32_t, std::future<void>>(++thread_count, std::async(std::launch::async, FunctionToPass)));
    return thread_count;
}

void Async::Wait(std::uint32_t AsyncID)
{
    auto it = this->threadmap.find(AsyncID);
    if (it != this->threadmap.end())
        it->second.wait();
}

bool Async::Wait_For(std::uint32_t AsyncID, std::uint32_t Milliseconds)
{
    auto it = this->threadmap.find(AsyncID);
    if (it != this->threadmap.end())
        return it->second.wait_for(std::chrono::milliseconds(Milliseconds)) == std::future_status::ready;

    return false;
}

void Async::Clear()
{
    std::lock_guard<std::mutex> guard(map_lock);
    this->threadmap.clear();
    decltype(threadmap)().swap(threadmap);
    thread_count = 0;
}
