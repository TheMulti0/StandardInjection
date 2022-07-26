#include "UniqueHandle.h"

UniqueHandle MakeUniqueHandle(HANDLE h)
{
	if (h == INVALID_HANDLE_VALUE || h == NULL)
	{
		
	}
	return sr::make_unique_resource_checked(h, INVALID_HANDLE_VALUE, CloseHandle);
}
