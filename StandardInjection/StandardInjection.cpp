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

int main()
{
	const auto dllPath = GetDllPath();
	const auto dllPathLength = dllPath.size() + 1;

	const int processId = 1844;

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