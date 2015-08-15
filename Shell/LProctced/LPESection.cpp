// LPESection.cpp: implementation of the LPESection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LPESection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define  ZALIGN(x,a)(((x-1)/a+1)*a)
LPESection::LPESection()
{

}

LPESection::~LPESection()
{

}

void LPESection::operator=(LPEFile& lPeFile)
{
	SetInfo(lPeFile.GetInfo());
}

int LPESection::RvaToSectionIndex(DWORD dwRva)
{
   int iSectionIndex=-1;
   int iSectionCount=GetSectionCount();
   for (int i=0;i<iSectionCount;i++)
   {
      PIMAGE_SECTION_HEADER pTempSectionHeader=GetSection(i);
	  if (pTempSectionHeader->VirtualAddress<=dwRva)
	  {
		  if (dwRva<=pTempSectionHeader->Misc.VirtualSize+pTempSectionHeader->VirtualAddress)
		  {
             iSectionIndex=i;
             break;
		  }
	  }
   }
   return iSectionIndex;
}

int LPESection::OffsetToSectionIndex(DWORD dwOffset)
{
   int iSectionIndex;
   int iSectionCount=GetSectionCount();
   for (int i=0;i<=iSectionCount;i++)
   {
	   PIMAGE_SECTION_HEADER pTempSectionHeader=GetSection(i);
	   if (pTempSectionHeader->PointerToRawData<dwOffset)
	   {
		   if (dwOffset<=pTempSectionHeader->PointerToRawData+pTempSectionHeader->SizeOfRawData)
		   {
               iSectionIndex=i;
			   break;
		   }
	   }
   }
   return iSectionIndex;
}

DWORD LPESection::GetVRk(int nSeciotnIndex)
{
    PIMAGE_SECTION_HEADER pTempSectionHeader=GetSection(nSeciotnIndex);
	DWORD dwVRk=pTempSectionHeader->VirtualAddress-pTempSectionHeader->PointerToRawData;
	return dwVRk;
}

DWORD LPESection::RvaToOffset(DWORD dwRva)
{
    int nSectionIdex=RvaToSectionIndex(dwRva);
	DWORD dwVRk=GetVRk(nSectionIdex);
	return dwRva-dwVRk;
}

DWORD LPESection::OffsetToRva(DWORD dwOffset)
{
   int nSectionIndex=OffsetToSectionIndex(dwOffset);
   DWORD dwVRk=GetVRk(nSectionIndex);
   return dwVRk+dwOffset;
}

BOOL LPESection::AddSection(char *pSectionName, DWORD &dwSectionSize,DWORD dwSectionStact)
{
    LPVOID lPointer=NULL;
	PBYTE  pData=NULL;
    DWORD  newSectionAddr=0;
	
	if (!IsPEFile())
	{
		return FALSE;
	}

	PIMAGE_NT_HEADERS pNTHdr=GetNtHeader();
    //是否有足够空间容纳节头
	if ((pNTHdr->FileHeader.NumberOfSections+1)*sizeof(IMAGE_SECTION_HEADER)>pNTHdr->OptionalHeader.SizeOfHeaders)
    {
		return FALSE;
    }
    //对齐数据
	DWORD uCodeDelta=ZALIGN(dwSectionSize,pNTHdr->OptionalHeader.SectionAlignment);
	DWORD uFileDelta=ZALIGN(dwSectionSize,pNTHdr->OptionalHeader.FileAlignment);

	PIMAGE_SECTION_HEADER pNewSec=(PIMAGE_SECTION_HEADER)(pNTHdr+1)+pNTHdr->FileHeader.NumberOfSections;
	PIMAGE_SECTION_HEADER pLaseSec=pNewSec-1;

	//----------为将要创建的节赋值----------
	strcpy((char*)pNewSec->Name,pSectionName);
	pNewSec->VirtualAddress=pLaseSec->VirtualAddress+ZALIGN(pLaseSec->Misc.VirtualSize,pNTHdr->OptionalHeader.SectionAlignment);
	newSectionAddr=pNewSec->PointerToRawData=pLaseSec->PointerToRawData+pLaseSec->SizeOfRawData;
	pNewSec->Misc.VirtualSize=dwSectionSize;
	pNewSec->SizeOfRawData=uFileDelta;
	pNewSec->Characteristics=dwSectionStact;

	//-------PE头的一些设置-----------------
	pNTHdr->FileHeader.NumberOfSections++;
	pNTHdr->OptionalHeader.SizeOfCode+=uFileDelta;
	pNTHdr->OptionalHeader.SizeOfImage+=uCodeDelta;

	pNTHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size=0;
    pNTHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress=0;

	dwSectionSize=uFileDelta;

	//改变自身数据
	DWORD dwOldSize=pstMapFile->dwFileSize;
	pstMapFile->dwFileSize+=dwSectionSize;

	UnmapViewOfFile(pstMapFile->hMapping);
	CloseHandle(pstMapFile->hMapping);
	pstMapFile->hMapping=CreateFileMapping(pstMapFile->hFile,NULL,PAGE_READWRITE,0,pstMapFile->dwFileSize,NULL);
	pstMapFile->ImageBase=MapViewOfFile(pstMapFile->hMapping,FILE_MAP_ALL_ACCESS,0,0,pstMapFile->dwFileSize);
	return TRUE;

}

BYTE* LPESection::GetSectionData(int nIndex, DWORD *rdwSize)
{
      BYTE* lpBuffer=NULL;
      PIMAGE_SECTION_HEADER pSection=GetSection(nIndex);
	  if (pSection->SizeOfRawData>0)
	  {
		  DWORD dwFileOffset=pSection->PointerToRawData;
		  *rdwSize=pSection->SizeOfRawData;
		  lpBuffer=GetImage()+dwFileOffset;
	  }
	  return lpBuffer;
}

BOOL LPESection::WriteSectionData(int nIndex, DWORD dwOffset, BYTE *lpBuffer, DWORD *dwSize)
{
   BOOL bIsOK=false;
   DWORD dwSectionSize;
   BYTE* lpMyBuffer=NULL;
   lpMyBuffer=GetSectionData(nIndex,&dwSectionSize);
   if (lpMyBuffer!=NULL&&dwSectionSize>=(*dwSize+dwOffset))
   {
	   lpMyBuffer=lpMyBuffer+dwOffset;
	   memcpy(lpMyBuffer,lpBuffer,*dwSize);
	   FlushViewOfFile(pstMapFile->hMapping,pstMapFile->dwFileSize);
	   bIsOK=TRUE;
   }
   return bIsOK;
}
