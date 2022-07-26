#pragma once

#include "Windows.h"
#include "unique_resource.h"

typedef sr::unique_resource<void*, int(*)(void*)> UniqueHandle;

UniqueHandle MakeUniqueHandle(HANDLE h);
