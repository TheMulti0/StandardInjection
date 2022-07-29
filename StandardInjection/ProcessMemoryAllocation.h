#pragma once

#include <Windows.h>

class ProcessMemoryAllocation
{
public:
	ProcessMemoryAllocation(
		HANDLE process,
		void* offset,
		size_t size);

	~ProcessMemoryAllocation();

	void Write(const void* buffer, size_t size) const;

	[[nodiscard]] void* GetPointer() const;

private:
	HANDLE _process;
	size_t _size;
	void* _ptr;
};

