#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

// Change "MY_DLL_NAME.dll" to your dll name
#define DLL_NAME "pasta.dll"

// And you need:
// 1. change all bytes(0x00, 0x01, 0x03....) in define "JUNKS" to random bytes
// 2. change size this block by add new bytes
#define JUNKS \
__asm _emit 0x10 \
__asm _emit 0x01 \
__asm _emit 0x86 \
__asm _emit 0xfb \
__asm _emit 0x62 \
__asm _emit 0x7e \
__asm _emit 0x91 \
__asm _emit 0x69 \
__asm _emit 0x59 \
__asm _emit 0x2c \
__asm _emit 0xc9 \
__asm _emit 0xf5 \
__asm _emit 0x6e \
__asm _emit 0x93 \
__asm _emit 0x58 \
__asm _emit 0x11 \
__asm _emit 0xec \
__asm _emit 0x44 \
__asm _emit 0x84 \
__asm _emit 0xcc \
__asm _emit 0xbb \
__asm _emit 0x68 \
__asm _emit 0x75 \
__asm _emit 0x57 \
__asm _emit 0x97 \
__asm _emit 0xec \
__asm _emit 0xfa \
__asm _emit 0x91 \
__asm _emit 0x94 \
__asm _emit 0xd4 \
__asm _emit 0x50 \
__asm _emit 0xa3 \
__asm _emit 0x4c \
__asm _emit 0xe4 \
__asm _emit 0x3d \
__asm _emit 0x4f \
__asm _emit 0x3f \
__asm _emit 0x07 \
__asm _emit 0x47 \
__asm _emit 0xf1 \
__asm _emit 0x08 \
__asm _emit 0x84 \
__asm _emit 0xa9 \
__asm _emit 0x67 \
__asm _emit 0x50 \
__asm _emit 0x89 \
__asm _emit 0xdd \
__asm _emit 0x0f \
__asm _emit 0xed \
__asm _emit 0x3a \
__asm _emit 0x6b \
__asm _emit 0x5b \
__asm _emit 0x69 \
__asm _emit 0x48 \
__asm _emit 0x1d \
__asm _emit 0x69 \
__asm _emit 0x69 \
__asm _emit 0x69 \
__asm _emit 0x22 \
__asm _emit 0x06 \
__asm _emit 0x02 \

// Don't change this!
#define _JUNK_BLOCK(s) __asm jmp s JUNKS __asm s:


/////////////////////////////////////////////////////////////////////////////////////
//                                 INJECTOR CODE                                   //
/////////////////////////////////////////////////////////////////////////////////////

DWORD Process(char* ProcessName)
{
	_JUNK_BLOCK(jmp_label1)
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
	_JUNK_BLOCK(jmp_label2)
	PROCESSENTRY32 ProcEntry;
	
	_JUNK_BLOCK(jmp_label3)
	ProcEntry.dwSize = sizeof(ProcEntry);
	
	_JUNK_BLOCK(jmp_label4)
	do
	{
		_JUNK_BLOCK(jmp_label5)
		if (!strcmp(ProcEntry.szExeFile, ProcessName))
		{
			_JUNK_BLOCK(jmp_label6)
			DWORD dwPID = ProcEntry.th32ProcessID;
			
			_JUNK_BLOCK(jmp_label7)
			CloseHandle(hPID);
			
			_JUNK_BLOCK(jmp_label8)
			return dwPID;
		}
		
		_JUNK_BLOCK(jmp_label9)
	}
	while (Process32Next(hPID, &ProcEntry));
	
	_JUNK_BLOCK(jmp_label10)
}

int main()
{
	_JUNK_BLOCK(jmp_label11)
	DWORD dwProcess;
	
	_JUNK_BLOCK(jmp_label12)
	char myDLL[MAX_PATH];
	
	_JUNK_BLOCK(jmp_label13)
	GetFullPathName(DLL_NAME, MAX_PATH, myDLL, 0);
	
	_JUNK_BLOCK(jmp_label4)
	dwProcess = Process("csgo.exe");
	
	_JUNK_BLOCK(jmp_label15)
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcess);
	
	_JUNK_BLOCK(jmp_label16)
	LPVOID allocatedMem = VirtualAllocEx(hProcess, NULL, sizeof(myDLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	_JUNK_BLOCK(jmp_label17)
	WriteProcessMemory(hProcess, allocatedMem, myDLL, sizeof(myDLL), NULL);
	
	_JUNK_BLOCK(jmp_label18)
	CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, allocatedMem, 0, 0);
	
	_JUNK_BLOCK(jmp_label19)
	CloseHandle(hProcess);
	
	_JUNK_BLOCK(jmp_label20)
	return 0;
	
	_JUNK_BLOCK(jmp_label21)
}