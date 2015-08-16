
#include <Windows.h>
#include "Debug.h"
#include "DealEvent.h"
#include <TlHelp32.h>
#include <vector>
#include "StartDebug.h"

using std::vector;


_DEBUG_EVENT	DbgEvt	= {0};		
DWORD	dwRet			= DBG_EXCEPTION_NOT_HANDLED;//未处理!!!!!!!!!!!dwRet有错误使用的代码似乎
STARTUPINFO			stcStartupInfo	= {sizeof(STARTUPINFO)};
PROCESS_INFORMATION stcProcInfo		= {0};//进程信息
extern HANDLE g_hProc;//被调试者的句柄
vector<ThreadInformation> vTI;//被调试者的线程信息
const wchar_t MUTEX_NAME[] = L"NZND_MUTEX";

int StartDebug(bool bNewProcess,void *Infor);
bool GetEvent();


//////////////////////////////////////////////////////////////////////////
//bool bNewProcess	:以调试的方式启动一个进程(true),附加到一个进程(false)
//void *Infor		:指向一个wchar_t字符串或者DWORD值,这取决于bNewProcess.
int StartDebug(bool bNewProcess,DWORD dwPID)
{


	if (!bNewProcess)
	{
		if(0==DebugActiveProcess(dwPID))
		{
			return -1;
		}//传入DWORD值,而不是传入DWORD指针.
	}

	
	//HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, MUTEX_NAME); //打开互斥量
	//CloseHandle(hMutex);//关闭互斥量表示我已经准备好了.


	GetEvent();

	return 1;
}

bool GetEvent()
{
	while(1==WaitForDebugEvent(&DbgEvt,INFINITE))
	{


		switch (DbgEvt.dwDebugEventCode)
		{		
			//调试异常
		case EXCEPTION_DEBUG_EVENT:
			{
				dwRet= OnExceptionDebugEvent(&DbgEvt.u.Exception);
				break;
			}	
			//进程创建
		case CREATE_PROCESS_DEBUG_EVENT:
			{
				Deal_CPEV();
				break;
			}
			//进程退出
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
		//dwRet的值:
		//DBG_CONTINUE:				异常已经处理
		//DBG_EXCEPTION_NOT_HANDLED:异常需要系统继续处理(我们的默认)
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



