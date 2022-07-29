// StandardInjection.cpp : Defines the entry point for the console application.
//

#include <filesystem>
#include <iostream>

#include "DllInjector.h"

int main()
{
	const std::string dllName = "testlib.dll";
	const std::string relativeDllPath = "../bin/" + dllName;
	const auto windowName = std::string("Untitled - Notepad");

	try
	{
		const auto injector = DllInjector(dllName, relativeDllPath, windowName);
	}
	catch (std::runtime_error& e)
	{
		printf(e.what());
	}

	printf("Dll allocated");
	std::cin.get();

	return 0;
}