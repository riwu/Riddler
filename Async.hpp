#ifndef AYNC_HPP_INCLUDED
#define AYNC_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <system_error>
#include <map>
#include <mutex>
#include <future>

class Async
{
    private:
        std::mutex map_lock;
        std::uint32_t thread_count;
        std::map<std::uint32_t, std::future<void>> threadmap;

    public:
        Async() : thread_count(0) {};
        std::uint32_t StartAsync(std::function<void()>& FunctionToPass);

        void Wait(std::uint32_t AsyncID);
        bool Wait_For(std::uint32_t AsyncID, std::uint32_t Milliseconds);
        void Clear();

        inline std::size_t count() {return threadmap.size();}
};

#endif // AYNC_HPP_INCLUDED
