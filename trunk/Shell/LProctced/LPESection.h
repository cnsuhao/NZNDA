// LPESection.h: interface for the LPESection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LPESECTION_H__099364B6_558D_4C22_B6CB_A9320BDA61CF__INCLUDED_)
#define AFX_LPESECTION_H__099364B6_558D_4C22_B6CB_A9320BDA61CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LPEFile.h"
//继承LPEFile里面基础处理功能并对  对节处理进行扩展
class LPESection : public LPEFile  
{
public:
	BOOL WriteSectionData(int nIndex,DWORD dwOffset,BYTE* lpBuffer,DWORD *dwSize);   //写入节数据
	BYTE* GetSectionData(int nIndex,DWORD* rdwSize);                                  //得到节数据
	BOOL AddSection(char* pSectionName,DWORD &dwSectionSize,DWORD dwSectionStact);     //添加一个节
	DWORD OffsetToRva(DWORD dwOffset);                                                 //计算偏移
	DWORD RvaToOffset(DWORD dwRva);                                                    //计算偏移
	DWORD GetVRk(int nSeciotnIndex);
	int OffsetToSectionIndex(DWORD dwOffset);
	int RvaToSectionIndex(DWORD dwRva);
	LPESection();
	virtual ~LPESection();
	void operator =(LPEFile& lPeFile);

};

#endif // !defined(AFX_LPESECTION_H__099364B6_558D_4C22_B6CB_A9320BDA61CF__INCLUDED_)
