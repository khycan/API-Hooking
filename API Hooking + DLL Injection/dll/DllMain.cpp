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
	MessageBox(NULL,"you are hacked!","�˸�",MB_OK);
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
	
	// dll�� handle�� ���� �� �����Լ��� �ּҸ� ���� 
	hMod = GetModuleHandle("KERNEL32.dll");
    pDestFuncAddr = (LPVOID)GetProcAddress(hMod,"CreateFileW");
	
	/*
	      JMP �ּ� ��� = (�̵��� �ּ�) - (���� �ּ�) - 5(��ɾ� ����)
	*/ 
	pGapOfAddress = (DWORD)&MyHookFunc - (DWORD)pDestFuncAddr - 5;
 	
 	// ������ �ּ��� ���� 5����Ʈ���� ��� 
	memcpy(g_pOrgBytes, pDestFuncAddr, 5);
	
	// ���� ����� 5����Ʈ�� 
	memcpy(&NewBytes[1], &pGapOfAddress, 4);
	
	//  ������ �ּ��� ����޸� ���� ���� 
	/*
		VirtualProtect(������ ���� �ּ�, ������ ũ��(BYTE), ���� �ο��� ����, ���� ������ ������ ����) 
	*/
	VirtualProtect(pDestFuncAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	
	// �� Opcode ����� 
	memcpy(pDestFuncAddr, NewBytes, 5);
	
	// ����޸� ���� ���� 
	VirtualProtect(pDestFuncAddr, 5, dwOldProtect, &dwOldProtect);
}

void UnHookCode(void)
{
	HMODULE hMod;
	HANDLE hProc = NULL;
	// Destination Function Address
	LPVOID pDestFuncAddr;
    DWORD dwOldProtect;
	
	// dll�� handle�� ���� �� �����Լ��� �ּҸ� ���� 
	hMod = GetModuleHandle("USER32.dll");
    pDestFuncAddr = (LPVOID)GetProcAddress(hMod,"PostMessageW");
    
    // ����޸� ���� ���� 
	VirtualProtect(pDestFuncAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	
	// ����� �� Opcode ����� 
	memcpy(pDestFuncAddr, g_pOrgBytes, 5);
	
	// ����޸� ���� ���� 
	VirtualProtect(pDestFuncAddr, 5, dwOldProtect, &dwOldProtect);
}
