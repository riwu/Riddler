#include "PSInfo.hpp"

template<typename T>
int PascalHigh(T* Arr)
{
    return reinterpret_cast<const int*>(Arr)[-1];
}

template<typename T>
int PascalLength(T* Arr)
{
    return reinterpret_cast<const int*>(Arr)[-1] + 1;
}

template<typename T>
int PascalReferenceCount(T* Arr)
{
    return reinterpret_cast<const int*>(Arr)[-2];
}

const auto ExportCount = sizeof(Exports) / (sizeof(Exports[0]) * 2);
const auto TypeCount = sizeof(Types) / (sizeof(Types[0]) * 2);

int GetPluginABIVersion()
{
	return 2;
}

int GetFunctionCount()
{
	return ExportCount;
}

int GetTypeCount()
{
	return TypeCount;
}

int GetFunctionInfo(int Index, void* &Address, char* &Definition)
{
	if (static_cast<std::size_t>(Index) < ExportCount)
	{
		#ifdef _MSC_VER
		#pragma warning(disable: 4996)
		#endif
		strcpy(Definition, Exports[Index * 2 + 1]);
		Address = reinterpret_cast<void*>(GetProcAddress(hInstance, Exports[Index * 2]));
		return Index;
	}
	return -1;
}

int GetTypeInfo(int Index, char* &Type, char* &Definition)
{
	if (static_cast<std::size_t>(Index) < ExportCount)
	{
		#ifdef _MSC_VER
		#pragma warning(disable: 4996)
		#endif
		strcpy(Type, Types[Index * 2 + 0]);
		strcpy(Definition, Types[Index * 2 + 1]);
		return Index;
	}
	return -1;
}
