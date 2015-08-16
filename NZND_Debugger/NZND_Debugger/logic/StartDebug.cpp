
#include <Windows.h>
#include "Debug.h"
#include "DealEvent.h"
#include <TlHelp32.h>
#include <vector>
#include "StartDebug.h"

using std::vector;


_DEBUG_EVENT	DbgEvt	= {0};		
DWORD	dwRet			= DBG_EXCEPTION_NOT_HANDLED;//δ����!!!!!!!!!!!dwRet�д���ʹ�õĴ����ƺ�
STARTUPINFO			stcStartupInfo	= {sizeof(STARTUPINFO)};
PROCESS_INFORMATION stcProcInfo		= {0};//������Ϣ
extern HANDLE g_hProc;//�������ߵľ��
vector<ThreadInformation> vTI;//�������ߵ��߳���Ϣ
const wchar_t MUTEX_NAME[] = L"NZND_MUTEX";

int StartDebug(bool bNewProcess,void *Infor);
bool GetEvent();


//////////////////////////////////////////////////////////////////////////
//bool bNewProcess	:�Ե��Եķ�ʽ����һ������(true),���ӵ�һ������(false)
//void *Infor		:ָ��һ��wchar_t�ַ�������DWORDֵ,��ȡ����bNewProcess.
int StartDebug(bool bNewProcess,DWORD dwPID)
{


	if (!bNewProcess)
	{
		if(0==DebugActiveProcess(dwPID))
		{
			return -1;
		}//����DWORDֵ,�����Ǵ���DWORDָ��.
	}

	
	//HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, MUTEX_NAME); //�򿪻�����
	//CloseHandle(hMutex);//�رջ�������ʾ���Ѿ�׼������.


	GetEvent();

	return 1;
}

bool GetEvent()
{
	while(1==WaitForDebugEvent(&DbgEvt,INFINITE))
	{


		switch (DbgEvt.dwDebugEventCode)
		{		
			//�����쳣
		case EXCEPTION_DEBUG_EVENT:
			{
				dwRet= OnExceptionDebugEvent(&DbgEvt.u.Exception);
				break;
			}	
			//���̴���
		case CREATE_PROCESS_DEBUG_EVENT:
			{
				Deal_CPEV();
				break;
			}
			//�����˳�
		case EXIT_PROCESS_DEBUG_EVENT:
			{
				Deal_EPDE();
				ExitProcess(0);
				break;
			}

		default:
			{
				break;
			}
		}
		ContinueDebugEvent(
			DbgEvt.dwProcessId,
			DbgEvt.dwThreadId,
			dwRet);
		//dwRet��ֵ:
		//DBG_CONTINUE:				�쳣�Ѿ�����
		//DBG_EXCEPTION_NOT_HANDLED:�쳣��Ҫϵͳ��������(���ǵ�Ĭ��)
	}
	return false;
}





bool BreakNow()
{
	if(0==DebugBreakProcess(g_hProc))
	{
		return false;
	}
	else
	{
		return true;
	}

}



