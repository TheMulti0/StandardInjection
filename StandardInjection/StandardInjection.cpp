// StandardInjection.cpp : Defines the entry point for the console application.
//

#include <filesystem>
#include <iostream>
#include <Windows.h>

#include "InjectedDll.h"
#include "ProcessMemoryAllocation.h"
#include "UniqueHandle.h"

std::wstring ToWString(const std::string& s)
{
	const auto size = s.size();

	const int len = MultiByteToWideChar(
		CP_ACP, 
		0, 
		s.c_str(), 
		size, 
		nullptr, 
		0);

	std::wstring r(len, L'\0');

	MultiByteToWideChar(
		CP_ACP, 
		0,
		s.c_str(),
		size, 
		&r[0], 
		len);

	return r;
}

int* GetProcessId(const std::string& windowTitle)
{
	const std::wstring wWindowTitle = ToWString(windowTitle);
	DWORD processId;

	GetWindowThreadProcessId(
		FindWindow(nullptr, wWindowTitle.c_str()),
		&processId);

	return new int(processId);
}

int strcompare(const char* One, const char* Two)
{
	return _stricmp(One, Two);
}

void HandleError()
{
	int lastError = GetLastError();
	printf("Last error: %d", lastError);
	throw;
}

int main()
{
	const std::string dllName = "testlib.dll";
	const std::string relativeDllPath = "../bin/" + dllName;
	const auto dllFile = std::filesystem::path(relativeDllPath);

	if (!exists(dllFile))
	{
		HandleError();
	}

	const auto dllPath = canonical(dllFile).generic_string();
	if (dllPath.empty())
	{
		HandleError();
	}
	const auto dllPathLength = dllPath.size() + 1;

	const auto windowName = std::string("Untitled - Notepad");
	const int* pProcessId = GetProcessId(windowName);
	if (pProcessId == nullptr)
	{
		HandleError();
	}

	const int processId = *pProcessId;

	// Open a handle to target process
	const auto process = MakeUniqueHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId));
	if (process.get() == nullptr)
	{
		HandleError();
	}

	// Allocate memory for the dllpath in the target process
	// length of the path string + null terminator
	const auto dll = ProcessMemoryAllocation(process.get(), nullptr, dllPathLength);

	// Write the path to the address of the memory we just allocated
	// in the target process
	dll.Write(dllPath.c_str(), dllPathLength);

	const auto injected = InjectedDll(
		processId, 
		process.get(),
		ToWString(dllName),
		dll.GetPointer());

	/*if (loadThread.get() == nullptr)
	{
		HandleError();
	}
	DWORD ret = 0;
	GetExitCodeThread(loadThread.get(), &ret);*/

	std::cout << "Dll path allocated at: " << std::hex << dll.GetPointer() << std::endl;
	std::cin.get();

	return 0;
}