#include <windows.h>
#include <stdio.h>


int main()
{
	

	STARTUPINFO si = { sizeof(si) };   
	PROCESS_INFORMATION pi;   

	si.dwFlags = STARTF_USESHOWWINDOW;   
	si.wShowWindow = TRUE;

	wchar_t cmdline[65] =L" "; //注意前面有空格。
	
	//输入一个PID吧
	DWORD dwA=0;
	scanf_s("%d",&dwA);
	printf("%d",dwA);



	wchar_t wcPID[64]={0};

	_itow_s(dwA,wcPID,10);

	wcscat_s(cmdline,wcPID);


	BOOL bRet = CreateProcess (   
		TEXT("..\\Debug\\NZND_Debugger.exe"),
		cmdline,    
		NULL,   
		NULL,   
		FALSE,   
		CREATE_NEW_CONSOLE,   
		NULL,   
		NULL,   
		&si,   
		&pi); 

	int a=GetLastError();


	return 0;
}