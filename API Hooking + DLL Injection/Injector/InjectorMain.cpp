#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

#define DEF_DLL_PATH ("c:\\myhack.dll")

DWORD FindProcessID(LPCTSTR szProcessName);
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllName);

int main(int argc, char *argv[])
{
    DWORD dwPID = 0xFFFFFFFF;
    
    if(argc != 2)
	{
		printf("Injector.exe [ProcessName]");
		return 1;
	}
	
    //find process
    dwPID = FindProcessID(argv[1]);
    if(dwPID == 0xFFFFFFFF)
    {
        printf("There is no <%s> process!\n",argv[1]);
        return 1;         
    }
    
    //nject dll
    InjectDll(dwPID, DEF_DLL_PATH);
    
    system("PAUSE");
    return EXIT_SUCCESS;
}


/*
	���� ���μ����� PID���� ã�� �Լ� 
	������ PID return 
*/
DWORD FindProcessID(LPCTSTR szProcessName)
{
    DWORD dwPID = 0xFFFFFFFF;
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;
    
    // Get the snapshot of the system
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


/*
	���� ���μ����� ����޸𸮸� �Ҵ��Ͽ� DLL ���� 
	������ TRUE return 
*/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllName)
{
    int ForError=0; 
    HANDLE hProcess, hThread;
    HMODULE hMod;
    LPVOID pRemoteBuf;
    DWORD dwBufSize = lstrlen(szDllName) + 1;
    LPTHREAD_START_ROUTINE pThreadProc;
     
    // #1. dwPID �� �̿��Ͽ� ��� ���μ���(notepad.exe)�� HANDLE�� ����
    if( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )  //���н� NULL ������ ���μ����ڵ� 
        return FALSE;
        
    // #2. ��� ���μ���(notepad.exe) �޸𸮿� szDllName(dwBufSize) ũ�⸸ŭ �޸𸮸� �Ҵ� 
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE); //������ �Ҵ�� �޸� �ּ�(��� ���μ��� ����޸�) 
     
    // #3. �Ҵ� ���� �޸𸮿� myhack.dll ��θ� ��
    ForError = WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllName, dwBufSize, NULL);
    if( !ForError )
        return FALSE;
     
     
    // #4. LoadLibraryA() API �ּҸ� ���� 
	hMod = GetModuleHandle("kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod,"LoadLibraryA");
     
    // #5. notepad.exe ���μ����� ������ ���� 
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
     
    CloseHandle(hThread);
    CloseHandle(hProcess);
     
    printf("��� ����"); 
     
    return TRUE; 
}
