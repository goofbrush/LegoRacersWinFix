#include <Windows.h>
#include <TlHelp32.h>
#include <map>

using BYTE = unsigned char;


class Memory
{
public:
    Memory(const char* windowName, const char* executableName) {
        while(!CreateHandle(windowName)) Sleep(100);
        addModule(executableName);
    }

    void addModule(const char* moduleName) {
        modules[moduleName] = GetModuleBaseAddress(moduleName);
    }

    template <typename Type, typename... ptr>
    Type readValue(uintptr_t address, const char* moduleBase, ptr&&... ptrs) { 
        Type output; address = itrPtrs(address, moduleBase, ptrs...);
        ReadProcessMemory(Handle, (LPVOID)address, &output, sizeof(output), 0);
        return output;
    }

    template <typename Type, typename... ptr>
    void writeValue(Type input, uintptr_t address, const char* moduleBase, ptr&&... ptrs) {
        address = itrPtrs(address, moduleBase, ptrs...);
        WriteProcessMemory(Handle, (LPVOID)address, &input, sizeof(input), 0);
    }

private:
    HWND Window;    // Game Window
    DWORD ProcID;   // Game Thread ID
    HANDLE Handle;  // Game Process Handle
    std::map<const char*,uintptr_t> modules; // Module Addresses

    bool CreateHandle(const char* windowName) {
        Window = FindWindowA(NULL, windowName);
        if(Window != NULL) {
            GetWindowThreadProcessId(Window, &ProcID);
            Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcID);
            if(Handle != NULL) { return true;  } // Connection Successful
        }
        return false; // Could not Connect to the Game      
    }

    uintptr_t GetModuleBaseAddress(const char* moduleName) {
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcID);
        if (hSnap != INVALID_HANDLE_VALUE) { MODULEENTRY32 modEntry; modEntry.dwSize = sizeof(modEntry);
            if (Module32First(hSnap, &modEntry)) {
                do { if (!_stricmp((const char*)modEntry.szModule, moduleName)) 
                    return (uintptr_t)modEntry.modBaseAddr;
                } while (Module32Next(hSnap, &modEntry));
            }
        } CloseHandle(hSnap);
        return 0;
    }

    template <typename... ptr>
    uintptr_t itrPtrs(uintptr_t address, const char* moduleBase, ptr&&... ptrs) {
        address += modules[moduleBase];

        ([&]{   // Iterates through variadic pointer stack
            ReadProcessMemory(Handle, (LPVOID)address, &address, sizeof(unsigned int), 0);
            address += ptrs;    
        }(),...);

        return address;
    }
};



int main() {
    const char* exe = "LEGORacers.exe"; uintptr_t fixaddr;
    Memory mem = Memory("LEGO Racers", exe);

    while(true) {
        if(mem.readValue<int>(0xC4E08, exe, 0x20) == 14656118) { fixaddr = 0x417602; break; } // 1999
        if(mem.readValue<int>(0xEF810, exe, 0xC) == 14623584) { fixaddr = 0x48A562; break; } // 2001
    }
    mem.writeValue<BYTE>((BYTE)235, fixaddr, "");
}


