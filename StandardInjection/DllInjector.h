#pragma once

#include <filesystem>
#include <string>

#include "Extensions.h"
#include "InjectedDll.h"
#include "ProcessMemoryAllocation.h"

class DllInjector
{
public:
	DllInjector(
		std::string dllName,
		std::string dllPath,
		const std::string& windowName
	) :
		_dllName(std::move(dllName)),
		_dllPath(std::move(dllPath)),
		_processId(GetProcessId(windowName))
	{
		Construct();
	}
	
	DllInjector(
		std::string dllName,
		std::string dllPath,
		const int processId
	) :
		_dllName(std::move(dllName)),
		_dllPath(std::move(dllPath)),
		_processId(processId)
	{
		Construct();
	}

private:
	static int GetProcessId(const std::string& windowTitle)
	{
		const std::wstring wWindowTitle = ToWString(windowTitle);
		DWORD processId;

		GetWindowThreadProcessId(
			FindWindow(nullptr, wWindowTitle.c_str()),
			&processId);

		if (processId == 0)
		{
			ThrowRuntimeException("Unable to find a process with the window title %s", windowTitle.c_str());
		}

		return processId;
	}

	void Construct()
	{
		_dllAbsolutePath = GetDllAbsolutePath(_dllPath);
		_dllAbsolutePathSize = _dllAbsolutePath.size() + 1;

		_process = std::move(OpenProcess());

		InjectDll();
	}

	[[nodiscard]] std::string GetDllAbsolutePath(const std::string& dllPath) const
	{
		const auto dllFile = std::filesystem::path(dllPath);

		if (!exists(dllFile))
		{
			ThrowRuntimeException("Unable to find a file at the path %s", dllPath.c_str());
		}

		const auto absolutePath = canonical(dllFile).generic_string();
		if (absolutePath.empty())
		{
			ThrowRuntimeException("Unable to find a file at the path %s", dllPath.c_str());
		}

		return absolutePath;
	}

	[[nodiscard]] UniqueHandle OpenProcess() const
	{
		auto process = MakeUniqueHandle(
			::OpenProcess(PROCESS_ALL_ACCESS, FALSE, _processId));

		if (process.get() == nullptr)
		{
			ThrowWinApiException("Unable to open process with id %d", _processId);
		}

		return process;
	}

	void InjectDll()
	{
		_dllNameInProcess = std::make_unique<ProcessMemoryAllocation>(
			_process.get(),
			nullptr, 
			_dllAbsolutePathSize);

		_dllNameInProcess->Write(_dllAbsolutePath.c_str(), _dllAbsolutePathSize);

		_injectedDll = std::make_unique<InjectedDll>(
			_processId,
			_process.get(),
			ToWString(_dllName),
			_dllNameInProcess->GetPointer());
	}

	std::string _dllName;
	std::string _dllPath;
	int _processId;

	std::string _dllAbsolutePath;
	size_t _dllAbsolutePathSize{};

	UniqueHandle _process;

	std::unique_ptr<ProcessMemoryAllocation> _dllNameInProcess;
	std::unique_ptr<InjectedDll> _injectedDll;
};

