#ifndef RIDDLER_HPP_INCLUDED
#define RIDDLER_HPP_INCLUDED

#if defined _WIN32 || defined _WIN64
#include <winternl.h>
#include <Windows.h>
#include <TlHelp32.h>
#endif

#include <algorithm>
#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <bitset>
#include <cstring>
#include "GC.hpp"
#include "PSString.hpp"
#include "PSArray.hpp"
#include "Threading.hpp"
#include "Locking.hpp"
#include "ImageManager.hpp"
#include "Async.hpp"
//#include "jni.h"

#ifndef __cplusplus
#error
#endif

#ifdef BUILD_DLL
#define DLLSPEC __declspec(dllexport)
#else
#define DLLSPEC __declspec(dllimport)
#endif

/** Variables **/
extern GC garbagecleaner;
extern Threads ThreadList;
extern Locks LockList;
extern ImageManager ImgManager;
extern Async AsyncList;

/** Structures **/
typedef struct _LDR_MODULE
{
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

typedef struct _ProcessModuleInfo
{
    std::uint32_t Size;
    std::uint32_t Initialized;
    HANDLE SsHandle;
    LIST_ENTRY LoadOrder;
    LIST_ENTRY InitOrder;
    LIST_ENTRY MemoryOrder;
} ProcessModuleInfo, *pProcessModuleInfo;

typedef struct _REMOVED_MODULE
{
    void* ModuleBase;
    LDR_MODULE Module;
    std::wstring BaseDllName;
    std::wstring FullDllName;
} REMOVED_MODULE, *PREMOVED_MODULE;

struct WindowHandleStructure
{
    unsigned long PID;
    HWND WindowHandle;
};

/** Functions **/

#ifdef __cplusplus
extern "C"
{
#endif

	static const std::string Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    DLLSPEC bool RemoveGC(const void* ptr);
	DLLSPEC const char* ErrorMessage(std::uint32_t Error, bool Throw);
	DLLSPEC std::uint32_t QuickHammingDistance(const char* First, const char* Second);
	DLLSPEC std::uint32_t HammingDistance(const char* First, const char* Second);
	DLLSPEC std::uint32_t LevenshteinDistance(const char* First, const char* Second);
	DLLSPEC const wchar_t* UTF8ToUTF16(const char* utf8);
    DLLSPEC const char* UTF16ToUTF8(const wchar_t* utf16);
	DLLSPEC const char* GetProcessModuleInfo(const char* Process);
	DLLSPEC const char* EncodeBase64(const char* Data);
	DLLSPEC const char* DecodeBase64(const char* Data);
	DLLSPEC std::uint32_t* CRCGenerateTable();
	DLLSPEC const std::uint32_t* CRCLookUpTable();
	DLLSPEC std::uint32_t Adler32(std::uint8_t* Data, std::uint32_t Size);
	DLLSPEC std::uint32_t CRC32(std::uint8_t* Data, std::size_t Size, std::uint32_t InitialValue = 0xEDB88320);
    DLLSPEC HWND GetWindowHandle(const char* WindowName, const char* WindowClass);
    DLLSPEC HWND GetChildWindowHandle(HWND Parent, HWND ChildAfter, const char* WindowName, const char* WindowClass);
	DLLSPEC HWND GetDesktopWindowHandle();
	DLLSPEC void GetWindowSize(HWND WindowHandle, int &Width, int &Height);
	DLLSPEC const char* GetDisplayDevices();
	DLLSPEC DISPLAY_DEVICE GetPrimaryDisplayDevice();
	DLLSPEC const char* GetDisplaySettings(int Index);
	DLLSPEC bool SetDefaultDisplaySettings();
	DLLSPEC bool SetDisplayResolution(int width, int height);
	DLLSPEC void* CreateMemoryMap(const char* MapName, unsigned int size);
	DLLSPEC void* OpenMemoryMap(const char* MapName, unsigned int size);
	DLLSPEC HANDLE CreateNewEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, bool bManualReset, bool bInitialState, const char* EventName);
	DLLSPEC std::uint32_t OpenSingleEvent(const char* EventName, bool InheritHandle, std::uint32_t dwDesiredAccess = EVENT_ALL_ACCESS, std::uint32_t dwMilliseconds = INFINITE);
	DLLSPEC bool SetEventSignal(const char* EventName, bool Signaled);
	DLLSPEC bool DeleteEvent(const char* EventName);
	DLLSPEC void FlipImage(void* In, void* &Out, int width, int height, unsigned int Bpp);
	DLLSPEC int MessageBoxTimeOut(HWND hWnd, const char* lpText, const char* lpCaption, std::uint32_t uType, std::uint32_t dwMilliseconds, std::uint16_t wLanguageId = 0);
	DLLSPEC std::int64_t ReverseNumber(std::int64_t Number);
	DLLSPEC const char* GetEnvVariable(const char* Variable);
	DLLSPEC bool SetEnvVariable(const char* VariableName, const char* VariableValue);
	DLLSPEC void QuickSort(void* base, size_t num, size_t size, int(*compar)(const void*, const void*));
	DLLSPEC void SetWindowTransparency(HWND hwnd, std::uint8_t Transperancy);
	DLLSPEC int GetCurrentProcessID();
	DLLSPEC void AppendWindowText(HWND WindowHandle, const char* Text);
	DLLSPEC HWND WindowHandleFromPoint(POINT point);
	DLLSPEC HWND ChildWindowHandleFromPoint(HWND WindowHandle, POINT point);
	DLLSPEC PROCESSENTRY32 GetProcessInfo(const char* ProcessName);
	DLLSPEC bool IsProcessRunning(const char* pProcessName);
	DLLSPEC HWND FindMainWindow(unsigned long PID);
	DLLSPEC HWND GetWindowParent(HWND Child);
	DLLSPEC HWND SetWindowParent(HWND Parent, HWND Child);
	DLLSPEC void GetWindowDimensions(HWND WindowHandle, RECT &rect);
	DLLSPEC HWND GetPreviousWindowHandle(HWND WindowHandle);
	DLLSPEC HWND GetNextWindowHandle(HWND WindowHandle);
	DLLSPEC const char* ProcessInfoToString(PROCESSENTRY32 Proc32);
	DLLSPEC MODULEENTRY32* GetModulesInfo(std::uint32_t ProcessID);
	DLLSPEC MODULEENTRY32 GetModuleInfo(std::uint32_t ProcessID, const char* ModuleName);
	DLLSPEC const char* ModuleInfoToString(MODULEENTRY32 Mod32);
	DLLSPEC bool RunApplication(const char* ApplicationName, char* ApplicationArguments, bool WaitInputIdle);
	DLLSPEC bool StartServiceEx(const char* szServiceName);
	DLLSPEC bool StopServiceEx(const char* szServiceName);
	DLLSPEC bool CreateRegistryKey(HKEY Root, const char* SubRoot, const char* KeyName, DWORD KeyType, const char* InitialValue, REGSAM samDesired = 0);
	DLLSPEC bool DeleteRegistryKey(HKEY Root, const char* KeyToRemove, REGSAM samDesired = 0);
	DLLSPEC bool DeleteRegistryValue(HKEY Root, const char* SubRoot, const char* KeyToRemove, REGSAM samDesired = 0);
	DLLSPEC bool SetRegistryKey(HKEY Root, const char* SubRoot, const char* KeyToEdit, const char* NewValue, DWORD KeyType, REGSAM samDesired = 0);
	DLLSPEC int ReadRegistryKey(HKEY Root, const char* SubRoot, const char* KeyToRead, DWORD KeyType, char* &Result, REGSAM samDesired = 0);
    DLLSPEC bool GetDriveInfo(const char* Drive, unsigned short &FreeSpace, unsigned short &TotalSpace, bool InGigaBytes = false);
    DLLSPEC const char* GetTypeOfDrive(const char* Drive);
    DLLSPEC int GetLogicalDrivesList(char Drives[26]);
    DLLSPEC unsigned int Thread_CreateThread(void* Pointer);
    DLLSPEC void Thread_DetachThread(unsigned int ThreadID);
    DLLSPEC void Thread_JoinThread(unsigned int ThreadID);
    DLLSPEC void Thread_ClearThreads(bool Join);
    DLLSPEC unsigned int Thread_HardwareConcurrency();
    DLLSPEC unsigned int Thread_NativeID(unsigned int ThreadID);
    DLLSPEC void Thread_SleepThreadFor(unsigned int Milliseconds);
    DLLSPEC void Thread_YieldThread();
    DLLSPEC unsigned int Async_CreateAsync(void* Pointer);
    DLLSPEC void Async_ClearAsync();
    DLLSPEC void Async_Wait(unsigned int AsyncID);
    DLLSPEC bool Async_WaitFor(unsigned int AsyncID, unsigned int Milliseconds);
    DLLSPEC unsigned int Lock_CreateLock();
    DLLSPEC void Lock_RemoveLock(unsigned int LockID);
    DLLSPEC void Lock_Lock(unsigned int LockID);
    DLLSPEC bool Lock_TryLock(unsigned int LockID);
    DLLSPEC void Lock_Unlock(unsigned int LockID);
    DLLSPEC void Lock_GlobalLock();
    DLLSPEC bool Lock_GlobalTryLock();
    DLLSPEC void Lock_GlobalUnlock();
    DLLSPEC void Lock_ClearLocks();
    DLLSPEC unsigned int Graphics_CreateImage(int Width, int Height);
    DLLSPEC unsigned int Graphics_LoadImage(const char* FilePath);
    DLLSPEC unsigned int Graphics_ImageFromPointer(unsigned char* Pointer, int Width, int Height);
    DLLSPEC unsigned int Graphics_ImageFromCursor();
    DLLSPEC unsigned int Graphics_ImageFromWindow(HWND Window, int X, int Y, int Width, int Height);
    DLLSPEC void Graphics_DrawToWindow(unsigned int ImageID, HWND Window, int X, int Y, int Width, int Height, int SourceX, int SourceY, DWORD RasterType = SRCCOPY);
    DLLSPEC void Graphics_SaveImage(unsigned int ImageID, const char* FilePath);
    DLLSPEC void Graphics_DeleteImage(unsigned int ImageID);
    DLLSPEC unsigned int Graphics_GetPixel(unsigned int ImageID, int X, int Y);
    DLLSPEC CG::BGRA* Graphics_GetPixels(unsigned int ImageID, int &Width, int &Height);
    DLLSPEC void Graphics_SetPixel(unsigned int ImageID, unsigned int Colour, int X, int Y);
    DLLSPEC void Graphics_GetDimensions(unsigned int ImageID, int &Width, int &Height);
    DLLSPEC void Graphics_Clear();

	DLLSPEC int strcompare(const char* One, const char* Two, bool CaseSensitive = true);

    /*JNIEXPORT jint Java_natives_Riddler_quickHammingDifference(JNIEnv* env, jobject obj, jstring first, jstring second);
    JNIEXPORT jint Java_natives_Riddler_hammingDifference(JNIEnv* env, jobject obj, jstring first, jstring second);
    JNIEXPORT jint Java_natives_Riddler_levenshteinDistance(JNIEnv* env, jobject obj, jstring first, jstring second);
	JNIEXPORT jlong Java_natives_Riddler_GetDesktopWindowHandle(JNIEnv* env, jobject obj);
	JNIEXPORT void Java_natives__Riddler_GetWindowSize(JNIEnv* env, jobject obj, jlong WindowHandle, jint Width, jint Height);
	JNIEXPORT jlong Java_natives_Riddler_CreateMemoryMap(JNIEnv* env, jobject obj, jstring MapName, jlong size);
	JNIEXPORT jlong Java_natives_Riddler_OpenMemoryMap(JNIEnv* env, jobject obj, jstring MapName, jlong size);
	JNIEXPORT void Java_natives_Riddler_FlipImage(JNIEnv* env, jobject obj, jobject bytebuff, jint width, jint height, jint bpp);
	JNIEXPORT jint Java_natives_Riddler_MessageBoxTimeOut(JNIEnv* env, jobject obj, jlong hWnd, jstring lpText, jstring lpCaption, long uType, long dwMilliseconds);
	JNIEXPORT jlong Java_natives_Riddler_ReverseNumber(JNIEnv* env, jobject obj, jlong Number);
	JNIEXPORT jstring Java_natives_Riddler_GetEnvVariable(JNIEnv* env, jobject obj, jstring Variable);
	JNIEXPORT void Java_natives_Riddler_SetWindowTransparency(JNIEnv* env, jobject obj, jlong hwnd, jint Transparency);*/

#ifdef __cplusplus
}
#endif

#endif // RIDDLER_HPP_INCLUDED
