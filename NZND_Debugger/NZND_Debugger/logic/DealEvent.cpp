#include <Windows.h>
#include "Debug.h"
#include "DealEvent.h"
#include <vector>

using std::vector;

extern _DEBUG_EVENT	DbgEvt;	
HANDLE g_hProc;//�������ߵľ��





_PROCMSG g_ProcMsg={0x12345678};


bool	Encry(BYTE bFirstKey);
bool	Decry(BYTE bFirstKey);
BYTE XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
void XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
bool	EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock);

//����
//����ֵ
/*
true:�����Ѿ�����
*/
bool	EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock)
{
	//������δ����Ѿ�������
	if (true==g_ProcMsg.vbEncryState[dwIndexBlock])
	{
		return true;
	}

	//���ӿɶ���д��ִ������
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
			//������һ�����ܶ�,��ȡ��Կ
			BYTE NowKey=XOREncryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],0);	
			//����Կ��������
			for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
			{
				//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=NowKey;

				ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
				byATemp^=NowKey;
				WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);

			}
			//�ָ���һ�������εļ��ܶ�			
			XORDecryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],NowKey);
			RepairProtNoAccess(dwIndexBlock-1);
		}
		else
		{
			EncryOfBlock(bFirstKey,dwIndexBlock-1);//�����Լ�.
		}
	}
	g_ProcMsg.vbEncryState[dwIndexBlock]=true;
	return true;//�Ѿ�����
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

//ȡ������
bool RepairProtData(DWORD dwIndex)
{
	DWORD dwRet=0;

	DWORD OldProtect	= 0;//��Ҫ��ֵ,������ű�������
	PDWORD lpOldProtect =&OldProtect;

	DWORD OldProtectB	= 0;//һ������ʹ�õ�ֵ,���ڴ�ű�������
	PDWORD  lpOldProtectB=&OldProtectB;

	if(0!=VirtualProtectEx(g_hProc,g_ProcMsg.vpAddress[dwIndex],g_ProcMsg.vdLength[dwIndex],PAGE_EXECUTE_READWRITE,lpOldProtect))
	{
		return true;
	}
	return false;
}

//ʹָ�����ڴ�����Ȩ��
bool RepairProtNoAccess(DWORD dwIndex)
{
	DWORD dwRet=0;

	DWORD OldProtect	= 0;//��Ҫ��ֵ,������ű�������
	PDWORD lpOldProtect =&OldProtect;

	DWORD OldProtectB	= 0;//һ������ʹ�õ�ֵ,���ڴ�ű�������
	PDWORD  lpOldProtectB=&OldProtectB;

	if(0!=VirtualProtectEx(g_hProc,g_ProcMsg.vpAddress[dwIndex],g_ProcMsg.vdLength[dwIndex],PAGE_NOACCESS,lpOldProtect))
	{
		return true;
	}
	return false;
}





//�����̽��ܰ�
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
		//�ȼ�����һ����

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
				EncryOfBlock(bFirstKey,dwIndexBlock-1);//������һ����.

				//������һ�����ܶ�,��ȡ��Կ
				BYTE NowKey=XOREncryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],0);	
				//����Կ��������
				for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
				{
					//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=NowKey;

					ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
					byATemp^=NowKey;
					WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);

				}
				//�ָ���һ�������εļ��ܶ�
				XORDecryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],NowKey);
				RepairProtNoAccess(dwIndexBlock-1);
			}
		}

		//������һ�����ܶ�,��ȡ��Կ
		BYTE NowKey=XOREncryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],0);	
		//����Կ��������
		for (DWORD dSize=0;dSize<g_ProcMsg.vdLength[dwIndexBlock];++dSize)
		{
			//((BYTE*)(g_ProcMsg.vpAddress[dwIndexBlock]))[dSize]^=NowKey;

			ReadDebuggedMemory(g_ProcMsg.vpAddress[dwIndexBlock],1,&byATemp);
			byATemp^=NowKey;
			WriteProcessMemory(g_hProc,g_ProcMsg.vpAddress[dwIndexBlock],&byATemp,1,NULL);

		}
		//�ָ���һ�������εļ��ܶ�
		XORDecryData((BYTE*)g_ProcMsg.vpAddress[dwIndexBlock-1],g_ProcMsg.vdLength[dwIndexBlock-1],NowKey);
		RepairProtNoAccess(dwIndexBlock-1);
		return true;
	}
	return true;
}





BYTE XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//���ܵõ���Կ
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


void XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//���ܻ�ԭ
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




//���̴�������.
bool Deal_CPEV()
{
	OutputDebug(L"���̴����¼�");

	g_hProc   = OpenProcess(PROCESS_ALL_ACCESS,0,DbgEvt.dwProcessId);
	if (NULL==g_hProc)
	{
		OutputDebug(L"�򿪽���ʧ��,����Ϊ:%p,FILE: %s, LINE: %d��",DbgEvt.dwProcessId,__FILE__,__LINE__);
		return false;
	}


	return true;
}



//����ָ����ַ�Ƿ�������Ѽ�¼ProtIndex���б�,�����򷵻��±�.-1û�ҵ�.
int SerarchProtIndex(LPVOID lpAddress)
{
	vector<void*>::iterator it;

	int iIndex=0;//���

	if (lpAddress>=g_ProcMsg.vpAddress[0])
	{
		LPVOID lpMaxProtectAddr=(LPVOID)((DWORD)g_ProcMsg.vpAddress[g_ProcMsg.vpAddress.size()-1]+g_ProcMsg.vdLength[g_ProcMsg.vdLength.size()-1]);

		if(lpAddress<=lpMaxProtectAddr)

		{//��begin()��end()-1,����Ҳ����������һ����
			for (it=g_ProcMsg.vpAddress.begin();it<g_ProcMsg.vpAddress.end()-1;++it,++iIndex)
			{
				if (*it<=lpAddress&&lpAddress>=*(it+1))
				{
					return iIndex;
				}
			}
			return g_ProcMsg.vpAddress.size()-1;//����Ҳ����������һ����
		}
	}
	return -1;//��,�����Ҳ���

}




DWORD OnExceptionDebugEvent(LPEXCEPTION_DEBUG_INFO pDbgInfo)
{

	EXCEPTION_RECORD* pstcExp = &pDbgInfo->ExceptionRecord;

	DWORD	dwExpCode	= pstcExp->ExceptionCode;	//�쳣��
	PVOID	lpExpAddr	= pstcExp->ExceptionAddress;//�쳣��ַ

	switch (dwExpCode)
	{
		//�Ƿ������쳣
	case EXCEPTION_ACCESS_VIOLATION:
		{
			OutputDebug(L"AV ERROR:Addr:0x%p",lpExpAddr);

			/*
			���ҵ�ǰ�쳣��ַ�Ƿ��ڼ�������.



			�����,���ؼ��ܴ����Index.

			ȥ���ü��ܴ����ı�������.

			����Index���д�����н���.

			ע��Ҫ+��������.

			*/

			int iProtIndex=SerarchProtIndex(lpExpAddr);

			if (-1!=iProtIndex)
			{
				//���ӿɶ���д��ִ������
				RepairProtData(iProtIndex);
				//����
				Decry(iProtIndex,g_ProcMsg.dwStact);

				return DBG_CONTINUE;//�쳣�Ѵ���
			}





			break;
		}
		//�ϵ��쳣
	case EXCEPTION_BREAKPOINT:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
		}
		//�ڴ�����쳣
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		{
			break;
		}
		//��Чָ��
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		{
			break;
		}
		//�������
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		{
			break;
		}
		//ָ�֧�ֵ�ǰģʽ
	case EXCEPTION_PRIV_INSTRUCTION:
		{
			break;
		}
		//������Ӳ���ϵ��쳣
	case EXCEPTION_SINGLE_STEP:
		{
			OutputDebug(L"Int3 Break:Addr:0x%p",lpExpAddr);
			break;
		}

	default:
		break;
	}


	return DBG_EXCEPTION_NOT_HANDLED;////DBG_CONTINUE//�쳣�Ѿ�����

}




//���������˳�
bool Deal_EPDE()
{
	OutputDebug(L"���������˳�");
	return true;
}

/*
//��;:
ͨ�ö����������ڴ溯��.��ʧ�ܲ����ı�Ȩ�޳���.
//����
LPVOID	lpAddress	//��ʼ����ַ
DWORD	dwGetNumber	//ϣ����õ��ڴ������ֽ�����.
_Out_	BYTE* wcGetValue	//���ڴ�����ݵĵط�.����:BYTE byA[20]�е�byA;
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