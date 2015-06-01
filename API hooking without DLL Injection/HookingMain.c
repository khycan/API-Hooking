#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>

DWORD FindProcessID(LPCTSTR szProcessName);
void UnHookCode(void);
void HookCode(void);

BYTE g_pOrgBytes[5] = {0,};

void HookFunc(void)
{	
	UnHookCode();
	MessageBox(NULL,"Hello Hooking!","Be hooked",MB_OK);
}

int main(int argc, char *argv[]) {
	int number=0;
	
	printf("Input number : ");
	scanf("%d",&number);
	
	HookCode();
	
	printf("Your number is %d",number);
	
	system("pause");
	return 0;
}

void HookCode(void)
{
	HMODULE hMod;
	HANDLE hProc = NULL;
	LPVOID pFunction;
	LPCTSTR procName = "Project1.exe";
	DWORD dwPID = 0xFFFFFFFF;
	DWORD pGapOfAddress;
	BYTE NewBytes[5]={0xE9,0,};
	
	// dll의 handle을 구한 뒤 "printf"의 주소를 구함 
	hMod = GetModuleHandle("msvcrt.dll");
    pFunction = (LPVOID)GetProcAddress(hMod,"printf");
	
	/*
	      JMP 주소 계산 = (이동할 주소) - (현재 주소) - 5(명령어 길이)
	*/ 
	pGapOfAddress = (DWORD)&HookFunc - (DWORD)pFunction - 5;
	
	// 현재 프로세스의 PID구함 
	dwPID = FindProcessID(procName);
	if(dwPID == 0xFFFFFFFF) printf("Process <%s> is not exist.",procName);
	
	// 현재 프로세스 handle을 구함 
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
 	
 	// printf의 원래 5바이트값을 백업 
	memcpy(g_pOrgBytes, pFunction, 5);
	
	// 새로 덮어씌울 5바이트값 
	memcpy(&NewBytes[1], &pGapOfAddress, 4);
	
	// 덮어씌움 
	WriteProcessMemory(hProc, pFunction, NewBytes,5,NULL);
}

void UnHookCode(void)
{
	HMODULE hMod;
	LPVOID pFunction;
	LPCTSTR procName = "Project1.exe";
	DWORD dwPID = 0xFFFFFFFF;
    HANDLE hProc = NULL;
	
	// dll의 handle을 구한 뒤 "printf"의 주소를 구함 
	hMod = GetModuleHandle("msvcrt.dll");
    pFunction = (LPVOID)GetProcAddress(hMod,"printf");  //prointf주소를 구함
    
    // 현재 프로세스의 PID구함 
    dwPID = FindProcessID(procName);
	if(dwPID == 0xFFFFFFFF) printf("Process <%s> is not exist.",procName);
	
	// 현재 프로세스 handle을 구함 
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
    
    // 백업해 둔 원래 5바이트값을 다시 덮어씌움 
	WriteProcessMemory(hProc, pFunction, g_pOrgBytes,5,NULL);
}

DWORD FindProcessID(LPCTSTR szProcessName)
{
    DWORD dwPID = 0xFFFFFFFF;
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;
    
    // Get the enapshot of the system
    pe.dwSize = sizeof( PROCESSENTRY32 );
    hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 );
    
    //find process
    Process32First(hSnapShot, &pe);
    do
    {
        if(!_stricmp(szProcessName, pe.szExeFile))
        {
            dwPID = pe.th32ProcessID;
            break;                            
        }           
    }
    while(Process32Next(hSnapShot, &pe));
    
    CloseHandle(hSnapShot);
    
    return dwPID;
}
