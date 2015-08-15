// LPEImport.h: interface for the LPEImport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LPEIMPORT_H__AB5B1FBB_2EB2_4B80_A9D5_49E5E6B0EACF__INCLUDED_)
#define AFX_LPEIMPORT_H__AB5B1FBB_2EB2_4B80_A9D5_49E5E6B0EACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LPEFile.h"

//继承自LPEFile 对处理导入表功能进行扩展
class LPEImport : public LPEFile  
{
public:
	BOOL ReLocalImport(DWORD dwRVA);
	DWORD GetImportThunk(PIMAGE_IMPORT_DESCRIPTOR pImportDesc);
	void operator=(LPEFile& lPeFile);
	PIMAGE_IMPORT_BY_NAME GetImportFucByName(DWORD RavThunk);
	PIMAGE_IMPORT_DESCRIPTOR GetImportDesc(int index);
	int GetImportDesCount();
	PIMAGE_IMPORT_DESCRIPTOR GetFirstImportDesc();
	LPEImport();
	virtual ~LPEImport();

};

#endif // !defined(AFX_LPEIMPORT_H__AB5B1FBB_2EB2_4B80_A9D5_49E5E6B0EACF__INCLUDED_)
