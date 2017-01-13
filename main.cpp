#include <Windows.h>
#include <iostream>
#include <sstream>
#include "Riddler.hpp"

HINSTANCE hInstance = nullptr;

inline std::size_t get_garbage_count()
{
    return garbagecleaner.count() + ThreadList.count() + AsyncList.count() + LockList.count() + ImgManager.count();
}

inline std::string print_garbage_count(std::size_t &total)
{
    std::stringstream ss;
    ss<<"\nObject count:   "<<garbagecleaner.count()<<"\n";
    ss<<"Thread count:   "<<ThreadList.count()<<"\n";
    ss<<"Async  count:   "<<AsyncList.count()<<"\n";
    ss<<"Lock   count:   "<<LockList.count()<<"\n";
    ss<<"Image  count:   "<<ImgManager.count()<<"\n";

    total = get_garbage_count();
    ss<<"The total garbage count is: "<<total<<"\n";

    return ss.str();
}

inline void print_cleanup(HANDLE hStdout)
{
    std::size_t total = 0;
    std::string result = print_garbage_count(total);
    if (total)
    {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout<<result;
    }
    else
    {
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout<<"\nRiddler: --- No garbage to cleanup..\n";
    }
}

inline void run_cleanup()
{
    CONSOLE_SCREEN_BUFFER_INFO Info;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &Info);
    int Attributes = Info.wAttributes;

    SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout<<"Garbage cleanup Initialized..";
    print_cleanup(hStdout);

        garbagecleaner.Clear();
        ThreadList.Clear(false);
        AsyncList.Clear();
        LockList.Clear();
        ImgManager.Clear();

    SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout<<"Garbage cleanup Finished..";
    print_cleanup(hStdout);

    SetConsoleTextAttribute(hStdout, Attributes);
}

extern "C" bool __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            hInstance = hinstDLL;
            DisableThreadLibraryCalls(hinstDLL);
            return true;
        }

        case DLL_THREAD_DETACH:
        {
            run_cleanup();
            break;
        }

        case DLL_PROCESS_DETACH:
        {
            run_cleanup();
            break;
        }

        default:
            break;
    }
    return true;
}
