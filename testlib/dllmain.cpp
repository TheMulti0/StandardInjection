// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_THREAD_ATTACH:
	case DLL_PROCESS_ATTACH:
		MessageBox(0, L"Hello From testlib!", L"Hello", MB_ICONINFORMATION);
		break;
	default:
		break;
	}

	return TRUE;
}

