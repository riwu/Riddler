#include "Riddler.hpp"

GC garbagecleaner;
Threads ThreadList;
Locks LockList;
ImageManager ImgManager;
Async AsyncList;

bool RemoveGC(const void* ptr)
{
    return garbagecleaner.Remove(ptr);
}

void Shrink_ImageGC()
{

}

const char* ErrorMessage(std::uint32_t Error, bool Throw)
{
	LPTSTR lpMsgBuf = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, Error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, nullptr);
	if (Throw)
	{
		std::string Buffer(lpMsgBuf);
		LocalFree(lpMsgBuf);
		throw std::runtime_error(std::string(Buffer.begin(), Buffer.end()));
	}

	PSString<char>* Buffer = garbagecleaner.Add(new PSString<char>(lpMsgBuf));
	LocalFree(lpMsgBuf);
	return Buffer->data();
}

std::uint32_t QuickHammingDistance(const char* First, const char* Second)
{
    int flen = strlen(First);
    int slen = strlen(Second);

	if (flen <= slen)
	{
		return std::inner_product(First, First + flen, Second, 0, std::plus<int>(), std::not_equal_to<char>());
	}

	return std::inner_product(Second, Second + slen, First, 0, std::plus<int>(), std::not_equal_to<char>());
}

std::uint32_t HammingDistance(const char* First, const char* Second)
{
	std::uint32_t mismatch = 0;
	std::size_t Length = std::min(strlen(First), strlen(Second));

	for (std::size_t I = 0; I < Length; ++I)
	{
		if (First[I] != Second[I])
		{
			++mismatch;
		}
	}

	return mismatch;
}

std::uint32_t LevenshteinDistance(const char* First, const char* Second)
{
	const std::size_t FirstLength = strlen(First);
	const std::size_t SecondLength = strlen(Second);

	std::vector<std::uint32_t> Current(SecondLength + 1);
	std::vector<std::uint32_t> Previous(SecondLength + 1);

	std::size_t I = 0;
	std::generate(Previous.begin(), Previous.end(), [&] {return I++; });

	for (I = 0; I < FirstLength; ++I)
	{
		Current[0] = I + 1;

		for (std::size_t J = 0; J < SecondLength; ++J)
		{
			auto Cost = First[I] == Second[J] ? 0 : 1;
			Current[J + 1] = std::min(std::min(Current[J] + 1, Previous[J + 1] + 1), Previous[J] + Cost);
		}

		Current.swap(Previous);
	}
	return Previous[SecondLength];
}

const wchar_t* UTF8ToUTF16(const char* utf8)
{
	PSString<wchar_t>* utf16 = new PSString<wchar_t>();
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, strlen(utf8), NULL, 0);
	if (len > 1)
	{
		utf16->resize(len);
		wchar_t* ptr = &utf16[0];
		MultiByteToWideChar(CP_UTF8, 0, utf8, strlen(utf8), ptr, len);
	}
	return &utf16[0];
}

const char* UTF16ToUTF8(const wchar_t* utf16)
{
	PSString<char>* utf8 = new PSString<char>();
	int len = WideCharToMultiByte(CP_UTF8, 0, utf16, wcslen(utf16), NULL, 0, 0, 0);
	if (len > 1)
	{
		utf8->resize(len);
		char* ptr = &utf8[0];
		WideCharToMultiByte(CP_UTF8, 0, utf16, wcslen(utf16), ptr, len, 0, 0);
	}
	return utf8->data();
}

const char* GetProcessModuleInfo(const char* Process)
{
	SIZE_T dwBytesRead = 0;
	PROCESS_BASIC_INFORMATION PBI = { 0 };
	PROCESSENTRY32 ProcessInfo = GetProcessInfo(Process);
	HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessInfo.th32ProcessID);

	typedef NTSTATUS NTAPI(*pNtQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
	pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationProcess");


    PSString<char>* result = garbagecleaner.Add(new PSString<char>());
    auto to_hex_string = [](std::size_t val, std::ios_base &(*f)(std::ios_base&)) -> std::string
	{
		std::stringstream oss;
		oss << std::hex << std::uppercase << val;
		return oss.str();
	};

	if (NT_SUCCESS(NtQIP(ProcessHandle, ProcessBasicInformation, &PBI, sizeof(PBI), reinterpret_cast<DWORD*>(&dwBytesRead))))
	{
		PEB_LDR_DATA LdrData;
		LDR_MODULE LdrModule;
		PPEB_LDR_DATA pLdrData = nullptr;

		char* LdrDataOffset = reinterpret_cast<char*>(PBI.PebBaseAddress) + offsetof(PEB, Ldr);
		ReadProcessMemory(ProcessHandle, LdrDataOffset, &pLdrData, sizeof(pLdrData), &dwBytesRead);
		ReadProcessMemory(ProcessHandle, pLdrData, &LdrData, sizeof(LdrData), &dwBytesRead);

		LIST_ENTRY* Head = LdrData.InMemoryOrderModuleList.Flink;
		LIST_ENTRY* Node = Head;

		do
		{
			if (ReadProcessMemory(ProcessHandle, reinterpret_cast<char*>(Node) - sizeof(LIST_ENTRY), &LdrModule, sizeof(LdrModule), &dwBytesRead))
			{
				if (LdrModule.BaseAddress)
				{
					std::wstring BaseDllName(LdrModule.BaseDllName.Length / sizeof(wchar_t), 0);
					std::wstring FullDllName(LdrModule.FullDllName.Length / sizeof(wchar_t), 0);
					ReadProcessMemory(ProcessHandle, LdrModule.BaseDllName.Buffer, &BaseDllName[0], LdrModule.BaseDllName.Length, &dwBytesRead);
					ReadProcessMemory(ProcessHandle, LdrModule.FullDllName.Buffer, &FullDllName[0], LdrModule.FullDllName.Length, &dwBytesRead);

					result->append("BaseAddress:      ").append(to_hex_string(reinterpret_cast<std::size_t>(LdrModule.BaseAddress), std::hex).c_str()).append("\r\n");
					result->append("Entry-Point:      ").append(to_hex_string(reinterpret_cast<std::size_t>(LdrModule.EntryPoint), std::hex).c_str()).append("\r\n");
					const char* tmp = UTF16ToUTF8(BaseDllName.c_str());
					result->append("Base DLL-Name:   ").append(tmp).append("\r\n");
					garbagecleaner.Remove(tmp);

                    tmp = UTF16ToUTF8(FullDllName.c_str());
					result->append("Full DLL-Name:   ").append(tmp).append("\r\n");
					garbagecleaner.Remove(tmp);

                    result->append("Reference Count:  ").append(std::to_string(LdrModule.LoadCount).c_str()).append("\r\n");
					result->append("Image Size:       ").append(std::to_string(LdrModule.SizeOfImage).c_str()).append("\r\n");
					result->append("Time Date Stamp:  ").append(std::to_string(LdrModule.TimeDateStamp).c_str()).append("\r\n");
					result->append("\r\n");
				}
			}

			Node = LdrModule.InMemoryOrderModuleList.Flink;
		} while (Head != Node);
	}

	if (!ProcessHandle)
        garbagecleaner.Remove(result);

	CloseHandle(ProcessHandle);

	return result->data();
}

const char* EncodeBase64(const char* Data)
{
	std::string Binary = std::string();
	PSString<char>* Result = garbagecleaner.Add(new PSString<char>());
	int len = strlen(Data);

	for (int I = 0; I < len; ++I)
	{
		Binary += std::bitset<8>(Data[I]).to_string();
	}

	for (std::size_t I = 0; I < Binary.size(); I += !I ? 7 : 6)
	{
		std::string temp = Binary.substr(I >= 1 ? I - 1 : 0);
		temp.resize(6, '0');
		Result->operator+=(Base64Chars[std::bitset<6>(temp).to_ulong()]);
	}

	if (Result->size() % 4)
	{
		Result->resize(Result->size() + 4 - Result->size() % 4, '=');
	}

	return Result->data();
}

const char* DecodeBase64(const char* Data)
{
	#ifdef _MSC_VER
	#pragma warning(disable: 4996)
	#endif

	std::string Binary = std::string();
	PSString<char>* Result = garbagecleaner.Add(new PSString<char>());
	int len = strlen(Data);

	for (std::size_t I = len; I > 0; --I)
	{
		if (Data[I - 1] != '=')
		{
			std::string Characters(I, '0');
			strncpy(&Characters[0], Data, I);
			for (auto it = Characters.begin(); it != Characters.end(); ++it)
			{
				Binary += std::bitset<6>(Base64Chars.find(*it)).to_string();
			}
			break;
		}
	}

	for (std::size_t I = 0; I < Binary.size(); I += !I ? 9 : 8)
	{
		Result->operator+=(static_cast<char>(std::bitset<8>(Binary.substr(I >= 1 ? I - 1 : 0)).to_ulong()));
	}

	return Result->data();
}

std::uint32_t* CRCGenerateTable()
{
	static std::uint32_t LookUpTable[256];
	for (int I = 0; I < 256; ++I)
	{
		std::uint32_t Value = I;
		for (int K = 0; K < 8; ++K)
		{
			Value = Value & 1 ? 0xEDB88320 ^ (Value >> 1) : Value >> 1;
		}
		LookUpTable[I] = Value;
	}
	return &LookUpTable[0];
}

const std::uint32_t* CRCLookUpTable()
{
	static const std::uint32_t LookUpTable[256] =
	{
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
		0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
		0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
		0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
		0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
		0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
		0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
		0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
		0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
		0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
		0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
		0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
		0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
		0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
		0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
		0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
		0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
		0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
		0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
		0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
		0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
		0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
		0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
		0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
		0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
		0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
		0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
		0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
		0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
		0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
		0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
		0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
		0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
		0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
		0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
	};
	return LookUpTable;
}

std::uint32_t Adler32(std::uint8_t* Data, std::uint32_t Size)
{
	std::uint32_t A = 1, B = 0;
	for (std::uint32_t I = 0; I < Size; ++I)
	{
		A = (A + Data[I]) % 0xFFF1;
		B = (B + A) % 0xFFF1;
	}
	return (B << 0x10) | A;
}

std::uint32_t CRC32(std::uint8_t* Data, std::size_t Size, std::uint32_t InitialValue)
{
	std::uint32_t CRC = ~InitialValue;
	const std::uint32_t* LookUpTable = CRCLookUpTable();
	std::uint8_t* Pointer = static_cast<std::uint8_t*>(Data);

	for (std::size_t I = 0; I < Size; ++I)
	{
		CRC = LookUpTable[(CRC ^ *Pointer++) & 0xFF] ^ (CRC >> 8);
	}

	return ~CRC;
}

int strcompare(const char* One, const char* Two, bool CaseSensitive)
{
	#if defined _WIN32 || defined _WIN64
	return CaseSensitive ? strcmp(One, Two) : _stricmp(One, Two);
	#else
	return CaseSensitive ? strcmp(One, Two) : strcasecmp(One, Two);
	#endif
}

HWND GetWindowHandle(const char* WindowName, const char* WindowClass)
{
    return FindWindow(WindowName, WindowClass);
}

HWND GetChildWindowHandle(HWND Parent, HWND ChildAfter, const char* WindowName, const char* WindowClass)
{
    return FindWindowEx(Parent, ChildAfter, WindowClass, WindowName);
}

HWND GetDesktopWindowHandle()
{
	return GetDesktopWindow();
}

void GetWindowSize(HWND WindowHandle, int &Width, int &Height)
{
	RECT Rect = { 0 };
	GetWindowRect(WindowHandle, &Rect);
	Width = Rect.right - Rect.left;
	Height = Rect.bottom - Rect.top;
}

const char* GetDisplayDevices()
{
	int index = 0;
	DISPLAY_DEVICE dd = { 0 };
	dd.cb = sizeof(DISPLAY_DEVICE);
	PSString<char>* str = garbagecleaner.Add(new PSString<char>());

	while (EnumDisplayDevices(NULL, index++, &dd, 0))
	{
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			str->append("*");

		str->append(dd.DeviceName).append(", ").append(dd.DeviceString).append("\r\n");
	}
	return str->data();
}

DISPLAY_DEVICE GetPrimaryDisplayDevice()
{
	int index = 0;
	DISPLAY_DEVICE dd = { 0 };
	dd.cb = sizeof(DISPLAY_DEVICE);

	while (EnumDisplayDevices(NULL, index++, &dd, 0))
	{
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			return dd;
	}
	return dd;
}

const char* GetDisplaySettings(int Index)
{
	DISPLAY_DEVICE dd = { 0 };
	dd.cb = sizeof(DISPLAY_DEVICE);

	if (EnumDisplayDevices(NULL, Index, &dd, 0))
	{
		DISPLAY_DEVICE monitor = { 0 };
		monitor.cb = sizeof(DISPLAY_DEVICE);
		if (EnumDisplayDevices(dd.DeviceName, Index, &monitor, 0))
		{
			DEVMODE dm = { 0 };
			dm.dmSize = sizeof(DEVMODE);

			if (EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
			{
				PSString<char>* str = garbagecleaner.Add(new PSString<char>());
				str->append("Device name:       ").append(dd.DeviceName).append("\r\n");
				str->append("Monitor name:      ").append(monitor.DeviceID).append("\r\n");
				str->append("Refresh rate:      ").append(std::to_string(dm.dmDisplayFrequency).c_str()).append("\r\n");
				str->append("Colour depth:      ").append(std::to_string(dm.dmBitsPerPel).c_str()).append("\r\n");
				str->append("Screen resolution: ").append(std::to_string(dm.dmPelsWidth).c_str()).append(" x ").append(std::to_string(dm.dmPelsHeight).c_str()).append("\r\n");
				return str->data();
			}
		}
	}
	return nullptr;
}

bool SetDefaultDisplaySettings()
{
	return ChangeDisplaySettings(nullptr, 0) == DISP_CHANGE_SUCCESSFUL;
}

bool SetDisplayResolution(int width, int height)
{
	DISPLAY_DEVICE dd = GetPrimaryDisplayDevice();
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(DEVMODE);

	if (EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
	{
		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;
		dm.dmFields = (DM_PELSWIDTH | DM_PELSHEIGHT);

		if (ChangeDisplaySettings(&dm, CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
		{
			return false;
		}

		return (ChangeDisplaySettings(&dm, 0) == DISP_CHANGE_SUCCESSFUL);
	}

	return false;
}

void* CreateMemoryMap(const char* MapName, unsigned int size)
{
	#if defined _WIN32 || defined _WIN64
	typedef struct { void* hFileMap; void* pData; std::size_t size; } data;
	data* info = new data();
	info->hFileMap = nullptr;
	info->pData = nullptr;
	info->size = 0;

	if ((info->hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, MapName)) == nullptr)
	{
		return nullptr;
	}

	if ((info->pData = MapViewOfFile(info->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, size)) == nullptr)
	{
		CloseHandle(info->hFileMap);
		delete info;
		return nullptr;
	}

	#else

	typedef struct { int hFileMap = nullptr; void* pData = nullptr; std::size_t size } data;
	data* info = new data();
	info->hFileMap = nullptr;
	info->pData = nullptr;
	info->size = 0;

	if ((info->hFileMap = open(MapName, O_RDWR | O_CREAT, 438)) == -1)
	{
		return nullptr;
	}

	if ((info->pData = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, hFileMap, 0)) == MAP_FAILED)
	{
		close(hFileMap);
		delete info;
		return nullptr;
	}
	#endif

	info->size = size;

	garbagecleaner.Add<PSBaseObject>(new PSNativeObject<data>(info), [&](PSBaseObject* ptr){
		PSNativeObject<data>* obj = static_cast<PSNativeObject<data>*>(ptr);

		#if defined _WIN32 || defined _WIN64
		UnmapViewOfFile(static_cast<const data*>(obj->address())->pData);
		CloseHandle(static_cast<const data*>(obj->address())->hFileMap);
		delete obj;
		#else
		munmap(static_cast<const data*>(obj->address())->(pData, size));
		close(static_cast<const data*>(obj->address())->hFileMap);
		delete obj;
		#endif

	});
	return info->pData;
}

void* OpenMemoryMap(const char* MapName, unsigned int size)
{
	#if defined _WIN32 || defined _WIN64
	typedef struct { void* hFileMap; void* pData; std::size_t size; } data;
	data* info = new data();
	info->hFileMap = nullptr;
	info->pData = nullptr;
	info->size = 0;

	if ((info->hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, MapName)) == nullptr)
	{
		return nullptr;
	}

	if ((info->pData = MapViewOfFile(info->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, size)) == nullptr)
	{
		CloseHandle(info->hFileMap);
		delete info;
		return nullptr;
	}

	#else

	typedef struct { void* hFileMap; void* pData; std::size_t size; } data;
	data* info = new data();
	info->hFileMap = nullptr;
	info->pData = nullptr;
	info->size = 0;

	if ((info->hFileMap = open(MapName, O_RDWR | O_CREAT, 438)) == -1)
	{
		return nullptr;
	}

	if ((info->pData = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, hFileMap, 0)) == MAP_FAILED)
	{
		close(info->hFileMap);
		delete info;
		return nullptr;
	}
	#endif

	info->size = size;
	garbagecleaner.Add<PSBaseObject>(new PSNativeObject<data>(info), [&](PSBaseObject* ptr){
		PSNativeObject<data>* obj = static_cast<PSNativeObject<data>*>(ptr);

		#if defined _WIN32 || defined _WIN64
		UnmapViewOfFile(static_cast<const data*>(obj->address())->pData);
		CloseHandle(static_cast<const data*>(obj->address())->hFileMap);
		delete obj;
		#else
		munmap(static_cast<const data*>(obj->address())->(pData, size));
		close(static_cast<const data*>(obj->address())->hFileMap);
		delete obj;
		#endif

	});
	return info->pData;
}

HANDLE CreateNewEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, bool bManualReset, bool bInitialState, const char* EventName)
{
	HANDLE h = CreateEvent(lpEventAttributes, bManualReset, bInitialState, EventName);
	auto ptr = garbagecleaner.Add<PSBaseObject>(reinterpret_cast<PSBaseObject*>(h), [](PSBaseObject* ptr){ CloseHandle(ptr); });
	return ptr;
}

std::uint32_t OpenSingleEvent(const char* EventName, bool InheritHandle, std::uint32_t dwDesiredAccess, std::uint32_t dwMilliseconds)
{
	void* hEvent = OpenEvent(dwDesiredAccess, InheritHandle, EventName);
	if (hEvent)
	{
		std::uint32_t Result = WaitForSingleObject(hEvent, dwMilliseconds);
		CloseHandle(hEvent);
		return Result;
	}
	return WAIT_FAILED;
}

bool SetEventSignal(void* Event, bool Signaled)
{
	if (Event && Signaled)
	{
		return SetEvent(Event) != 0;
	}
	return ResetEvent(Event) != 0;
}

bool DeleteEvent(void* Event)
{
	return CloseHandle(Event) != 0;
}

void FlipImage(void* In, void* &Out, int width, int height, unsigned int Bpp)
{
	unsigned long Chunk = (Bpp > 24 ? width * 4 : width * 3 + width % 4);
	unsigned char* Destination = static_cast<unsigned char*>(Out);
	unsigned char* Source = static_cast<unsigned char*>(In) + Chunk * (height - 1);

	while (Source != In)
	{
		std::memcpy(Destination, Source, Chunk);
		Destination += Chunk;
		Source -= Chunk;
	}
}

int MessageBoxTimeOut(HWND hWnd, const char* lpText, const char* lpCaption, std::uint32_t uType, std::uint32_t dwMilliseconds, std::uint16_t wLanguageId)
{
	typedef int(__stdcall * MSGBOXAAPI)(IN HWND hWnd, IN const char* lpText, IN const char* lpCaption, IN std::uint32_t uType, IN std::uint16_t wLanguageId, IN std::uint32_t dwMilliseconds);
	static MSGBOXAAPI MsgBoxTOA = nullptr;

	if (!MsgBoxTOA)
	{
		MsgBoxTOA = reinterpret_cast<MSGBOXAAPI>(GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxTimeoutA"));
	}

	return MsgBoxTOA ? MsgBoxTOA(hWnd, lpText, lpCaption, uType, wLanguageId, dwMilliseconds) : 0;
}

std::int64_t ReverseNumber(std::int64_t Number)
{
	int Sum = 0;
	while (Number)
	{
		Sum *= 10;
		Sum += Number % 10;
		Number /= 10;
	}
	return Sum;
}

const char* GetEnvVariable(const char* Variable)
{
	#ifdef _MSC_VER
	#pragma warning(disable: 4996)
	#endif
	return garbagecleaner.Add(new PSString<char>(getenv(Variable)))->data();
}

bool SetEnvVariable(const char* VariableName, const char* VariableValue)
{
    return SetEnvironmentVariable(VariableName, VariableValue);
}

void QuickSort(void* base, size_t num, size_t size, int(*compar)(const void*, const void*))
{
	qsort(base, num, size, compar);
}

void SetWindowTransparency(HWND hwnd, std::uint8_t Transperancy)
{
	long wAttr = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, wAttr | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, Transperancy, 2);
}

int GetCurrentProcessID()
{
    return getpid();
}

void AppendWindowText(HWND WindowHandle, const char* Text)
{
    SendMessage(WindowHandle, EM_SETSEL, -1, -1);
    SendMessage(WindowHandle, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(Text));
}

HWND WindowHandleFromPoint(POINT point)
{
    return WindowFromPoint(point);
}

HWND ChildWindowHandleFromPoint(HWND WindowHandle, POINT point)
{
    return ChildWindowFromPoint(WindowHandle, point);
}

bool IsMainWindow(HWND WindowHandle)
{
    return !GetWindow(WindowHandle, GW_OWNER) && IsWindowVisible(WindowHandle);
}

BOOL CALLBACK EnumWindowsProc(HWND WindowHandle, LPARAM lParam)
{
    unsigned long PID = 0;
    WindowHandleStructure* data = reinterpret_cast<WindowHandleStructure*>(lParam);

    GetWindowThreadProcessId(WindowHandle, &PID);
    if (data->PID != PID || !IsMainWindow(WindowHandle))
    {
        return TRUE;
    }
    data->WindowHandle = WindowHandle;
    return FALSE;
}

HWND FindMainWindow(unsigned long PID)
{
    WindowHandleStructure data = {PID, nullptr};
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.WindowHandle;
}

HWND GetWindowParent(HWND Child)
{
	return GetParent(Child);
}

HWND SetWindowParent(HWND Parent, HWND Child)
{
	return SetParent(Child, Parent);
}

void GetWindowDimensions(HWND WindowHandle, RECT &rect)
{
    GetWindowRect(WindowHandle, &rect);
}

HWND GetPreviousWindowHandle(HWND WindowHandle)
{
    return GetNextWindow(WindowHandle, GW_HWNDPREV);
}

HWND GetNextWindowHandle(HWND WindowHandle)
{
    return GetNextWindow(WindowHandle, GW_HWNDNEXT);
}

PROCESSENTRY32 GetProcessInfo(const char* ProcessName)
{
	void* hSnap = nullptr;
	PROCESSENTRY32 Proc32 = { 0 };

	if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
		return Proc32;

	Proc32.dwSize = sizeof(PROCESSENTRY32);
	while (Process32Next(hSnap, &Proc32))
	{
		if (!strcompare(ProcessName, Proc32.szExeFile, false))
		{
			CloseHandle(hSnap);
			return Proc32;
		}
	}
	CloseHandle(hSnap);
	Proc32 = { 0 };
	return Proc32;
}

bool IsProcessRunning(const char* pProcessName)
{
	return (GetProcessInfo(pProcessName).th32ProcessID != 0);
}

const char* ProcessInfoToString(PROCESSENTRY32 Proc32)
{
	if (Proc32.th32ProcessID != 0)
	{
		PSString<char>* str = garbagecleaner.Add(new PSString<char>());
		str->append("  =======================================================\r\n");
		str->append("  Process Name:            ").append(Proc32.szExeFile).append("\r\n");
		str->append("  =======================================================\r\n\r\n");
		str->append("  Process ID:              ").append(std::to_string(Proc32.th32ProcessID).c_str()).append("\r\n");
		str->append("  Thread Count:            ").append(std::to_string(Proc32.cntThreads).c_str()).append("\r\n");
		str->append("  Priority Base:           ").append(std::to_string(Proc32.pcPriClassBase).c_str()).append("\r\n");
		str->append("  Parent Process ID:       ").append(std::to_string(Proc32.th32ParentProcessID).c_str()).append("\r\n\r\n");
		str->append("  =======================================================\r\n");
		return str->data();
	}
	return nullptr;
}

MODULEENTRY32* GetModulesInfo(std::uint32_t ProcessID)
{
	void* hSnap = nullptr;
	MODULEENTRY32 Mod32 = { 0 };

	PSArray<MODULEENTRY32>* Modules = garbagecleaner.Add<PSArray<MODULEENTRY32>>(new PSArray<MODULEENTRY32>());

	if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID)) == INVALID_HANDLE_VALUE)
		return nullptr;

	Mod32.dwSize = sizeof(MODULEENTRY32);
	while (Module32Next(hSnap, &Mod32))
	{
		Modules->push_back(Mod32);
	}
	CloseHandle(hSnap);
	return Modules->data();
}

MODULEENTRY32 GetModuleInfo(std::uint32_t ProcessID, const char* ModuleName)
{
	void* hSnap = nullptr;
	MODULEENTRY32 Mod32 = { 0 };

	if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID)) == INVALID_HANDLE_VALUE)
		return Mod32;

	Mod32.dwSize = sizeof(MODULEENTRY32);
	while (Module32Next(hSnap, &Mod32))
	{
		if (!strcompare(ModuleName, Mod32.szModule, false))
		{
			CloseHandle(hSnap);
			return Mod32;
		}
	}

	CloseHandle(hSnap);
	Mod32 = { 0 };
	return Mod32;
}

const char* ModuleInfoToString(MODULEENTRY32 Mod32)
{
	auto to_hex_string = [](std::size_t val, std::ios_base &(*f)(std::ios_base&)) -> std::string
	{
		std::stringstream oss;
		oss << std::hex << std::uppercase << val;
		return oss.str();
	};

	PSString<char>* str = garbagecleaner.Add(new PSString<char>());
	str->append("  =======================================================\r\n");
	str->append("  Module Name:             ").append(Mod32.szModule).append("\r\n");
	str->append("  =======================================================\r\n\r\n");
	str->append("  Module Path:             ").append(Mod32.szExePath).append("\r\n");
	str->append("  Process ID:              ").append(std::to_string(Mod32.th32ProcessID).c_str()).append("\r\n");
	str->append("  Load Count (Global):     ").append(std::to_string(static_cast<int>(Mod32.GlblcntUsage != 0xFFFF ? Mod32.GlblcntUsage : -1)).c_str()).append("\r\n");
	str->append("  Load Count (Process):    ").append(std::to_string(static_cast<int>(Mod32.ProccntUsage != 0xFFFF ? Mod32.ProccntUsage : -1)).c_str()).append("\r\n");
	str->append("  Base Address:            0x").append(to_hex_string(reinterpret_cast<std::size_t>(Mod32.modBaseAddr), std::hex).c_str()).append("\r\n");
	str->append("  Base Size:               0x").append(to_hex_string(Mod32.modBaseSize, std::hex).c_str()).append("\r\n\r\n");
	str->append("  =======================================================\r\n");
	return str->data();
}

bool RunApplication(const char* ApplicationName, char* ApplicationArguments, bool WaitInputIdle)
{
	bool Result = false;
	STARTUPINFO         siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;

	std::memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	std::memset(&piProcessInfo, 0, sizeof(piProcessInfo));

	if (CreateProcessA(ApplicationName, &ApplicationArguments[0], nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &siStartupInfo, &piProcessInfo) != false)
		Result = true;

	if (WaitInputIdle)
	{
		Result = false;
		if (WaitForInputIdle(piProcessInfo.hProcess, 10000) == 0)
			Result = true;
	}
	CloseHandle(piProcessInfo.hThread);
	CloseHandle(piProcessInfo.hProcess);
	return Result;
}

bool StartServiceEx(const char* szServiceName)
{
	SC_HANDLE schService;
	SC_HANDLE schSCManager;
	SERVICE_STATUS ssStatus;
	bool bRet = false;
	int iCont = 0;

	schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	if (schSCManager)
	{
		schService = OpenService(schSCManager, szServiceName, SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (StartService(schService, 0, nullptr))
			{
				Sleep(1000);
				while (QueryServiceStatus(schService, &ssStatus))
				{
					++iCont;
					if (ssStatus.dwCurrentState == SERVICE_START_PENDING)
					{
						Sleep(1000);
						if (iCont > 10)
							break;
					}
					else
						break;
				}

				if (ssStatus.dwCurrentState == SERVICE_RUNNING)
					bRet = true;
				else
					bRet = false;
			}
			CloseServiceHandle(schService);
		}
		else
			bRet = true;
		CloseServiceHandle(schSCManager);
	}
	else
		bRet = false;
	return bRet;
}

bool StopServiceEx(const char* szServiceName)
{
	SC_HANDLE schService;
	SC_HANDLE schSCManager;
	SERVICE_STATUS ssStatus;
	bool bRet = true;
	int iCont = 0;

	schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	if (schSCManager)
	{
		schService = OpenService(schSCManager, szServiceName, SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus))
			{
				Sleep(1000);
				while (QueryServiceStatus(schService, &ssStatus))
				{
					++iCont;
					if (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
					{
						Sleep(1000);
						if (iCont > 10)
							break;
					}
					else
						break;
				}

				if (ssStatus.dwCurrentState == SERVICE_STOPPED)
					bRet = true;
				else
					bRet = false;
			}
			CloseServiceHandle(schService);
		}
		else
			bRet = true;
		CloseServiceHandle(schSCManager);
	}
	else
		bRet = false;
	return bRet;
}

bool CreateRegistryKey(HKEY Root, const char* SubRoot, const char* KeyName, DWORD KeyType, const char* InitialValue, REGSAM samDesired)
{
	HKEY hKey;
	DWORD dwError = ERROR_CANTOPEN;
	if ((dwError = RegOpenKeyEx(Root, SubRoot, 0, KEY_ALL_ACCESS | samDesired, &hKey)) != ERROR_SUCCESS)
	{
		dwError = ERROR_CANTOPEN;
		DWORD Disposition;
		dwError = RegCreateKeyEx(Root, SubRoot, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, &Disposition);
		dwError = (Disposition != 0) ? Disposition : dwError;
	}

	if ((dwError == ERROR_SUCCESS) || (dwError == REG_CREATED_NEW_KEY) || (dwError == REG_OPENED_EXISTING_KEY))
	{
		if (!InitialValue || !strlen(InitialValue))
		{
			dwError = ERROR_CANTWRITE;
			size_t Size = sizeof(DWORD);
			switch (KeyType)
			{
				case REG_DWORD:
				{
					Size = sizeof(DWORD);
					DWORD Value = std::stoul(InitialValue);
					dwError = RegSetValueEx(hKey, KeyName, 0, KeyType, reinterpret_cast<const std::uint8_t*>(&Value), Size);
				}
				break;

				case REG_QWORD:
				{
					Size = sizeof(std::uint64_t);
					std::uint64_t Value = std::stoull(InitialValue);
					dwError = RegSetValueEx(hKey, KeyName, 0, KeyType, reinterpret_cast<const std::uint8_t*>(&Value), Size);
				}
				break;

				case REG_MULTI_SZ:
				{
					std::vector<char> Buffer(Size);
					for (size_t I = 0; I < Buffer.size(); I++)
						Buffer[I] = InitialValue[I] != 0xA ? InitialValue[I] : 0;
					dwError = RegSetValueEx(hKey, KeyName, 0, KeyType, reinterpret_cast<const std::uint8_t*>(Buffer.data()), Buffer.size());
				}
				break;

				case REG_SZ:
				case REG_EXPAND_SZ:
				{
					Size = strlen(InitialValue);
					dwError = RegSetValueEx(hKey, KeyName, 0, KeyType, reinterpret_cast<const std::uint8_t*>(InitialValue), Size);
				}
				break;

				default:
					dwError = ERROR_CANTWRITE;
				break;
			}
		}
		RegCloseKey(hKey);
	}

	return (!InitialValue || !strlen(InitialValue)) ? (dwError == REG_CREATED_NEW_KEY) || (dwError == REG_OPENED_EXISTING_KEY) || (dwError == ERROR_SUCCESS) : (dwError == ERROR_SUCCESS);
}

bool DeleteRegistryKey(HKEY Root, const char* KeyToRemove, REGSAM samDesired)
{
	DWORD dwError = ERROR_CANTOPEN;
	if ((dwError = samDesired ? RegDeleteKeyEx(Root, KeyToRemove, samDesired, 0) : RegDeleteKey(Root, KeyToRemove)) == ERROR_SUCCESS)
		RegCloseKey(Root);

	return (dwError == ERROR_SUCCESS);
}

bool DeleteRegistryValue(HKEY Root, const char* SubRoot, const char* KeyToRemove, REGSAM samDesired)
{
	HKEY hKey;
	DWORD dwError = ERROR_CANTOPEN;
	if ((dwError = RegOpenKeyEx(Root, SubRoot, 0, KEY_ALL_ACCESS | samDesired, &hKey)) == ERROR_SUCCESS)
	{
		dwError = ERROR_CANTOPEN;
		dwError = RegDeleteValue(hKey, KeyToRemove);
		RegCloseKey(hKey);
	}

	return (dwError == ERROR_SUCCESS);
}

bool SetRegistryKey(HKEY Root, const char* SubRoot, const char* KeyToEdit, const char* NewValue, DWORD KeyType, REGSAM samDesired)
{
	HKEY hKey;
	DWORD dwError = ERROR_CANTOPEN;
	if ((dwError = RegOpenKeyEx(Root, SubRoot, 0, KEY_ALL_ACCESS | samDesired, &hKey)) == ERROR_SUCCESS)
	{
		dwError = ERROR_CANTWRITE;
		switch (KeyType)
		{
			case REG_DWORD:
			{
				DWORD dwSize = sizeof(DWORD);
				DWORD Data = std::stoul(NewValue);
				dwError = RegSetValueEx(hKey, KeyToEdit, 0, KeyType, reinterpret_cast<const std::uint8_t*>(&Data), dwSize);
			}
			break;

			case REG_QWORD:
			{
				DWORD dwSize = sizeof(std::uint64_t);
				std::uint64_t Data = std::stoull(NewValue);
				dwError = RegSetValueEx(hKey, KeyToEdit, 0, KeyType, reinterpret_cast<const std::uint8_t*>(&Data), dwSize);
			}
			break;

			case REG_MULTI_SZ:
			{
				std::vector<char> Buffer(strlen(NewValue));
				for (std::size_t I = 0; I < Buffer.size(); ++I)
					Buffer[I] = NewValue[I] != 0xA ? NewValue[I] : 0;
				dwError = RegSetValueEx(hKey, KeyToEdit, 0, KeyType, reinterpret_cast<const std::uint8_t*>(Buffer.data()), Buffer.size());
			}
			break;

			case REG_SZ:
			case REG_EXPAND_SZ:
			{
				dwError = RegSetValueEx(hKey, KeyToEdit, 0, KeyType, reinterpret_cast<const std::uint8_t*>(NewValue), strlen(NewValue));
			}
			break;

			default:
				dwError = ERROR_CANTWRITE;
			break;
		}
		RegCloseKey(hKey);
	}

	return (dwError == ERROR_SUCCESS);
}

int ReadRegistryKey(HKEY Root, const char* SubRoot, const char* KeyToRead, DWORD KeyType, char* &Result, REGSAM samDesired)
{
	#ifdef _MSC_VER
	#pragma warning(disable: 4996)
	#endif

	HKEY hKey;
	DWORD dwError = ERROR_CANTOPEN;
	if ((dwError = RegOpenKeyEx(Root, SubRoot, 0, KEY_READ | samDesired, &hKey)) == ERROR_SUCCESS)
	{
		DWORD dwSize = 1024;
		dwError = ERROR_CANTREAD;

		if (KeyType == 0)
		{
			char TempData[255] = { 0 };
			dwSize = sizeof(TempData) / sizeof(char);
			dwError = RegQueryValueEx(hKey, KeyToRead, nullptr, &KeyType, reinterpret_cast<std::uint8_t*>(TempData), &dwSize);
			KeyType = (dwError == ERROR_SUCCESS) ? KeyType : 0;
		}

		if (KeyType != 0)
		{
			switch (KeyType)
			{
				case REG_DWORD:
				{
					dwSize = sizeof(DWORD);
					DWORD Data = 0;
					dwError = RegQueryValueEx(hKey, KeyToRead, nullptr, &KeyType, reinterpret_cast<std::uint8_t*>(&Data), &dwSize);
					strcpy(Result, std::to_string(Data).c_str());
				}
				break;

				case REG_QWORD:
				{
					dwSize = sizeof(std::uint64_t);
					std::uint64_t Data = 0;
					dwError = RegQueryValueEx(hKey, KeyToRead, nullptr, &KeyType, reinterpret_cast<std::uint8_t*>(&Data), &dwSize);
					strcpy(Result, std::to_string(Data).c_str());
				}
				break;

				case REG_MULTI_SZ:
				{
					std::vector<char> Data(dwSize - 1);
					dwError = RegQueryValueEx(hKey, KeyToRead, nullptr, &KeyType, reinterpret_cast<std::uint8_t*>(Data.data()), &dwSize);
					for (std::size_t I = 0; I < Data.size(); ++I)
						Result[I] = (Data[I] == 0 ? 0xA : Data[I]);
				}
				break;

				case REG_SZ:
				case REG_EXPAND_SZ:
				{
						std::vector<char> Data(dwSize);
						dwError = RegQueryValueEx(hKey, KeyToRead, nullptr, &KeyType, reinterpret_cast<std::uint8_t*>(Data.data()), &dwSize);
						strcpy(Result, Data.data());
				}
				break;

				default:
					dwError = ERROR_CANTREAD;
				break;
			}
		}
		RegCloseKey(hKey);
		return dwSize;
	}

	return dwError == ERROR_SUCCESS;
}

bool GetDriveInfo(const char* Drive, std::uint16_t &FreeSpace, std::uint16_t &TotalSpace, bool InGigaBytes)
{
    ULARGE_INTEGER FreeBytesAvailable = {0};
    ULARGE_INTEGER TotalNumberOfBytes = {0};

    bool Result = GetDiskFreeSpaceEx(Drive, &FreeBytesAvailable, &TotalNumberOfBytes, NULL);
    if (Result)
    {
        FreeSpace = (FreeBytesAvailable.QuadPart / (InGigaBytes ? 0x40000000 : 0x100000));
        TotalSpace = (TotalNumberOfBytes.QuadPart / (InGigaBytes ? 0x40000000 : 0x100000));
    }
    else
    {
        FreeSpace = 0;
        TotalSpace = 0;
    }
    return Result;

}

const char* GetTypeOfDrive(const char* Drive)
{
    const char* Result = NULL;
    unsigned int DriveType = GetDriveType(Drive);

    switch(DriveType)
    {
        case DRIVE_FIXED:
            Result = "Hard disk";
            break;

        case DRIVE_CDROM:
            Result = "CD/DVD";
            break;

        case DRIVE_REMOVABLE:
            Result = "Removable";
            break;

        case DRIVE_REMOTE:
            Result = "Network";
            break;

        default:
            Result = "Unknown";
            break;
    }

    return Result ? garbagecleaner.Add(new PSString<char>(Result))->data() : NULL;
}

int GetLogicalDrivesList(char Drives[26])
{
    int Res = 0;
    DWORD DrivesMask = GetLogicalDrives();

    for (int I = 0; I < 26; ++I)
    {
        if (DrivesMask & (1 << I))
        {
            Drives[Res++] = 'A' + I;
        }
    }
    return Res;
}

unsigned int Thread_CreateThread(void* Pointer)
{
    std::function<void()> FunctionToPass((void(*)())Pointer);
    return ThreadList.StartThread(FunctionToPass);
}

void Thread_DetachThread(unsigned int ThreadID)
{
    ThreadList.Detach(ThreadID);
}

void Thread_JoinThread(unsigned int ThreadID)
{
    ThreadList.Join(ThreadID);
}

void Thread_ClearThreads(bool Join)
{
    ThreadList.Clear(Join);
}

unsigned int Thread_HardwareConcurrency()
{
    return ThreadList.HardwareConcurrency();
}

unsigned int Thread_NativeID(unsigned int ThreadID)
{
    return ThreadList.NativeID(ThreadID);
}

void Thread_SleepThreadFor(unsigned int Milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(Milliseconds));
}

void Thread_YieldThread()
{
    std::this_thread::yield();
}

unsigned int Async_CreateAsync(void* Pointer)
{
    std::function<void()> FunctionToPass((void(*)())Pointer);
    return AsyncList.StartAsync(FunctionToPass);
}

void Async_ClearAsync()
{
    AsyncList.Clear();
}

void Async_Wait(unsigned int AsyncID)
{
    AsyncList.Wait(AsyncID);
}

bool Async_WaitFor(unsigned int AsyncID, unsigned int Milliseconds)
{
    return AsyncList.Wait_For(AsyncID, Milliseconds);
}

unsigned int Lock_CreateLock()
{
    return LockList.AddLock();
}

void Lock_RemoveLock(unsigned int LockID)
{
    LockList.RemoveLock(LockID);
}

void Lock_Lock(unsigned int LockID)
{
    LockList.Lock(LockID);
}

bool Lock_TryLock(unsigned int LockID)
{
    return LockList.TryLock(LockID);
}

void Lock_Unlock(unsigned int LockID)
{
    LockList.Unlock(LockID);
}

void Lock_GlobalLock()
{
    LockList.GlobalLock();
}

bool Lock_GlobalTryLock()
{
    return LockList.GlobalTryLock();
}

void Lock_GlobalUnlock()
{
    LockList.GlobalUnlock();
}

void Lock_ClearLocks()
{
    LockList.Clear();
}

unsigned int Graphics_CreateImage(int Width, int Height)
{
    return ImgManager.CreateImage(Width, Height);
}

unsigned int Graphics_LoadImage(const char* FilePath)
{
    return ImgManager.LoadImage(FilePath);
}

unsigned int Graphics_ImageFromPointer(unsigned char* Pointer, int Width, int Height)
{
    return ImgManager.ImageFromPointer(Pointer, Width, Height);
}

unsigned int Graphics_ImageFromCursor()
{
    try {return ImgManager.ImageFromCursor();} catch (std::exception &e){}
    return -1;
}

unsigned int Graphics_ImageFromWindow(HWND Window, int X, int Y, int Width, int Height)
{
    return ImgManager.ImageFromWindow(Window, X, Y, Width, Height);
}

void Graphics_DrawToWindow(unsigned int ImageID, HWND Window, int X, int Y, int Width, int Height, int SourceX, int SourceY, DWORD RasterType)
{
    ImgManager.DrawToWindow(ImageID, Window, X, Y, Width, Height, SourceX, SourceY, RasterType);
}

void Graphics_SaveImage(unsigned int ImageID, const char* FilePath)
{
    ImgManager.SaveImage(ImageID, FilePath);
}

void Graphics_DeleteImage(unsigned int ImageID)
{
    ImgManager.DeleteImage(ImageID);
}

unsigned int Graphics_GetPixel(unsigned int ImageID, int X, int Y)
{
    return ImgManager.GetPixel(ImageID, X, Y);
}

CG::BGRA* Graphics_GetPixels(unsigned int ImageID, int &Width, int &Height)
{
    return ImgManager.GetPixels(ImageID, Width, Height);
}

void Graphics_SetPixel(unsigned int ImageID, unsigned int Colour, int X, int Y)
{
    ImgManager.SetPixel(ImageID, Colour, X, Y);
}

void Graphics_GetDimensions(unsigned int ImageID, int &Width, int &Height)
{
    ImgManager.GetDimensions(ImageID, Width, Height);
}

void Graphics_Clear()
{
    ImgManager.Clear();
}

void reverse(char* &str, int len)
{
	std::reverse(&str[0], &str[0] + strlen(str));
}

void reverse(int* &arr, int len)
{
	std::reverse(&arr[0], &arr[0] + len);
}
