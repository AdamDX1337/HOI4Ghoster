// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include "MinHook.h"
#include <Psapi.h>



typedef void* (__fastcall* GetCAddPlayerCommand)(void* pThis, void* User, void* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7);
GetCAddPlayerCommand CAddPlayerCommandHook;
GetCAddPlayerCommand CAddPlayerCommandTramp;

typedef void* (__fastcall* GetDummyFunc)(void* pThis, int* tag, int toggled);
GetDummyFunc UselessDummyFunc; //This is actually the enable ai func

uintptr_t GameBase = (uintptr_t)GetModuleHandleA("hoi4.exe");



//Sig Scanning So it auto-updates (may need to change between hypermajor updates)
uintptr_t FindPattern(char* pattern, char* mask)
{
    uintptr_t base = GameBase;
    MODULEINFO modInfo;
    GetModuleInformation(GetCurrentProcess(), (HMODULE)GameBase, &modInfo, sizeof(MODULEINFO));
    uintptr_t size = modInfo.SizeOfImage;

    uintptr_t patternLength = (uintptr_t)strlen(mask);

    for (uintptr_t i = 0; i < size - patternLength; i++)
    {
        bool found = true;

        for (uintptr_t j = 0; j < patternLength; j++)
        {
            found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
        }

        if (found)
            return base + i;
    }

    return 0xDEADBEEF;
}




// CCommand Equivalent (void* var = operator new(int);
void* __fastcall Command(size_t Size) {
	size_t i;
	void* result;
	for (i = Size; ; Size = i) {
		result = _malloc_base(Size);
		if (result) {
			break;
		}
	}
	return result;
}

int i = 0;
int* _ptr = &i;
void* CommandVar = (void*)Command(56);

//AddPlayer Command Hook
void* __fastcall hkCAddPlayerCommand(void* pThis, void* User, void* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7)
{
	return (void*)UselessDummyFunc(CommandVar, _ptr, 0); //returning this instead of add player function
}


//Pretty sure I need this
void MainThread() {
	while (true) {

	}
	
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        MH_Initialize();
        

        const char* pattern = "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x48\x89\x4C\x24\x00\x41\x56\x48\x83\xEC\x00\x49\x8B\xF1\x49\x8B\xF8\x48\x8B\xDA\x4C\x8B\xF1\xE8\x00\x00\x00\x00\x66\x41\xC7\x46\x00\x00\x00\x41\xC7\x46\x00\x00\x00\x00\x00\x33\xC9\x41\x89\x4E\x00\x41\xC7\x46\x00\x00\x00\x00\x00\x66\x41\x89\x4E\x00\x49\x89\x4E\x00\x48\x8D\x05\x00\x00\x00\x00\x49\x89\x06\x49\x8D\x4E\x00\x48\x8B\xD3\xE8\x00\x00\x00\x00\x90";
        const char* mask = "xxxx?xxxx?xxxx?xxxx?xxxxx?xxxxxxxxxxxxx????xxxx???xxx?????xxxxx?xxx?????xxxx?xxx?xxx????xxxxxx?xxxx????x";

        uintptr_t CAddPlayerAddress = FindPattern(const_cast<char*>(pattern), const_cast<char*>(mask));

        pattern = "\x57\x48\x83\xEC\x00\x41\x8B\xF0\x48\x8B\xDA\x48\x8B\xF9\xE8\x00\x00\x00\x00\x33\xC9\x66\xC7\x47\x00\x00\x00\x48\x8D\x05\x00\x00\x00\x00\x89\x4F\x00\x48\x89\x07\x66\x89\x4F\x00\x48\x89\x4F\x00\xC7\x47\x00\x00\x00\x00\x00\xC7\x47\x00\x00\x00\x00\x00\x8B\x03\x48\x8B\x5C\x24\x00\x89\x47\x00\x48\x8B\xC7\x89\x77\x00\x48\x8B\x74\x24\x00\x48\x83\xC4\x00\x5F\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x00\x48\x8B\xF9";
        mask = "xxxx?xxxxxxxxxx????xxxxx???xxx????xx?xxxxxx?xxx?xx?????xx?????xxxxxx?xx?xxxxx?xxxx?xxx?xxxxxxxxxxxxxxxxxxx?xxxx?xxx";

        uintptr_t DummyAddress = FindPattern(const_cast<char*>(pattern), const_cast<char*>(mask));




		CAddPlayerCommandHook = GetCAddPlayerCommand(CAddPlayerAddress);
		MH_CreateHook(CAddPlayerCommandHook, &hkCAddPlayerCommand, (LPVOID*)&CAddPlayerCommandTramp);
		MH_EnableHook(CAddPlayerCommandHook);

		UselessDummyFunc = GetDummyFunc(DummyAddress);
        

        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, nullptr);

    }
    return TRUE;
}

