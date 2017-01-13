#ifndef LOCKS_HPP_INCLUDED
#define LOCKS_HPP_INCLUDED

#include <mutex>
#include <map>
#include <memory>

class Locks
{
    private:
        std::uint32_t lock_count;
        std::pair<bool, std::unique_ptr<std::mutex>> global_lock;
        std::map<std::uint32_t, std::tuple<bool, std::unique_ptr<std::mutex>>> locklist;

    public:
        Locks();
        ~Locks() {this->Clear();}
        std::uint32_t AddLock();
        void RemoveLock(std::uint32_t ID);
        void Lock(std::uint32_t ID);
        bool TryLock(std::uint32_t ID);
        void Unlock(std::uint32_t ID);
        void GlobalLock();
        bool GlobalTryLock();
        void GlobalUnlock();
        void Clear();

        inline std::size_t count() {return locklist.size();}
};

#endif // LOCKS_HPP_INCLUDED
