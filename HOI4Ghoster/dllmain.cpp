// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <string>
#include "MinHook.h"




typedef void* (__fastcall* GetCAddPlayerCommand)(void* pThis, void* User, void* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7);
GetCAddPlayerCommand CAddPlayerCommandHook;
GetCAddPlayerCommand CAddPlayerCommandTramp;

typedef void* (__fastcall* GetDummyFunc)(void* pThis, int* tag, int toggled);
GetDummyFunc UselessDummyFunc; //This is actually the enable ai func

uintptr_t GameBase = (uintptr_t)GetModuleHandleA("hoi4.exe");

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
        
		CAddPlayerCommandHook = GetCAddPlayerCommand(GameBase + 0x1651050);
		MH_CreateHook(CAddPlayerCommandHook, &hkCAddPlayerCommand, (LPVOID*)&CAddPlayerCommandTramp);
		MH_EnableHook(CAddPlayerCommandHook);

		UselessDummyFunc = GetDummyFunc(GameBase + 0xBD5040);
        

        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, nullptr);

    }
    return TRUE;
}

