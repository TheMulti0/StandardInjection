#include "UniqueHandle.h"

UniqueHandle MakeUniqueHandle(HANDLE h)
{
	return sr::make_unique_resource_checked(h, INVALID_HANDLE_VALUE, CloseHandle);
}
