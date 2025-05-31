#include <windows.h>
#include "cuocx.hpp"

bool applyHooks()
{


	return true;
}

void Main_UnprotectModule(HMODULE hModule)
{
	PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hModule + header->e_lfanew);

	SIZE_T size = ntHeader->OptionalHeader.SizeOfImage;
	DWORD oldProtect;
	VirtualProtect((LPVOID)hModule, size, PAGE_EXECUTE_READWRITE, &oldProtect);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HMODULE hModule = GetModuleHandle(NULL); // codmp.exe
		if (hModule)
			Main_UnprotectModule(hModule);

#if 0
#ifdef DEBUG
		if (hLogFile == INVALID_HANDLE_VALUE)
		{
			hLogFile = CreateFile(L"./memlog.txt",
				GENERIC_WRITE,
				FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL);
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, hLogFile);
		}
#endif
#endif

		void _CL_Init();
	    __call(0x0043c166, (int)_CL_Init);
	    __call(0x0043c7c7, (int)_CL_Init);
	}
	break;

	case DLL_PROCESS_DETACH:
	{

	}
	break;

	}
	return TRUE;
}