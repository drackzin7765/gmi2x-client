#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <fstream>

DWORD GetProcessIdByName(const wchar_t* processName)
{
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snapshot, &entry))
    {
        do
        {
            if (_wcsicmp(entry.szExeFile, processName) == 0)
            {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return pid;
}

HWND WaitForWindow(const char* className, const char* windowTitle)
{
    HWND hWnd = nullptr;
    for (int i = 0; i < 100; ++i) // try max 10 seconds (100 * 100ms)
    {
        hWnd = FindWindowA(className, windowTitle);
        if (hWnd)
            return hWnd;
        Sleep(100);
    }
    return nullptr;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    char modulePath[MAX_PATH];
    GetModuleFileNameA(NULL, modulePath, MAX_PATH);

    std::string dllPath(modulePath);
    size_t pos = dllPath.find_last_of('\\');
    if (pos != std::string::npos)
        dllPath.replace(pos + 1, std::string::npos, "gmi2x-client.dll");

    std::string iniPath(modulePath);
    pos = iniPath.find_last_of('\\');
    if (pos != std::string::npos)
        iniPath.replace(pos + 1, std::string::npos, "config.ini");

    std::string gamePath;

    // Read GamePath from ini
    std::ifstream iniFile(iniPath);
    if (iniFile.is_open())
    {
        std::string line;
        while (std::getline(iniFile, line))
        {
            if (line.find("GamePath=") == 0)
            {
                gamePath = line.substr(9);
                break;
            }
        }
    }

    if (gamePath.empty())
    {
        MessageBoxA(NULL, "Please specify the path to CoDUOMP.exe in config.ini", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Extract executable filename wide
    size_t lastSlash = gamePath.find_last_of("\\/");
    std::wstring exeNameW;
    if (lastSlash != std::string::npos)
        exeNameW = std::wstring(gamePath.begin() + lastSlash + 1, gamePath.end());
    else
        exeNameW = std::wstring(gamePath.begin(), gamePath.end());

    DWORD pid = GetProcessIdByName(exeNameW.c_str());
    if (pid == 0)
    {
        // Start process
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        char cmdLine[MAX_PATH];
        strcpy(cmdLine, gamePath.c_str());

        if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            MessageBoxA(NULL, "Failed to start game process", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        pid = pi.dwProcessId;
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                                  PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
                                  FALSE, pid);
    if (!hProcess)
    {
        MessageBoxA(NULL, "Failed to open game process (are you running as admin?)", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Allocate memory in the remote process for the DLL path
    void* remoteMem = VirtualAllocEx(hProcess, NULL, dllPath.size() + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!remoteMem)
    {
        MessageBoxA(NULL, "Failed to allocate memory in target process", "Error", MB_OK | MB_ICONERROR);
        CloseHandle(hProcess);
        return 1;
    }

    if (!WriteProcessMemory(hProcess, remoteMem, dllPath.c_str(), dllPath.size() + 1, NULL))
    {
        MessageBoxA(NULL, "Failed to write DLL path to target process memory", "Error", MB_OK | MB_ICONERROR);
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // Create remote thread to load the DLL
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                                        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
                                        remoteMem, 0, NULL);
    if (!hThread)
    {
        MessageBoxA(NULL, "Failed to create remote thread", "Error", MB_OK | MB_ICONERROR);
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    // Wait a bit for the window to appear
    Sleep(3000);

    HWND hWnd = WaitForWindow(NULL, "CoD:United Offensive Multiplayer");
    if (hWnd)
    {
        char imguiIniPath[MAX_PATH];
        strcpy(imguiIniPath, modulePath);
        pos = std::string(imguiIniPath).find_last_of('\\');
        if (pos != std::string::npos)
            std::string(imguiIniPath).replace(pos + 1, std::string::npos, "imgui.ini");

        COPYDATASTRUCT cds;
        cds.dwData = 1;
        cds.cbData = (DWORD)(strlen(imguiIniPath) + 1);
        cds.lpData = imguiIniPath;

        SendMessageA(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
    }

    return 0;
}
