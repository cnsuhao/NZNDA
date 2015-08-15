// LPEExport.h: interface for the LPEExport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LPEEXPORT_H__A7CA1B00_E4AB_43C2_B89B_B8A006CD88C1__INCLUDED_)
#define AFX_LPEEXPORT_H__A7CA1B00_E4AB_43C2_B89B_B8A006CD88C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LPEFile.h"

//继承自LPEFile对处理导出表功能进行扩展
class LPEExport : public LPEFile  
{
public:
	WORD GetNameOrdValue(int nIndex);
	DWORD GetNameOrd(int nIndex);
	DWORD GetFirstNameOrd();
	char* GetNameValue(int nIndex);
	DWORD GetName(int nIndex);
	DWORD GetFirstName();
	DWORD GetFuncValue(int nIndex);
	DWORD GetFunc(int nIndex);
	DWORD GetFirstFunc();
	int GetNameCount();
	int GetFuncCount();
	PIMAGE_EXPORT_DIRECTORY GetExportTab();
	LPEExport();
	virtual ~LPEExport();
	void operator=(LPEFile& lPeFile);

};

#endif // !defined(AFX_LPEEXPORT_H__A7CA1B00_E4AB_43C2_B89B_B8A006CD88C1__INCLUDED_)
