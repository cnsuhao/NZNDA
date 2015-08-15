// LPESection.h: interface for the LPESection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LPESECTION_H__099364B6_558D_4C22_B6CB_A9320BDA61CF__INCLUDED_)
#define AFX_LPESECTION_H__099364B6_558D_4C22_B6CB_A9320BDA61CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LPEFile.h"
//�̳�LPEFile������������ܲ���  �Խڴ��������չ
class LPESection : public LPEFile  
{
public:
	BOOL WriteSectionData(int nIndex,DWORD dwOffset,BYTE* lpBuffer,DWORD *dwSize);   //д�������
	BYTE* GetSectionData(int nIndex,DWORD* rdwSize);                                  //�õ�������
	BOOL AddSection(char* pSectionName,DWORD &dwSectionSize,DWORD dwSectionStact);     //���һ����
	DWORD OffsetToRva(DWORD dwOffset);                                                 //����ƫ��
	DWORD RvaToOffset(DWORD dwRva);                                                    //����ƫ��
	DWORD GetVRk(int nSeciotnIndex);
	int OffsetToSectionIndex(DWORD dwOffset);
	int RvaToSectionIndex(DWORD dwRva);
	LPESection();
	virtual ~LPESection();
	void operator =(LPEFile& lPeFile);

};

#endif // !defined(AFX_LPESECTION_H__099364B6_558D_4C22_B6CB_A9320BDA61CF__INCLUDED_)
