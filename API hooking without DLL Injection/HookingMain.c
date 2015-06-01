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
	
	// dll�� handle�� ���� �� "printf"�� �ּҸ� ���� 
	hMod = GetModuleHandle("msvcrt.dll");
    pFunction = (LPVOID)GetProcAddress(hMod,"printf");
	
	/*
	      JMP �ּ� ��� = (�̵��� �ּ�) - (���� �ּ�) - 5(��ɾ� ����)
	*/ 
	pGapOfAddress = (DWORD)&HookFunc - (DWORD)pFunction - 5;
	
	// ���� ���μ����� PID���� 
	dwPID = FindProcessID(procName);
	if(dwPID == 0xFFFFFFFF) printf("Process <%s> is not exist.",procName);
	
	// ���� ���μ��� handle�� ���� 
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
 	
 	// printf�� ���� 5����Ʈ���� ��� 
	memcpy(g_pOrgBytes, pFunction, 5);
	
	// ���� ����� 5����Ʈ�� 
	memcpy(&NewBytes[1], &pGapOfAddress, 4);
	
	// ����� 
	WriteProcessMemory(hProc, pFunction, NewBytes,5,NULL);
}

void UnHookCode(void)
{
	HMODULE hMod;
	LPVOID pFunction;
	LPCTSTR procName = "Project1.exe";
	DWORD dwPID = 0xFFFFFFFF;
    HANDLE hProc = NULL;
	
	// dll�� handle�� ���� �� "printf"�� �ּҸ� ���� 
	hMod = GetModuleHandle("msvcrt.dll");
    pFunction = (LPVOID)GetProcAddress(hMod,"printf");  //prointf�ּҸ� ����
    
    // ���� ���μ����� PID���� 
    dwPID = FindProcessID(procName);
	if(dwPID == 0xFFFFFFFF) printf("Process <%s> is not exist.",procName);
	
	// ���� ���μ��� handle�� ���� 
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
    
    // ����� �� ���� 5����Ʈ���� �ٽ� ����� 
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
