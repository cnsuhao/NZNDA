// LPEReLocal.h: interface for the LPEReLocal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LPERELOCAL_H__1A77D5F5_D285_4781_A935_6B919D705B8A__INCLUDED_)
#define AFX_LPERELOCAL_H__1A77D5F5_D285_4781_A935_6B919D705B8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LPEFile.h"
//继承自LPEFile  对处理重定位表功能进行扩展
class LPEReLocal : public LPEFile  
{
public:
	BOOL RelocaltionVA(DWORD dwNewAddress, DWORD dwNewImageBase, DWORD &dwOldVA);    
	BOOL RelocationMySelf(DWORD dwNewSecAddress,DWORD dwImageBase);
	DWORD GetReLocaltionRVA(int nRelocalIndex,int nTypeOffsetIndex);
	WORD GetReLocationType(int nRelocalIndex,int nTypeOffsetIndex);
	WORD GetTypeOffset(int nRelocalIndex,int nOffsetIndex);
	int GetTypeOffsetCount(int nIndex);
	PIMAGE_BASE_RELOCATION GetReLocal(int nCount,DWORD *rdwOffset);
	int GetReLocalCount();
	PIMAGE_BASE_RELOCATION GetFirstReLocaltion();
	LPEReLocal();
	virtual ~LPEReLocal();
	void operator=(LPEFile& lPeFile);

};

#endif // !defined(AFX_LPERELOCAL_H__1A77D5F5_D285_4781_A935_6B919D705B8A__INCLUDED_)
