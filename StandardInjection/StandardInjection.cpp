// StandardInjection.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <Windows.h>

#include "ProcessMemoryAllocation.h"
#include "UniqueHandle.h"

std::string GetDllPath()
{
	const auto dllName = std::string("testlib.dll");

	char* const dllPath = _fullpath(
		nullptr,
		dllName.c_str(),
		_MAX_PATH);

	return { dllPath };
}

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

int GetProcessId(const std::string& windowTitle)
{
	const std::wstring wWindowTitle = ToWString(windowTitle);
	DWORD processId;

	GetWindowThreadProcessId(
		FindWindow(nullptr, wWindowTitle.c_str()),
		&processId);

	return processId;
}

int main()
{
	const auto dllPath = GetDllPath();
	const auto dllPathLength = dllPath.size() + 1;

	const auto windowName = std::string("Untitled - Notepad");
	const int processId = GetProcessId(windowName);

	// Open a handle to target process
	const auto process = MakeUniqueHandle(
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId));

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
			nullptr, 
			0,
			(LPTHREAD_START_ROUTINE) procAddress,
			dll.GetPointer(),
			0, 
			nullptr));

	// Wait for the execution of our loader thread to finish
	WaitForSingleObject(loadThread.get(), INFINITE);

	std::cout << "Dll path allocated at: " << std::hex << dll.GetPointer() << std::endl;
	std::cin.get();

	return 0;
}