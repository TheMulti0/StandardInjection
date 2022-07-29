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
	static std::unique_ptr<MODULEENTRY32W> GetModuleInfo(int processId, const std::wstring& moduleName);

	[[nodiscard]] std::unique_ptr<MODULEENTRY32W> GetModuleInfo() const;

	int _processId;
	HANDLE _process;
	std::wstring _dllName;
	UniqueHandle _loadThread;
};

