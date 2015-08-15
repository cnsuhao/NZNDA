// LPEFile.cpp: implementation of the LPEFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LPEFile.h"
#include <imagehlp.h>
#pragma comment(lib,"Dbghelp.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPEFile::LPEFile()
{
    bLoadStact=LFILE_FROM_NOMAL;
	pstMapFile=NULL;
}

LPEFile::~LPEFile()
{
    if (Free())
    {
       delete pstMapFile;
	   pstMapFile=NULL;
    }
}

//得到文件打开的数据 

MAP_FILE_STRUCT* LPEFile::GetInfo()
{
    return pstMapFile;
}
//用打开的信息载入一个PE

BOOL LPEFile::SetInfo(MAP_FILE_STRUCT* pMapFileMsg)
{
	this->pstMapFile=pMapFileMsg;
   return TRUE;
}

void LPEFile::operator =(LPEFile& lPeFile)
{
    SetInfo(lPeFile.GetInfo());
}

//用于将文件载入内存

BOOL LPEFile::LoadPEFile(char *lpFilename)
{
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID ImageBase;
    pstMapFile=new MAP_FILE_STRUCT;
	bLoadStact=LFILE_FROM_FILE;
    
	hFile=CreateFile(lpFilename,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==NULL)
	{
		return FALSE;
	}
	pstMapFile->dwFileSize=GetFileSize(hFile,NULL);
	hMapping=CreateFileMapping(hFile,NULL,PAGE_READWRITE,0,pstMapFile->dwFileSize,NULL);
	if (hMapping==NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}
    
	ImageBase=MapViewOfFile(hMapping,FILE_MAP_ALL_ACCESS,0,0,pstMapFile->dwFileSize);
	if (ImageBase==NULL)
	{
		CloseHandle(hFile);
		CloseHandle(hMapping);
		return FALSE;
	}
	pstMapFile->hFile=hFile;
	pstMapFile->hMapping=hMapping;
	pstMapFile->ImageBase=ImageBase;
   return TRUE;
}



BOOL LPEFile::LoadPEFile(BYTE *lpBuffer, DWORD dwSize)
{
	pstMapFile=new MAP_FILE_STRUCT;
    bLoadStact=LFILE_FROM_MEMORY;

	pstMapFile->hFile=pstMapFile->hMapping=NULL;
	pstMapFile->ImageBase=lpBuffer;
	pstMapFile->dwFileSize=dwSize;
	return TRUE;
}

//将文件从内存中释放

bool LPEFile::Free()
{
	if (bLoadStact==LFILE_FROM_FILE)
	{
		if (pstMapFile->hMapping!=NULL)
		{
			UnmapViewOfFile(pstMapFile->hMapping);
			CloseHandle(pstMapFile->hMapping);
			pstMapFile->hMapping=NULL;
			if (pstMapFile->hFile!=NULL)
			{
				CloseHandle(pstMapFile->hFile);
				pstMapFile->hFile=NULL;
			}
			   return true;
		}
	}else if (bLoadStact==LFILE_FROM_MEMORY)
	{
		LocalFree(pstMapFile->ImageBase);
		return true;
	}
	return false;

}

BYTE* LPEFile::GetImage()
{
	if (pstMapFile!=NULL)
	{
		return (BYTE*)pstMapFile->ImageBase;
	}
    return NULL;
}

DWORD LPEFile::GetImageSize()
{
	if (pstMapFile!=NULL)
	{
		return pstMapFile->dwFileSize;
	}
	return 0;
}
//得到文件的dos头
PIMAGE_DOS_HEADER LPEFile::GetDosHeader()
{
   return (PIMAGE_DOS_HEADER)GetImage();
}
//得到文件的NT头
PIMAGE_NT_HEADERS LPEFile::GetNtHeader()
{
    PIMAGE_DOS_HEADER pDH=GetDosHeader();
	if (pDH!=NULL)
	{
		return (PIMAGE_NT_HEADERS32)((DWORD)GetImage()+pDH->e_lfanew);
	}
	return NULL;
}
//得到文件头
PIMAGE_FILE_HEADER LPEFile::GetNtFileHeader()
{
   PIMAGE_NT_HEADERS pNth=GetNtHeader();
   if (pNth!=NULL)
   {
	   return (PIMAGE_FILE_HEADER)&pNth->FileHeader;
   }
   return NULL;
}
//得到可选头
PIMAGE_OPTIONAL_HEADER LPEFile::GetNtOptionalHeader()
{
	PIMAGE_NT_HEADERS pNth=GetNtHeader();
	if (pNth!=NULL)
	{
		return (PIMAGE_OPTIONAL_HEADER32)&pNth->OptionalHeader;
	}
   return NULL;
}
//得到数据目录
PIMAGE_DATA_DIRECTORY LPEFile::GetDataDirectory()
{
   PIMAGE_OPTIONAL_HEADER pOptionalHeader=GetNtOptionalHeader();
   if (pOptionalHeader!=NULL)
   {
	   return (PIMAGE_DATA_DIRECTORY) pOptionalHeader->DataDirectory;
   }
   return NULL;
}

PIMAGE_DATA_DIRECTORY LPEFile::GetDataDirectory(int nIndex)
{
    if (nIndex>=0&&nIndex<16)
    {
        PIMAGE_DATA_DIRECTORY pDataDir=GetDataDirectory();
		if (pDataDir!=NULL)
		{
				return &pDataDir[nIndex];
		}
    }
	return NULL;
}

int LPEFile::GetSectionCount()
{
     PIMAGE_FILE_HEADER pNtFileHeader=GetNtFileHeader();
	 if (pNtFileHeader!=NULL)
	 {
		 return pNtFileHeader->NumberOfSections;
	 }
	 return 0;
}

PIMAGE_SECTION_HEADER LPEFile::GetFirstSectionHeader()
{
     PIMAGE_NT_HEADERS pNtH=GetNtHeader();
	 if (pNtH!=NULL)
	 {
		 return IMAGE_FIRST_SECTION(pNtH);
	 }
	 return NULL;
}

PIMAGE_SECTION_HEADER LPEFile::GetSection(int nIndex)
{
	int nCount=GetSectionCount();
	if (nIndex>=nCount||nIndex<0)
	{
		return NULL;
	}
	PIMAGE_SECTION_HEADER pFirstSectionHeader=GetFirstSectionHeader();
    for (int i=0;i<nCount;i++,pFirstSectionHeader++)
    {
      if (i==nIndex)
      {
		  return pFirstSectionHeader;
      }
    }

	return NULL;
}
BOOL LPEFile::IsPEFile()
{
    PIMAGE_DOS_HEADER pDosHeader=GetDosHeader();
	if (pDosHeader->e_magic==IMAGE_DOS_SIGNATURE)
	{
        PIMAGE_NT_HEADERS pNtHeader=GetNtHeader();
		if (pNtHeader->Signature==IMAGE_NT_SIGNATURE)
		{
			return TRUE;
		}
	}
	return FALSE;
}


//------------------第16讲------------------------
LPVOID LPEFile::GetDirectoryEntryToData(USHORT DirectoryEntry)
{
    DWORD dwDataStartRVA;
	LPVOID pDirData=NULL;
	PIMAGE_NT_HEADERS     pNtH=NULL;
	PIMAGE_OPTIONAL_HEADER pOH=NULL;
	
	pNtH=GetNtHeader();
	if(!pNtH)
		return NULL;

	dwDataStartRVA=GetDataDirectory(DirectoryEntry)->VirtualAddress;
	if(!dwDataStartRVA)
		return NULL;
	
	pDirData=RvaToPtr(dwDataStartRVA);
	if(!pDirData)
		return NULL;	 
	return  pDirData;
}

LPVOID LPEFile::RvaToPtr(DWORD dwRVA)
{
	PIMAGE_NT_HEADERS pNtH=GetNtHeader();
	return ImageRvaToVa(pNtH,pstMapFile->ImageBase,dwRVA,NULL);
}
