#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef BOOL (WINAPI *Hpost)(HANDLE,UINT,WPARAM,LPARAM);

void HookCode(void);
void UnHookCode(void);


BYTE g_pOrgBytes[5] = {0,};

void MyHookFunc(void)
{
	UnHookCode();
	MessageBox(NULL,"you are hacked!","알림",MB_OK);
}

DWORD WINAPI ThreadProc(LPVOID lParam)
{
      
    HookCode();
      
    return 0;      
}

BOOL APIENTRY DllMain (HINSTANCE hinstDLL     /* Library instance handle. */ ,
                       DWORD fdwReason        /* Reason this function is being called. */ ,
                       LPVOID lpvReserved     /* Not used. */ )
{
    HANDLE hThread = NULL;
    
    switch (fdwReason)
    {
      case DLL_PROCESS_ATTACH:
        hThread = CreateThread(NULL,0,ThreadProc,NULL,0,NULL);
        CloseHandle(hThread);
        break;

      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}

void HookCode(void)
{
	HMODULE hMod;
	HANDLE hProc = NULL;
	//Destination Function Address
	LPVOID pDestFuncAddr;
	DWORD pGapOfAddress;
	DWORD dwOldProtect;
	BYTE NewBytes[5]={0xE9,0,};
	
	// dll의 handle을 구한 뒤 목적함수의 주소를 구함 
	hMod = GetModuleHandle("KERNEL32.dll");
    pDestFuncAddr = (LPVOID)GetProcAddress(hMod,"CreateFileW");
	
	/*
	      JMP 주소 계산 = (이동할 주소) - (현재 주소) - 5(명령어 길이)
	*/ 
	pGapOfAddress = (DWORD)&MyHookFunc - (DWORD)pDestFuncAddr - 5;
 	
 	// 목적지 주소의 원래 5바이트값을 백업 
	memcpy(g_pOrgBytes, pDestFuncAddr, 5);
	
	// 새로 덮어씌울 5바이트값 
	memcpy(&NewBytes[1], &pGapOfAddress, 4);
	
	//  목적지 주소의 가상메모리 권한 변경 
	/*
		VirtualProtect(변경할 시작 주소, 변경할 크기(BYTE), 새로 부여할 권한, 기존 권한을 저장할 공간) 
	*/
	VirtualProtect(pDestFuncAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	
	// 새 Opcode 덮어씌움 
	memcpy(pDestFuncAddr, NewBytes, 5);
	
	// 가상메모리 권한 복원 
	VirtualProtect(pDestFuncAddr, 5, dwOldProtect, &dwOldProtect);
}

void UnHookCode(void)
{
	HMODULE hMod;
	HANDLE hProc = NULL;
	// Destination Function Address
	LPVOID pDestFuncAddr;
    DWORD dwOldProtect;
	
	// dll의 handle을 구한 뒤 목적함수의 주소를 구함 
	hMod = GetModuleHandle("USER32.dll");
    pDestFuncAddr = (LPVOID)GetProcAddress(hMod,"PostMessageW");
    
    // 가상메모리 권한 변경 
	VirtualProtect(pDestFuncAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	
	// 백업해 둔 Opcode 덮어씌움 
	memcpy(pDestFuncAddr, g_pOrgBytes, 5);
	
	// 가상메모리 권한 복원 
	VirtualProtect(pDestFuncAddr, 5, dwOldProtect, &dwOldProtect);
}
