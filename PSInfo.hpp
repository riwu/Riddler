#ifndef PSINFO_HPP_INCLUDED
#define PSINFO_HPP_INCLUDED

#include <Windows.h>
#include <cstdint>

extern HINSTANCE hInstance;

static const char* Exports[] =
{
	"RemoveGC", "Function ReleaseGC(const ptr: Pointer): Boolean;",
	"RemoveGC", "Function ReleaseGC(const str: String): Boolean; overload;",
	"ErrorMessage", "Function ErrorMessage(Error: UInt32; Throw: Boolean = False): PChar;",
	"QuickHammingDistance", "Function QuickHammingDistance(const First, Second: String): UInt32;",
	"HammingDistance", "Function HammingDistance(const First, Second: String): UInt32;",
	"LevenshteinDistance", "Function LevenshteinDistance(const First, Second: String): UInt32;",
	"UTF8ToUTF16", "Function UTF8ToUTF16(const utf8: AnsiString): PWChar;",
	"UTF16ToUTF8", "Function UTF16ToUTF8(const utf16: UnicodeString): PChar;",
	"GetProcessModuleInfo", "Function GetProcessModuleInfo(const ProcessName: String): PChar;",
	"EncodeBase64", "Function EncodeBase64(const Data: String): PChar;",
	"DecodeBase64", "Function DecodeBase64(const Data: String): PChar;",
	"CRCGenerateTable", "Function CRCGenerateTable: PtrUInt;",
	"CRCLookUpTable", "Function CRCLookUpTable: PtrUInt;",
	"Adler32", "Function Adler32(const Data: Byte; Size: UInt32): UInt32;",
    "CRC32", "Function CRC32Checksum(const Data: Byte; Size, InitialValue: UInt32 = $EDB88320): UInt32;",
    "GetWindowHandle", "Function GetWindowHandle(const WindowName, WindowClass: String): PtrUInt;",
    "GetChildWindowHandle", "Function GetChildWindowHandle(Parent, Child: PtrUInt; const WindowName, ClassName: String): PtrUInt;",
    "GetDesktopWindowHandle", "Function GetDesktopWindowHandle: PtrUInt;",
    "GetWindowSize", "Procedure GetWindowSize(WindowHandle: PtrUInt; var Width, Height: Integer);",
    "GetDisplayDevices", "Function GetDisplayDevices: PChar;",
    "GetPrimaryDisplayDevice", "Function GetPrimaryDisplayDevice: DISPLAY_DEVICE;",
    "GetDisplaySettings", "Function GetDisplaySettings(Index: Integer): PChar;",
    "SetDefaultDisplaySettings", "Procedure SetDefaultDisplaySettings;",
    "SetDisplayResolution", "Function SetDisplayResolution(Width, Heignt: Integer): Boolean;",
    "CreateMemoryMap", "Function CreateMemoryMap(const MapName: String; Size: UInt32): Pointer;",
    "OpenMemoryMap", "Function OpenMemoryMap(const MapName: String; Size: UInt32): Pointer;",
    "CreateNewEvent", "Function CreateNewEvent(lpEventAttributes: ^SECURITY_ATTRIBUTES; bManualReset, bInitialState: Boolean; const EventName: String): PtrUInt;",
    "OpenSingleEvent", "Function OpenSingleEvent(const EventName: String; InheritHandle: Boolean; dwDesiredAccess: UInt32 = EVENT_ALL_ACCESS; dwMilliseconds: UInt32 = INFINITE): UInt32;",
    "SetEventSignal", "Function SetEventSignal(const EventName: String; Signaled: Boolean): Boolean;",
    "DeleteEvent", "Function DeleteEvent(const EventName: String): Boolean;",
    "FlipImage", "Procedure FlipImage(InPtr: Pointer; var OutPtr: Pointer; Width, Height: Integer; BitsPerPixel: UInt32);",
    "MessageBoxTimeOut", "Function MessageBoxTimeOut(Parent: PtrUInt; const lpText, lpCaption: String; uType, dwMilliseconds: UInt32; wLanguageID: Word = 0): Integer;",
    "ReverseNumber", "Function ReverseNumber(Number: Int64): Int64;",
    "GetEnvVariable", "Function GetEnvVariable(const Variable: String): PChar;",
    "SetEnvVariable", "Function SetEnvVariable(const VariableName, VariableValue: String): Boolean;",
    "QuickSort", "Function QSort(Base: Pointer; Num, Size: UInt32; Compar: Function(const a, b: Pointer): Integer): Integer;",
    "SetWindowTransparency", "Procedure SetWindowTransparency(Window: PtrUInt; Transparency: Byte);",
    "GetCurrentProcessID", "Function GetPID: Integer",
    "AppendWindowText", "Procedure AppendWindowText(WindowHandle: PtrUInt; const Text: String);",
    "WindowHandleFromPoint", "Function WindowHandleFromPoint(p: TPoint): PtrUInt;",
    "ChildWindowHandleFromPoint", "Function ChildWindowHandleFromPoint(Parent: PtrUInt; p: TPoint): PtrUInt;",
    "FindMainWindow", "Function FindMainWindow(PID: UInt32): PtrUInt;",
    "GetWindowParent", "Function GetWindowParent(Child: PtrUInt): PtrUInt;",
    "SetWindowParent", "Function SetWindowParent(Parent, Child: PtrUInt): PtrUInt;",
    "GetWindowDimensions", "Procedure GetWindowDimensions(WindowHandle: PtrUInt; var Area: TBox);",
    "GetPreviousWindowHandle", "Function GetPreviousWindow(WindowHandle: PtrUInt): PtrUInt;",
    "GetNextWindowHandle", "Function GetNextWindow(WindowHandle: PtrUInt): PtrUInt;",
    "GetProcessInfo", "Function GetProcessInfo(const ProcessName: String): PROCESSENTRY32;",
    "IsProcessRunning", "Function IsProcessRunning(const ProcessName: String): Boolean;",
    "ProcessInfoToString", "Function ProcessInfoToString(Proc32: PROCESSENTRY32): PChar;",
    "GetModulesInfo", "Function GetModulesInfo(ProcessID: UInt32): Array Of MODULEENTRY32;",
    "GetModuleInfo", "Function GetModuleInfo(ProcessID: UInt32; const ModuleName: String): MODULEENTRY32;",
    "ModuleInfoToString", "Function ModuleInfoToString(Mod32: MODULEENTRY32): PChar;",
    "RunApplication", "Function RunApplication(const ApplicationName, ApplicationArguments: String; WaitInputIdle: Boolean): Boolean;",
    "StartServiceEx", "Function StartService(const szServiceName: String): Boolean;",
    "StopServiceEx", "Function StopService(const szServiceName: String): Boolean;",
    "CreateRegistryKey", "Function CreateRegistryKey(Root: Pointer; const SubRoot, KeyName: String; KeyType: REG_KEY_TYPE; const InitialValue: String; samDesired: Integer = 0): Boolean;",
    "DeleteRegistryKey", "Function DeleteRegistryKey(Root: Pointer; const KeyToRemove: String; samDesired: Integer = 0): Boolean;",
    "DeleteRegistryValue", "Function DeleteRegistryValue(Root: Pointer; const SubRoot, KeyToRemove: String; samDesired: Integer = 0): Boolean;",
    "SetRegistryKey", "Function SetRegistryKey(Root: Pointer; const SubRoot, KeyToEdit, NewValue: String; KeyType: REG_KEY_TYPE; samDesired: Integer = 0): Boolean;",
    "ReadRegistryKey", "Function ReadRegistryKey(Root: PtrUInt; const SubRoot, KeyToRead: String; KeyType: REG_KEY_TYPE; var Str: array of char; samDesired: Integer = 0): Integer;",
    "GetDriveInfo", "Function GetDriveInfo(const Drive: String; var FreeSpace, TotalSpace: Word; InGigaBytes: Boolean = False): Boolean;",
    "GetTypeOfDrive", "Function GetTypeOfDrive(const Drive: String): PChar;",
    "GetLogicalDrivesList", "Function GetLogicalDrivesList(const Drives: array[0..25] of char): Integer;",
    "Thread_CreateThread", "Function CreateThread(FunctionToPass: Pointer): Integer;",
    "Thread_DetachThread", "Procedure DetachThread(ThreadID: Integer);",
    "Thread_JoinThread", "Procedure JoinThread(ThreadID: Integer);",
    "Thread_ClearThreads", "Procedure ClearThreads(Join: boolean);",
    "Thread_HardwareConcurrency", "Function ThreadHardwareConcurrency: Integer;",
    "Thread_NativeID", "Function NativeThreadID(ThreadID: Integer): Int64;",
    "Thread_SleepFor", "Procedure SleepThreadFor(Milliseconds: Integer);",
    "Thread_Yield", "Procedure YieldThread;",
    /*"Async_CreateAsync", "Function CreateAsync(FunctionToPass: Pointer): Integer;",
    "Async_ClearAsync", "Procedure ClearAsync;",
    "Async_Wait", "Procedure WaitAsync(AsyncID: Integer);",
    "Async_WaitFor", "Function WaitForAsync(AsyncID, Milliseconds: Integer): Boolean;",*/
    "Lock_CreateLock", "Function CreateLock: Integer;",
    "Lock_RemoveLock", "Procedure RemoveLock(LockID: Integer);",
    "Lock_Lock", "Procedure Lock(LockID: Integer);",
    "Lock_TryLock", "Function TryLock(LockID: Integer): Boolean;",
    "Lock_Unlock", "Procedure Unlock(LockID: Integer);",
    "Lock_GlobalLock", "Procedure GlobalLock;",
    "Lock_GlobalTryLock", "Function GlobalTryLock: Boolean;",
    "Lock_GlobalUnlock", "Procedure GlobalUnlock;",
    "Lock_ClearLocks", "Procedure ClearLocks;",
    "Graphics_CreateImage", "Function CreateImage(Width, Height: Integer): Integer;",
    "Graphics_LoadImage", "Function LoadImage(FilePath: String): Integer;",
    "Graphics_ImageFromPointer", "Function ImageFromPointer(Ptr: Pointer; Width, Height: Integer): Integer;",
    "Graphics_ImageFromCursor", "Function ImageFromCursor: Integer;",
    "Graphics_ImageFromWindow", "Function ImageFromWindow(WndHandle: PtrUInt; X, Y, Width, Height: Integer): Integer;",
    "Graphics_DrawToWindow", "Procedure DrawToWindow(ImageID: Integer; WndHandle: PtrUInt; X, Y, Width, Height, SourceX, SourceY: Integer = 0; RasterType: Integer = $00CC0020);",
    "Graphics_SaveImage", "Procedure SaveImage(ImageID: Integer; FilePath: String);",
    "Graphics_DeleteImage", "Procedure DeleteImage(ImageID: Integer);",
    "Graphics_GetPixel", "Function GetImagePixel(ImageID, X, Y: Integer): Integer;",
    "Graphics_GetPixels", "Function GetImagePixels(ImageID: Integer; var Width, Height: Integer): Pointer;",
    "Graphics_SetPixel", "Procedure SetImagePixel(ImageID, Colour, X, Y: Integer);",
    "Graphics_GetDimensions", "Procedure GetImageDimensions(ImageID: Integer; var Width, Height: Integer);",
    "Graphics_Clear", "Procedure ClearImages;"
};


static const char* Types[] =
{
    "PWChar", "^WideChar;",

    "DISPLAY_DEVICE", R"delimiter(record
                          cb: LongWord;
                          DeviceName: array[0..31] of char;
                          DeviceString: array[0..127] of char;
                          StateFlags: LongWord;
                          DeviceID: array[0..127] of char;
                          DeviceKey: array[0..127] of char;
                        end;)delimiter",

    "SECURITY_ATTRIBUTES", R"delimiter(record
                              nLength: UInt32;
                              lpSecurityDescriptor: Pointer;
                              bInheritHandle: Boolean;
                           end;)delimiter",


    "PROCESSENTRY32", R"delimiter(record
                          dwSize: UInt32;
                          cntUsage: UInt32;
                          th32ProcessID: UInt32;
                          th32DefaultHeapID: UInt64;
                          th32ModuleID: UInt32;
                          cntThreads: UInt32;
                          th32ParentProcessID: UInt32;
                          pcPriClassBase: LongInt;
                          dwFlags: UInt32;
                          szExeFile: array[0..259] of char;
                        end;)delimiter",

    "MODULEENTRY32", R"delimiter(record
                          dwSize: UInt32;
                          th32ModuleID: UInt32;
                          th32ProcessID: UInt32;
                          GlblcntUsage: UInt32;
                          ProccntUsage: UInt32;
                          modBaseAddr: ^Byte;
                          modBaseSize: UInt32;
                          hModule: Pointer;
                          szModule: array[0..255] of char;
                          szExePath: array[0..259] of char;
                        end;)delimiter",

    "EVENT_ALL_ACCESS", "Integer := $1F0003;",
    "INFINITE", "Integer := $FFFFFFFF;",
    "WAIT_ABANDONED", "Integer := $00000080;",
    "WAIT_OBJECT_0", "Integer := $00000000;",
    "WAIT_TIMEOUT", "Integer := $00000102;",

    "HKEY_CLASSES_ROOT", "Pointer := $80000000;",
    "HKEY_CURRENT_USER", "Pointer := $80000001;",
    "HKEY_LOCAL_MACHINE", "Pointer := $80000002;",
    "HKEY_USERS", "Pointer := $80000003;",
    "HKEY_PERFORMANCE_DATA", "Pointer := $80000004;",
    "HKEY_PERFORMANCE_TEXT", "Pointer := $80000050;",
    "HKEY_PERFORMANCE_NLSTEXT", "Pointer := $80000060;",
    "HKEY_CURRENT_CONFIG", "Pointer := $80000005;",
    "HKEY_DYN_DATA", "Pointer := $80000006;",

    "KEY_QUERY_VALUE", "Integer := $0001",
    "KEY_SET_VALUE", "Integer := $0002",
    "KEY_CREATE_SUB_KEY", "Integer := $0004",
    "KEY_ENUMERATE_SUB_KEYS", "Integer := $0008",
    "KEY_NOTIFY", "Integer := $0010",
    "KEY_CREATE_LINK", "Integer := $0020",
    "KEY_WOW64_32KEY", "Integer := $0200",
    "KEY_WOW64_64KEY", "Integer := $0100",
    "KEY_WOW64_RES", "Integer := $0300",
    "KEY_READ", "Integer := $00020019",
    "KEY_WRITE", "Integer := $00020006",
    "KEY_EXECUTE", "Integer := $00020019",
    "KEY_ALL_ACCESS", "Integer := $000F003F",

    "REG_KEY_TYPE", R"delimiter((REG_NONE, REG_SZ, REG_EXPAN_SZ,
                             REG_BINARY, REG_DWORD, REG_DWORD_BIG_ENDIAN,
                             REG_LINK, REG_MULTI_SZ, REG_RESOURCE_LIST,
                             REG_FULL_RESOURCE_DESCRIPTOR, REG_RESOURCE_REQUIREMENTS_LIST,
                             REG_QWORD);
                            )delimiter",

    "SRCCOPY", "Integer := $00CC0020;",
    "SRCPAINT", "Integer := $00EE0086;",
    "SRCAND", "Integer := $008800C6;",
    "SRCINVERT", "Integer := $00660046;",
    "SRCERASE", "Integer := $00440328;",
    "NOTSRCCOPY", "Integer := $00330008;",
    "NOTSRCERASE", "Integer := $001100A6;",
    "MERGECOPY", "Integer := $00C000CA;",
    "MERGEPAINT", "Integer := $00BB0226;",
    "PATCOPY", "Integer := $00F00021;",
    "PATPAINT", "Integer := $00FB0A09;",
    "PATINVERT", "Integer := $005A0049;",
    "DSTINVERT", "Integer := $00550009;",
    "BLACKNESS", "Integer := $00000042;",
    "WHITENESS", "Integer := $00FF0062;",
    "NOMIRRORBITMAP", "Integer := $80000000;",
    "CAPTUREBLT", "Integer := $40000000;"
};




#ifdef __cplusplus
extern "C"
{
#endif

	int __declspec(dllexport) GetPluginABIVersion();
	int __declspec(dllexport) GetFunctionCount();
	int __declspec(dllexport) GetTypeCount();
	int __declspec(dllexport) GetFunctionInfo(int Index, void* &Address, char* &Definition);
	int __declspec(dllexport) GetTypeInfo(int Index, char* &Type, char* &Definition);

#ifdef __cplusplus
}
#endif

#endif // PSINFO_HPP_INCLUDED
