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
	DWORD dwStact;            //一个标志
	DWORD dwImageBase;   //源程序的基址
	DWORD dwImportRVA;    //源程序导入表的位置
	DWORD dwEntryRVA;      //源程序入口点
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
		  //得到IAT
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
	//我们的工作在这里
	WorkOfKitty();

    ReLoadImportApi();
	//原解密代码
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


//获取kernel32基地址和用到的函数,创建信号量,获取PID,创建进程,等待信号量
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
* 获取Kernel32.dll的模块基址
*     此函数会通过读取PEB相关信息的方式获取Kernel32.dll的模块基址。但
* 需要注意的是，此函数在NT 6.1内核中获取的将会是KernelBase.dll的模块
* 基址，但是并不会对我们的使用产生太多影响。
*     在NT 6.1中获取基础API函数地址时，我们只需要注意KernelBase.dll
* 没有LoadLibrary，只有LoadLibraryEx这一特点就可以了。
*
* 参数：无
* 
* 返回值：
* DWORD：Kernel32.dll的模块基址。
*********************************************************************/
DWORD GetKernel32Base()
{
	DWORD dwKernel32Addr = 0;
	__asm
	{
		push eax
			mov eax,dword ptr fs:[0x30] // eax = PEB的地址
		mov eax,[eax+0x0C]          // eax = 指向PEB_LDR_DATA结构的指针
		mov eax,[eax+0x1C]          // eax = 模块初始化链表的头指针InInitializationOrderModuleList
		mov eax,[eax]               // eax = 列表中的第二个条目
		mov eax,[eax+0x08]          // eax = 获取到的Kernel32.dll基址（Win7下获取的是KernelBase.dll的基址）
		mov dwKernel32Addr,eax
			pop eax
	}

	return dwKernel32Addr;
}

/*********************************************************************
* 获取GetProcAddress的函数地址
*     此函数会通过读取Kernel32.dll的模块的相关映像信息读取其输出表。然
* 后通过遍历输出表查找“GetProcAddress”，并由此计算得出其函数地址。
*
* 参数：无
* 
* 返回值：
* DWORD：GetProcAddress的函数基址，返回0则代表获取失败。
*********************************************************************/
DWORD GetGPAFunAddr()
{
	DWORD dwAddrBase = GetKernel32Base();

	// 1. 获取DOS头、NT头
	PIMAGE_DOS_HEADER pDos_Header;
	PIMAGE_NT_HEADERS pNt_Header;
	pDos_Header = (PIMAGE_DOS_HEADER)dwAddrBase;
	pNt_Header  = (PIMAGE_NT_HEADERS)(dwAddrBase + pDos_Header->e_lfanew);

	// 2. 获取导出表项
	PIMAGE_DATA_DIRECTORY   pDataDir;
	PIMAGE_EXPORT_DIRECTORY pExport;
	pDataDir = pNt_Header->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_EXPORT;
	pExport  = (PIMAGE_EXPORT_DIRECTORY)(dwAddrBase + pDataDir->VirtualAddress);

	// 3. 获取导出表详细信息
	PDWORD pAddrOfFun      = (PDWORD)(pExport->AddressOfFunctions    + dwAddrBase);
	PDWORD pAddrOfNames    = (PDWORD)(pExport->AddressOfNames        + dwAddrBase);
	PWORD  pAddrOfOrdinals = (PWORD) (pExport->AddressOfNameOrdinals + dwAddrBase);

	// 4. 处理以函数名查找函数地址的请求，循环获取ENT中的函数名，并与传入值对比对，如能匹配上
	//    则在EAT中以指定序号作为索引，并取出其地址值。
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
* 初始化将要用到的API
*     此函数会初始化将要使用的API信息。由于Stub部分在被植入到其他程序中
* 后会丢弃掉导入表信息，因此只能通过动态获取API地址的方式调用API，而这
* 里的“初始化”的意思就是获取各个简要使用的API地址。
*
* 参数：无
* 
* 返回值：
* bool：成功返回true，失败则返回false。
*********************************************************************/
bool InitializationAPI()
{
	HMODULE hModule;

	// 1. 初始化基础API
	g_funGetProcAddress = (LPGETPROCADDRESS)GetGPAFunAddr();
	g_funLoadLibraryEx  = (LPLOADLIBRARYEX)g_funGetProcAddress((HMODULE)GetKernel32Base(),"LoadLibraryExW");

	// 2. 初始化其他API

	hModule = NULL;
	g_funIsDebuggerPresent = (LPIsDebuggerPresent)g_funGetProcAddress(hModule,"VirtualProtect");
	return true;
}
