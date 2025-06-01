#include <windows.h>
#include "cuocx.hpp"
#include <ImageHlp.h>
#include <stdint.h> 
#include <stdio.h>

static void(*Com_Quit_f)() = (void(*)())0x0043a2c0;

typedef HMODULE(WINAPI* LoadLibraryA_t)(LPCSTR lpLibFileName);
LoadLibraryA_t orig_LoadLibraryA = NULL;

HMODULE WINAPI hLoadLibraryA(LPCSTR lpLibFileName) {
    HMODULE hModule = orig_LoadLibraryA(lpLibFileName);

    if (!hModule) return NULL;

    DWORD pBase = (DWORD)GetModuleHandle(lpLibFileName);
    if (pBase) {
        void Main_UnprotectModule(HMODULE hModule);
        Main_UnprotectModule(hModule);

        if (strstr(lpLibFileName, "uo_cgame_mp") != NULL) {
			Com_Printf("Hooking into CG_INIT \n");
            void CG_Init(DWORD);
            CG_Init(pBase);
        }
    }

    return hModule;
}


// CHATGPT
BYTE original_bytes[5];
BYTE* trampoline = NULL; // dynamically allocated

void patch_opcode_loadlibrary(void)
{
    DWORD oldProtect;
    BYTE* pLoadLibrary = (BYTE*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

    if (!pLoadLibrary)
    {
        printf("Failed to find LoadLibraryA\n");
        return;
    }

    printf("LoadLibraryA address: %p\n", pLoadLibrary);

    // Save original bytes
    memcpy(original_bytes, pLoadLibrary, 5);

    // Allocate trampoline with execute permission
    trampoline = (BYTE*)VirtualAlloc(NULL, 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline)
    {
        printf("Failed to allocate trampoline\n");
        return;
    }

    // Copy original bytes into trampoline
    memcpy(trampoline, original_bytes, 5);

    // Calculate jump back from trampoline+5 to LoadLibraryA+5
    uintptr_t jump_back_addr = (uintptr_t)(pLoadLibrary + 5);
    uintptr_t trampoline_jump_src = (uintptr_t)(trampoline + 5);
    intptr_t rel_jump_back = jump_back_addr - trampoline_jump_src - 5;

    trampoline[5] = 0xE9; // JMP opcode
    *(DWORD*)(trampoline + 6) = (DWORD)rel_jump_back;

    // Set trampoline function pointer
    orig_LoadLibraryA = (LoadLibraryA_t)trampoline;

    // Change protection to writable
    if (!VirtualProtect(pLoadLibrary, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        printf("VirtualProtect failed\n");
        return;
    }

    // Calculate relative JMP from LoadLibraryA to hLoadLibraryA
    uintptr_t hook_addr = (uintptr_t)hLoadLibraryA;
    uintptr_t loadlib_addr = (uintptr_t)pLoadLibrary;
    intptr_t rel_addr = hook_addr - loadlib_addr - 5;

    pLoadLibrary[0] = 0xE9; // JMP opcode
    *(DWORD*)(pLoadLibrary + 1) = (DWORD)rel_addr;

    // Restore original protection
    if (!VirtualProtect(pLoadLibrary, 5, oldProtect, &oldProtect))
    {
        printf("Failed to restore protection\n");
        return;
    }

    // Flush CPU instruction cache so CPU executes new code
    if (!FlushInstructionCache(GetCurrentProcess(), pLoadLibrary, 5))
    {
        printf("FlushInstructionCache failed\n");
        return;
    }

    printf("Hook installed on LoadLibraryA successfully\n");
}




bool applyHooks()
{

	patch_opcode_loadlibrary();

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

		if (!applyHooks())
		{
			MessageBoxA(NULL, "Hooking failed", "c1cx", MB_OK | MB_ICONERROR);
			Com_Quit_f();
		}

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