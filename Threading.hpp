#ifndef THREADING_HPP_INCLUDED
#define THREADING_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <system_error>
#include <map>
#include <mutex>

class Threads
{
    private:
        std::mutex map_lock;
        std::uint32_t thread_count;
        std::map<std::uint32_t, std::thread> threadmap;

    public:
        Threads() : thread_count(0) {};
        std::uint32_t StartThread(std::function<void()>& FunctionToPass);


        void Detach(std::uint32_t ThreadID);
        void Join(std::uint32_t ThreadID);
        void Clear(bool Join);

        std::uint32_t HardwareConcurrency();
        std::thread::native_handle_type NativeID(std::uint32_t ThreadID);

        inline std::size_t count() {return threadmap.size();}
};

#endif // THREADING_HPP_INCLUDED
