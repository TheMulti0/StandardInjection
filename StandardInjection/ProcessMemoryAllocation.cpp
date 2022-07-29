#include "ProcessMemoryAllocation.h"

#include "Extensions.h"

ProcessMemoryAllocation::ProcessMemoryAllocation(
	const HANDLE process, 
	void* offset,
	const size_t size
) :
	_process(process),
	_size(size),
	_ptr(VirtualAllocEx(
		_process, 
		offset, 
		_size,
		MEM_COMMIT, 
		PAGE_READWRITE))
{
}

ProcessMemoryAllocation::~ProcessMemoryAllocation()
{
	if (!VirtualFreeEx(_process, _ptr, 0, MEM_RELEASE))
	{
		ThrowWinApiException("Failed to free pointer to %d", _ptr);
	}
}

void ProcessMemoryAllocation::Write(const void* buffer, const size_t size) const
{
	if (!WriteProcessMemory(
		_process, 
		_ptr, 
		buffer,
		size, 
		nullptr))
	{
		ThrowWinApiException("Failed to write buffer with size %d to pointer %d", size, _ptr);
	}
}

void* ProcessMemoryAllocation::GetPointer() const
{
	return _ptr;
}
