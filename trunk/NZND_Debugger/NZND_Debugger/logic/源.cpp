
#include "StartDebug.h"
#include <Windows.h>
#include "DealEvent.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <vector>

using namespace std;



DWORD	dwAttachPID			=	0;

bool	InputDebuggedPath	=	false;//是否以路径传入方式启动调试程序
extern WCHAR *wcInputDebuggedPath;

extern _PROCMSG g_ProcMsg;


int main()
{
	//读配置文件并初始化信息
	if(false==ReadFile())
	{
		return false;
	}

	LPWSTR *szArgList;
	int argCount;

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (szArgList == NULL)
	{
		return 10;
	}
	else
	{
		if (NULL==szArgList[1])
		{
			LocalFree(szArgList);
			return 20;
		}

		DWORD dwPID=_wtoi(szArgList[1]);

		StartDebug(false,dwPID);
	}

	LocalFree(szArgList);
	
	return 0;
}



bool ReadFile()
{

	ifstream ifA("conf.data",ios::binary);
	if (0==ifA)
	{
		return false;
	}

	ifA.read((char*)&g_ProcMsg.dwStact,sizeof(DWORD));
	DWORD dwSizeofVector=0;
	ifA.read((char*)&dwSizeofVector,sizeof(DWORD));
	for (DWORD i=0;i<dwSizeofVector;i++)
	{
		DWORD dwTempLength=0;

		ifA.read((CHAR*)&dwTempLength,sizeof(DWORD));

		g_ProcMsg.vdLength.push_back(dwTempLength);

		int itmpA=0;
		ifA.read((CHAR*)&itmpA,sizeof(int));
		if (0==itmpA)
		{
			g_ProcMsg.vbEncryState.push_back(false);
		}
		else
		{
			g_ProcMsg.vbEncryState.push_back(true);
		}
		PVOID pvTempAddr=0;
		ifA.read((CHAR*)&pvTempAddr,sizeof(PVOID));
		g_ProcMsg.vpAddress.push_back(pvTempAddr);



	}

	ifA.close();
	return true;

}