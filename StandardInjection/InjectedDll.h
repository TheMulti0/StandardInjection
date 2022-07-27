#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#include "UniqueHandle.h"

class InjectedDll
{
public:
	InjectedDll(
		int processId,
		HANDLE process,
		std::wstring dllName,
		void* dllNameBuffer
	);

	~InjectedDll();

private:
	static MODULEENTRY32 GetModuleInfo(int processId, const std::wstring& moduleName);

	MODULEENTRY32 GetModuleInfo() const;

	int _processId;
	HANDLE _process;
	std::wstring _dllName;
	UniqueHandle _loadThread;
};

