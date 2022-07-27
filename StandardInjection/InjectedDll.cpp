#include "InjectedDll.h"

InjectedDll::InjectedDll(const int processId, const HANDLE process, std::wstring dllName, void* dllNameBuffer):
	_processId(processId),
	_process(process),
	_dllName(std::move(dllName)),
	_loadThread(
		MakeUniqueHandle(
			CreateRemoteThread(
				_process,
				nullptr,
				0,
				reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA),
				dllNameBuffer,
				0,
				nullptr)))
{
	if (_loadThread.get() == nullptr)
	{
		throw;
	}

	WaitForSingleObject(_loadThread.get(), INFINITE);
}

InjectedDll::~InjectedDll()
{
	const auto moduleInfo = GetModuleInfo();

	const auto unloadThread = MakeUniqueHandle(
		CreateRemoteThread(
			_process,
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(FreeLibrary),
			moduleInfo.hModule,
			0,
			nullptr));

	WaitForSingleObject(unloadThread.get(), INFINITE);
}

MODULEENTRY32W InjectedDll::GetModuleInfo(int processId, const std::wstring& moduleName)
{
	MODULEENTRY32 moduleInfo = { 0 };

	const auto snapshot = MakeUniqueHandle(
		CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId));

	if (snapshot.get() == INVALID_HANDLE_VALUE)
	{
		return moduleInfo;
	}

	moduleInfo.dwSize = sizeof(MODULEENTRY32);

	while (Module32Next(snapshot.get(), &moduleInfo))
	{
		if (std::wstring(moduleInfo.szModule) == moduleName)
		{
			return moduleInfo;
		}
	}

	moduleInfo = { 0 };
	return moduleInfo;
}

MODULEENTRY32W InjectedDll::GetModuleInfo() const
{
	return GetModuleInfo(_processId, _dllName);
}
