// LPEReLocal.cpp: implementation of the LPEReLocal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LProctced.h"
#include "LPEReLocal.h"
#include "LPESection.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPEReLocal::LPEReLocal()
{

}

LPEReLocal::~LPEReLocal()
{

}

void LPEReLocal::operator =(LPEFile& lPeFile)
{
    SetInfo(lPeFile.GetInfo());
}
//得到首个重定位表
PIMAGE_BASE_RELOCATION LPEReLocal::GetFirstReLocaltion()
{
    PIMAGE_BASE_RELOCATION pRelocalDesc=NULL;
	pRelocalDesc=(PIMAGE_BASE_RELOCATION)GetDirectoryEntryToData(IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if (pRelocalDesc==NULL)
    {
		return NULL;
    }
	return pRelocalDesc;
}
//得到重定位表的个数
int LPEReLocal::GetReLocalCount()
{
   int i=0;
   DWORD dwOffset;
   PIMAGE_DATA_DIRECTORY pDataDir=GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC);
   dwOffset=pDataDir->VirtualAddress;
   PIMAGE_BASE_RELOCATION pTempReLocal=GetFirstReLocaltion();
   while(pTempReLocal->VirtualAddress)
   {
	   ++i;
	   dwOffset=dwOffset+(pTempReLocal->SizeOfBlock);
	   pTempReLocal=(PIMAGE_BASE_RELOCATION)RvaToPtr(dwOffset);
   }
   return i;
}

PIMAGE_BASE_RELOCATION LPEReLocal::GetReLocal(int nIndex, DWORD *rdwOffset)
{
     PIMAGE_BASE_RELOCATION pReLocal=NULL;
	 DWORD dwOffset=0;
     int nCount=GetReLocalCount();
	 PIMAGE_DATA_DIRECTORY pDataDir=GetDataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC);
     dwOffset=pDataDir->VirtualAddress;
     
	 PIMAGE_BASE_RELOCATION pTempReLocal=GetFirstReLocaltion();
	 int j=0;
	 if (nIndex<nCount)
	 {
        while(pTempReLocal->VirtualAddress)
		{
			if (nIndex==(j++))
			{
				pReLocal=pTempReLocal;
				*rdwOffset=dwOffset;
				break;
			}
			dwOffset=dwOffset+(pTempReLocal->SizeOfBlock);
	        pTempReLocal=(PIMAGE_BASE_RELOCATION)RvaToPtr(dwOffset);

		}
	 }
	 return pReLocal;
}

int LPEReLocal::GetTypeOffsetCount(int nIndex)
{
    int i=0;
	DWORD dwOffset;
	PIMAGE_BASE_RELOCATION pReLocal=GetReLocal(nIndex,&dwOffset);
	if (pReLocal!=NULL)
	{
		i=(pReLocal->SizeOfBlock-sizeof(IMAGE_BASE_RELOCATION))/sizeof(WORD);
	}
	return i;
}

WORD LPEReLocal::GetTypeOffset(int nRelocalIndex, int nOffsetIndex)
{
    DWORD dwRva;
	PIMAGE_BASE_RELOCATION pReLocal=GetReLocal(nRelocalIndex,&dwRva);
    LPESection lPeSecion;
	lPeSecion.SetInfo(GetInfo());
	DWORD dwFileOffset=lPeSecion.RvaToOffset(dwRva);
	WORD wTypeOffset=0;
	if (pReLocal!=NULL)
	{
       memcpy(&wTypeOffset,GetImage()+dwFileOffset+sizeof(IMAGE_BASE_RELOCATION)+\
		   sizeof(WORD)*nOffsetIndex,sizeof(WORD));
	}
	return wTypeOffset;
}

WORD LPEReLocal::GetReLocationType(int nRelocalIndex, int nTypeOffsetIndex)
{
   WORD wTypeOffset=GetTypeOffset(nRelocalIndex,nTypeOffsetIndex);
   wTypeOffset&=0xF000;
  wTypeOffset= wTypeOffset>>12;
   return wTypeOffset;
}

DWORD LPEReLocal::GetReLocaltionRVA(int nRelocalIndex, int nTypeOffsetIndex)
{
     DWORD dwOffset;
	 PIMAGE_BASE_RELOCATION pReLocal=GetReLocal(nRelocalIndex,&dwOffset);
	 WORD wTypeOffset=GetTypeOffset(nRelocalIndex,nTypeOffsetIndex);
	 wTypeOffset&=0x0FFF;
	 return pReLocal->VirtualAddress+wTypeOffset;
}


BOOL LPEReLocal::RelocaltionVA(DWORD dwNewAddress, DWORD dwNewImageBase, DWORD &dwOldVA)
{ 
	 PIMAGE_OPTIONAL_HEADER pOptHeader=GetNtOptionalHeader();
	 dwOldVA-=pOptHeader->ImageBase;
	 dwOldVA-=0x1000;
	 dwOldVA+=dwNewAddress;
	 dwOldVA+=dwNewImageBase;
	 return true;
}

BOOL LPEReLocal::RelocationMySelf(DWORD dwNewSecAddress, DWORD dwImageBase)
{
        int iRelocalCount=GetReLocalCount();
		for (int i=0;i<iRelocalCount;i++)
		{
			int iTypeOffset=GetTypeOffsetCount(i);
			for (int j=0;j<iTypeOffset;j++)
			{
				if (GetReLocationType(i,j)==3)
				{
					RelocaltionVA(dwNewSecAddress,dwImageBase,*(DWORD*)RvaToPtr(GetReLocaltionRVA(i,j)));
				}
             
			}
		}
		return TRUE;
}
