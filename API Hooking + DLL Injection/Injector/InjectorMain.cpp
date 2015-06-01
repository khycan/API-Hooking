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
	목적 프로세스의 PID값을 찾는 함수 
	성공시 PID return 
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
	목적 프로세스에 가상메모리를 할당하여 DLL 삽입 
	성공시 TRUE return 
*/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllName)
{
    int ForError=0; 
    HANDLE hProcess, hThread;
    HMODULE hMod;
    LPVOID pRemoteBuf;
    DWORD dwBufSize = lstrlen(szDllName) + 1;
    LPTHREAD_START_ROUTINE pThreadProc;
     
    // dwPID 를 이용하여 대상 프로세스의 HANDLE을 구함
    if( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )  //실패시 NULL 성공시 프로세스핸들 
        return FALSE;
        
    // 대상 프로세스메모리에 szDllName(dwBufSize) 크기만큼 메모리를 할당 
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE); //성공시 할당된 메모리 주소(대상 프로세스 가상메모리) 
     
    // 할당 받은 메모리에 myhack.dll 경로를 씀
    ForError = WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllName, dwBufSize, NULL);
    if( !ForError )
        return FALSE;
     
     
    // LoadLibraryA() API 주소를 구함 
	hMod = GetModuleHandle("kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod,"LoadLibraryA");
     
    // 대상프로세스에 스레드를 실행 
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
     
    CloseHandle(hThread);
    CloseHandle(hProcess);
     
    printf("모두 끝남"); 
     
    return TRUE; 
}
