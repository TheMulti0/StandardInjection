// StandardInjection.cpp : Defines the entry point for the console application.
//

#include <filesystem>
#include <iostream>
#include <Windows.h>

#include "ProcessMemoryAllocation.h"
#include "UniqueHandle.h"

std::wstring ToWString(const std::string& s)
{
	const auto size = s.size() + 1;

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

void HandleError()
{
	int lastError = GetLastError();
	printf("Last error: %d", lastError);
	throw;
}

int main()
{
	const auto dllName = "../bin/testlib.dll";
	const auto dllFile = std::filesystem::path(dllName);

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

	const auto procAddress = GetProcAddress(
		GetModuleHandleA("Kernel32.dll"), 
		"LoadLibraryA");
	// Create a Remote Thread in the target process which
	// calls LoadLibraryA as our dllpath as an argument -> program loads our dll

	const auto loadThread = MakeUniqueHandle(
		CreateRemoteThread(
			process.get(),
			0, 
			0,
			(LPTHREAD_START_ROUTINE)procAddress,
			dll.GetPointer(),
			0, 
			0));

	if (loadThread.get() == nullptr)
	{
		HandleError();
	}

	// Wait for the execution of our loader thread to finish
	WaitForSingleObject(loadThread.get(), INFINITE);

	DWORD ret = 0;
	GetExitCodeThread(loadThread.get(), &ret);

	std::cout << "Dll path allocated at: " << std::hex << dll.GetPointer() << std::endl;
	std::cin.get();

	return 0;
}