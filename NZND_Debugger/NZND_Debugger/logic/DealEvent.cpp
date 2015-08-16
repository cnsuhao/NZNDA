#include <Windows.h>
#include "Debug.h"
#include "DealEvent.h"
#include <vector>

using std::vector;

extern _DEBUG_EVENT	DbgEvt;	
HANDLE g_hProc;//被调试者的句柄





_PROCMSG g_ProcMsg={0x12345678};


bool	Encry(BYTE bFirstKey);
bool	Decry(BYTE bFirstKey);
BYTE XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
void XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
bool	EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock);

//加密
//返回值
/*
true:本块已经加密
*/
bool	EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock)
{
	//如果本段代码已经加密了
	if (true==g_ProcMsg.vbEncryState[dwIndexBlock])
	{
		return true;
	}

	//增加可读可写可执行属性
	RepairProtData(dwIndexBlock);

	BYTE byATemp=0;

	if (0==dwIndexBlock)
	{

		for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
		{
			//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=bFirstKey;

			ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
			byATemp^=bFirstKey;
			WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);


		}
	}
	else
	{
		if(true==g_ProcMsg.vbEncryState[dwIndexBlock-1])
		{
			RepairProtData(dwIndexBlock-1);
			//变形上一个加密段,获取密钥
			BYTE NowKey=XOREncryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],0);	
			//用密钥加密自身
			for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
			{
				//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=NowKey;

				ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
				byATemp^=NowKey;
				WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);

			}
			//恢复上一个被变形的加密段			
			XORDecryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],NowKey);
			RepairProtNoAccess(dwIndexBlock-1);
		}
		else
		{
			EncryOfBlock(bFirstKey,dwIndexBlock-1);//调用自己.
		}
	}
	g_ProcMsg.vbEncryState[dwIndexBlock]=true;
	return true;//已经加密
}


//bool	Encry(BYTE bFirstKey)
//{
//	for (DWORD dwIndexBlock=0;dwIndexBlock<g_ProcMsg.vdLength.size();++dwIndexBlock)
//	{
//		EncryOfBlock(bFirstKey,dwIndexBlock);
//
//	}
//
//	return  true;
//}

//取掉属性
bool RepairProtData(DWORD dwIndex)
{
	DWORD dwRet=0;

	DWORD OldProtect	= 0;//重要的值,用来存放保护属性
	PDWORD lpOldProtect =&OldProtect;

	DWORD OldProtectB	= 0;//一个不会使用的值,用于存放保护属性
	PDWORD  lpOldProtectB=&OldProtectB;

	if(0!=VirtualProtectEx(g_hProc,g_ProcMsg.vpAddress[dwIndex],g_ProcMsg.vdLength[dwIndex],PAGE_EXECUTE_READWRITE,lpOldProtect))
	{
		return true;
	}
	return false;
}

//使指定块内存变成无权限
bool RepairProtNoAccess(DWORD dwIndex)
{
	DWORD dwRet=0;

	DWORD OldProtect	= 0;//重要的值,用来存放保护属性
	PDWORD lpOldProtect =&OldProtect;

	DWORD OldProtectB	= 0;//一个不会使用的值,用于存放保护属性
	PDWORD  lpOldProtectB=&OldProtectB;

	if(0!=VirtualProtectEx(g_hProc,g_ProcMsg.vpAddress[dwIndex],g_ProcMsg.vdLength[dwIndex],PAGE_NOACCESS,lpOldProtect))
	{
		return true;
	}
	return false;
}





//本进程解密版
bool	Decry(DWORD dwIndexBlock,BYTE bFirstKey)
{
	if(dwIndexBlock>=g_ProcMsg.vdLength.size())
	{
		return false;
	}

	BYTE byATemp=0;

	if (0==dwIndexBlock)
	{
		for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
		{
			//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=bFirstKey;

			ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
			byATemp^=bFirstKey;
			WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);


		}
	}
	else
	{
		//先加密上一个块

		if( false==g_ProcMsg.vbEncryState[dwIndexBlock-1])
		{
			if (1==dwIndexBlock)
			{
				for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
				{
					//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=bFirstKey;

					ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
					byATemp^=bFirstKey;
					WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);


				}
			}
			else
			{
				EncryOfBlock(bFirstKey,dwIndexBlock-1);//加密上一个块.

				//变形上一个加密段,获取密钥
				BYTE NowKey=XOREncryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],0);	
				//用密钥加密自身
				for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
				{
					//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=NowKey;

					ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
					byATemp^=NowKey;
					WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);

				}
				//恢复上一个被变形的加密段
				XORDecryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],NowKey);
				RepairProtNoAccess(dwIndexBlock-1);
			}
		}

		//变形上一个加密段,获取密钥
		BYTE NowKey=XOREncryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],0);	
		//用密钥解密自身
		for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
		{
			//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=NowKey;

			ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
			byATemp^=NowKey;
			WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);

		}
		//恢复上一个被变形的加密段
		XORDecryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],NowKey);
		RepairProtNoAccess(dwIndexBlock-1);
		return true;
	}
	return true;
}





BYTE XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//加密得到密钥
{
	BYTE byATemp=0;

	for (DWORD d=0;d<dwSize;++d)
	{
		//lpBuffer[d]=lpBuffer[d]+bKey;
		//bKey=lpBuffer[d]^bKey;

		ReadDebuggedMemory(lpBuffer,1,&byATemp);
		byATemp+=bKey;
		bKey^=byATemp;
		WriteProcessMemory(g_hProc,lpBuffer,&byATemp,1,NULL);

	}
	return bKey;
}


void XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//解密还原
{
	BYTE byATemp=0;

	for (signed int d=(dwSize-1);d>=0;--d)
	{
		bKey=lpBuffer[d]^bKey;
		lpBuffer[d]=lpBuffer[d]-bKey;
		ReadDebuggedMemory(lpBuffer,1,&byATemp);
		bKey^=byATemp;
		byATemp-=bKey;
		WriteProcessMemory(g_hProc,lpBuffer,&byATemp,1,NULL);

	}
}




//进程创建处理.
bool Deal_CPEV()
{
	OutputDebug(L"进程创建事件");

	g_hProc   = OpenProcess(PROCESS_ALL_ACCESS,0,DbgEvt.dwProcessId);
	if (NULL==g_hProc)
	{
		OutputDebug(L"打开进程失败,进程为:%p,FILE: %s, LINE: %d…",DbgEvt.dwProcessId,__FILE__,__LINE__);
		return false;
	}


	return true;
}



//搜索指定地址是否存在于已记录ProtIndex的列表,存在则返回下标.-1没找到.
int SerarchProtIndex(LPVOID lpAddress)
{
	vector<void*>::iterator it;

	int iIndex=0;//序号

	if (lpAddress>=g_ProcMsg.vpAddress[0])
	{
		LPVOID lpMaxProtectAddr=(LPVOID)((DWORD)g_ProcMsg.vpAddress[g_ProcMsg.vpAddress.size()-1]+g_ProcMsg.vdLength[g_ProcMsg.vdLength.size()-1]);

		if(lpAddress<=lpMaxProtectAddr)

		{//从begin()到end()-1,如果找不到则在最后一个块
			for (it=g_ProcMsg.vpAddress.begin();it<g_ProcMsg.vpAddress.end()-1;++it,++iIndex)
			{
				if (*it<=lpAddress&&lpAddress>=*(it+1))
				{
					return iIndex;
				}
			}
			return g_ProcMsg.vpAddress.size()-1;//如果找不到则在最后一个块
		}
	}
	return -1;//空,或者找不到

}




DWORD OnExceptionDebugEvent(LPEXCEPTION_DEBUG_INFO pDbgInfo)
{

	EXCEPTION_RECORD* pstcExp = &pDbgInfo->ExceptionRecord;

	DWORD	dwExpCode	= pstcExp->ExceptionCode;	//异常码
	PVOID	lpExpAddr	= pstcExp->ExceptionAddress;//异常地址

	switch (dwExpCode)
	{
		//非法访问异常
	case EXCEPTION_ACCESS_VIOLATION:
		{
			OutputDebug(L"AV ERROR:Addr:0x%p",lpExpAddr);

			/*
			查找当前异常地址是否在加密区间.



			如果在,返回加密代码块Index.

			去除该加密代码块的保护属性.

			根据Index进行处理进行解密.

			注意要+保护属性.

			*/

			int iProtIndex=SerarchProtIndex(lpExpAddr);

			if (-1!=iProtIndex)
			{
				//增加可读可写可执行属性
				RepairProtData(iProtIndex);
				//解密
				Decry(iProtIndex,g_ProcMsg.dwStact);

				return DBG_CONTINUE;//异常已处理
			}





			break;
		}
		//断点异常
	case EXCEPTION_BREAKPOINT:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
		}
		//内存对齐异常
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		{
			break;
		}
		//无效指令
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		{
			break;
		}
		//除零错误
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		{
			break;
		}
		//指令不支持当前模式
	case EXCEPTION_PRIV_INSTRUCTION:
		{
			break;
		}
		//单步或硬件断点异常
	case EXCEPTION_SINGLE_STEP:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
			break;
		}

	default:
		break;
	}


	return DBG_EXCEPTION_NOT_HANDLED;////DBG_CONTINUE//异常已经处理

}




//被调试者退出
bool Deal_EPDE()
{
	OutputDebug(L"被调试者退出");
	return true;
}

/*
//用途:
通用读被调试者内存函数.读失败不做改变权限尝试.
//参数
LPVOID	lpAddress	//起始读地址
DWORD	dwGetNumber	//希望获得的内存内容字节数量.
_Out_	BYTE* wcGetValue	//用于存放内容的地方.例如:BYTE byA[20]中的byA;
*/

bool ReadDebuggedMemory(LPVOID lpAddress,DWORD dwGetNumber,_Out_ BYTE* wcGetValue)
{

	DWORD	dwRetN		= 0;

	if(!ReadProcessMemory(g_hProc,lpAddress,wcGetValue,dwGetNumber,&dwRetN))
	{
		return false;
	}
	return true;
}