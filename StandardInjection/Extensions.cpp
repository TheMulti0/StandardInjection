#include "Extensions.h"

#include <system_error>

std::string ToString(const std::wstring& s)
{
	const auto size = s.size();

	const int len = WideCharToMultiByte(
		CP_ACP,
		0,
		s.c_str(),
		size,
		nullptr,
		0,
		nullptr,
		nullptr);

	std::string r(len, L'\0');

	WideCharToMultiByte(
		CP_ACP,
		0,
		s.c_str(),
		size,
		&r[0],
		len,
		nullptr,
		nullptr);

	return r;
}

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

std::wstring GetErrorMessage(const DWORD dwErrorCode)
{
	LPTSTR message = nullptr;

	constexpr int flags = FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS
		| FORMAT_MESSAGE_ALLOCATE_BUFFER;

	const DWORD messageLength = FormatMessage(
		flags,
		nullptr,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&message),
		0,
		nullptr);

	if (messageLength <= 0)
	{
		const auto errorCode = GetLastError();

		throw std::system_error(
			errorCode, 
			std::system_category(),
			"Failed to retrieve error message string.");
	}

	auto deleter = [](void* p) { LocalFree(p); };

	const auto ptrBuffer = std::unique_ptr<wchar_t, decltype(deleter)>(message, deleter);

	return std::wstring(ptrBuffer.get(), messageLength);
}
