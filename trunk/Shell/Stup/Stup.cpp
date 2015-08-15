// Stup.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
            

#pragma comment(linker,"/ENTRY:LMain")
#pragma comment(linker,"/MERGE:.data=.text")
#pragma comment(linker,"/FILEALIGN:0x200")


bool InitializationAPI();
DWORD GetGPAFunAddr();
DWORD GetKernel32Base();
bool WorkOfKitty();



typedef bool (WINAPI *LPIsDebuggerPresent)();           // LPIsDebuggerPresent
LPIsDebuggerPresent  g_funIsDebuggerPresent  = nullptr;
typedef DWORD (WINAPI *LPGETPROCADDRESS)(HMODULE,LPCSTR);        // GetProcAddress
LPGETPROCADDRESS  g_funGetProcAddress  = nullptr;
typedef HMODULE (WINAPI *LPLOADLIBRARYEX)(LPCTSTR,HANDLE,DWORD); // LoadLibaryEx
LPLOADLIBRARYEX   g_funLoadLibraryEx   = nullptr;

struct _PROCMSG
{
	DWORD dwStact;            //һ����־
	DWORD dwImageBase;   //Դ����Ļ�ַ
	DWORD dwImportRVA;    //Դ��������λ��
	DWORD dwEntryRVA;      //Դ������ڵ�
};

_PROCMSG g_ProcMsg={0x12345678};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

BYTE *  RvaToPtr(DWORD dwRVA)
{
    BYTE *buf=(BYTE*)g_ProcMsg.dwImageBase;
    return buf+dwRVA;
}

void ReLoadImportApi()
{
      PIMAGE_IMPORT_DESCRIPTOR pTempImport=(_IMAGE_IMPORT_DESCRIPTOR*)RvaToPtr(g_ProcMsg.dwImportRVA);
	  while(pTempImport->FirstThunk)
	  {
		  PIMAGE_THUNK_DATA32 pOriginalFirstThunk=(PIMAGE_THUNK_DATA32)RvaToPtr(pTempImport->OriginalFirstThunk);
		  //�õ�IAT
		  DWORD pFirstThunk=(DWORD)RvaToPtr(pTempImport->FirstThunk);
		  char *pstrDllName=(char*)RvaToPtr(pTempImport->Name);
		  HMODULE hMod=LoadLibrary(pstrDllName);
		  while(pOriginalFirstThunk->u1.Ordinal!=NULL)
		  {
			  if (IMAGE_SNAP_BY_ORDINAL32(pOriginalFirstThunk->u1.Ordinal))
			  {
				  *((DWORD*)pFirstThunk)=(DWORD)GetProcAddress(hMod,(const char*)\
					  (pOriginalFirstThunk->u1.Function&0x0000FFFF));
			  }else{
				  PIMAGE_IMPORT_BY_NAME pImportName=(PIMAGE_IMPORT_BY_NAME)\
					  RvaToPtr((DWORD)pOriginalFirstThunk->u1.AddressOfData);
				  *((DWORD*)pFirstThunk)=(DWORD)GetProcAddress(hMod,(char*)pImportName->Name);
			  }
			  pOriginalFirstThunk++;
			  pFirstThunk+=4;
		  }
		  ++pTempImport;
	  }
}




//void XORData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)
//{
//
//	for (DWORD d=0;d<dwSize;d++)
//	{
//		lpBuffer[d]^=bKey;
//	}
//}


void Entry()
{
	//���ǵĹ���������
	WorkOfKitty();

    ReLoadImportApi();
	//ԭ���ܴ���
	//BYTE* lpBuffer=RvaToPtr(g_ProcMsg.dwEntryRVA);
    //XORData(lpBuffer,0x100,g_ProcMsg.dwStact);
}

extern "C"__declspec(naked) void LMain()
{
	__asm  pushad
   //MessageBox(NULL,"ok","ok",NULL);
    Entry();
	__asm{
		popad
        mov eax,g_ProcMsg.dwImageBase;
		add eax,g_ProcMsg.dwEntryRVA;
		jmp eax
	}
	
}


//��ȡkernel32����ַ���õ��ĺ���,�����ź���,��ȡPID,��������,�ȴ��ź���
bool WorkOfKitty()
{
	InitializationAPI();
	while(0==g_funIsDebuggerPresent())
	{
		int iTempA=5;
		_asm
		{
			mov iTempA,eax;
			mov iTempA,ebx;
		}
	}

	return true;
}

/*********************************************************************
* ��ȡKernel32.dll��ģ���ַ
*     �˺�����ͨ����ȡPEB�����Ϣ�ķ�ʽ��ȡKernel32.dll��ģ���ַ����
* ��Ҫע����ǣ��˺�����NT 6.1�ں��л�ȡ�Ľ�����KernelBase.dll��ģ��
* ��ַ�����ǲ���������ǵ�ʹ�ò���̫��Ӱ�졣
*     ��NT 6.1�л�ȡ����API������ַʱ������ֻ��Ҫע��KernelBase.dll
* û��LoadLibrary��ֻ��LoadLibraryEx��һ�ص�Ϳ����ˡ�
*
* ��������
* 
* ����ֵ��
* DWORD��Kernel32.dll��ģ���ַ��
*********************************************************************/
DWORD GetKernel32Base()
{
	DWORD dwKernel32Addr = 0;
	__asm
	{
		push eax
			mov eax,dword ptr fs:[0x30] // eax = PEB�ĵ�ַ
		mov eax,[eax+0x0C]          // eax = ָ��PEB_LDR_DATA�ṹ��ָ��
		mov eax,[eax+0x1C]          // eax = ģ���ʼ�������ͷָ��InInitializationOrderModuleList
		mov eax,[eax]               // eax = �б��еĵڶ�����Ŀ
		mov eax,[eax+0x08]          // eax = ��ȡ����Kernel32.dll��ַ��Win7�»�ȡ����KernelBase.dll�Ļ�ַ��
		mov dwKernel32Addr,eax
			pop eax
	}

	return dwKernel32Addr;
}

/*********************************************************************
* ��ȡGetProcAddress�ĺ�����ַ
*     �˺�����ͨ����ȡKernel32.dll��ģ������ӳ����Ϣ��ȡ�������Ȼ
* ��ͨ�������������ҡ�GetProcAddress�������ɴ˼���ó��亯����ַ��
*
* ��������
* 
* ����ֵ��
* DWORD��GetProcAddress�ĺ�����ַ������0������ȡʧ�ܡ�
*********************************************************************/
DWORD GetGPAFunAddr()
{
	DWORD dwAddrBase = GetKernel32Base();

	// 1. ��ȡDOSͷ��NTͷ
	PIMAGE_DOS_HEADER pDos_Header;
	PIMAGE_NT_HEADERS pNt_Header;
	pDos_Header = (PIMAGE_DOS_HEADER)dwAddrBase;
	pNt_Header  = (PIMAGE_NT_HEADERS)(dwAddrBase + pDos_Header->e_lfanew);

	// 2. ��ȡ��������
	PIMAGE_DATA_DIRECTORY   pDataDir;
	PIMAGE_EXPORT_DIRECTORY pExport;
	pDataDir = pNt_Header->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_EXPORT;
	pExport  = (PIMAGE_EXPORT_DIRECTORY)(dwAddrBase + pDataDir->VirtualAddress);

	// 3. ��ȡ��������ϸ��Ϣ
	PDWORD pAddrOfFun      = (PDWORD)(pExport->AddressOfFunctions    + dwAddrBase);
	PDWORD pAddrOfNames    = (PDWORD)(pExport->AddressOfNames        + dwAddrBase);
	PWORD  pAddrOfOrdinals = (PWORD) (pExport->AddressOfNameOrdinals + dwAddrBase);

	// 4. �����Ժ��������Һ�����ַ������ѭ����ȡENT�еĺ����������봫��ֵ�Աȶԣ�����ƥ����
	//    ����EAT����ָ�������Ϊ��������ȡ�����ֵַ��
	DWORD dwFunAddr;
	for (DWORD i=0; i<pExport->NumberOfNames; i++)
	{
		PCHAR lpFunName = (PCHAR)(pAddrOfNames[i]+dwAddrBase);
		if ( !strcmp(lpFunName, "GetProcAddress") )
		{
			dwFunAddr = pAddrOfFun[pAddrOfOrdinals[i]] + dwAddrBase;
			break;
		}
		if ( i == pExport->NumberOfNames-1 )
			return 0; 
	}

	return dwFunAddr;
}

/*********************************************************************
* ��ʼ����Ҫ�õ���API
*     �˺������ʼ����Ҫʹ�õ�API��Ϣ������Stub�����ڱ�ֲ�뵽����������
* ��ᶪ�����������Ϣ�����ֻ��ͨ����̬��ȡAPI��ַ�ķ�ʽ����API������
* ��ġ���ʼ��������˼���ǻ�ȡ������Ҫʹ�õ�API��ַ��
*
* ��������
* 
* ����ֵ��
* bool���ɹ�����true��ʧ���򷵻�false��
*********************************************************************/
bool InitializationAPI()
{
	HMODULE hModule;

	// 1. ��ʼ������API
	g_funGetProcAddress = (LPGETPROCADDRESS)GetGPAFunAddr();
	g_funLoadLibraryEx  = (LPLOADLIBRARYEX)g_funGetProcAddress((HMODULE)GetKernel32Base(),"LoadLibraryExW");

	// 2. ��ʼ������API

	hModule = NULL;
	g_funIsDebuggerPresent = (LPIsDebuggerPresent)g_funGetProcAddress(hModule,"VirtualProtect");
	return true;
}
