// StandardInjection.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <Windows.h>

#include "ProcessMemoryAllocation.h"

std::string GetDllPath()
{
	const auto dllName = std::string("testlib.dll");

	const auto dllPath = _fullpath(
		nullptr, 
		dllName.c_str(), 
		_MAX_PATH);

	return { dllPath };
}

int main()
{
	const auto dllPath = GetDllPath();
	const auto dllPathLength = dllPath.size() + 1;

	int processId = 1012;

	// Open a handle to target process
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	// Allocate memory for the dllpath in the target process
	// length of the path string + null terminator
	const auto dll = ProcessMemoryAllocation(hProcess, nullptr, dllPathLength);

	// Write the path to the address of the memory we just allocated
	// in the target process
	dll.Write(dllPath.data(), dllPathLength);

	// Create a Remote Thread in the target process which
	// calls LoadLibraryA as our dllpath as an argument -> program loads our dll
	HANDLE hLoadThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),
			"LoadLibraryA"), dll.GetPointer(), 0, 0);

	// Wait for the execution of our loader thread to finish
	WaitForSingleObject(hLoadThread, INFINITE);

	std::cout << "Dll path allocated at: " << std::hex << dll.GetPointer() << std::endl;
	std::cin.get();

	return 0;
}