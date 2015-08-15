// LPEImport.cpp: implementation of the LPEImport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LPEImport.h"
#include "LPESection.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPEImport::LPEImport()
{

}

LPEImport::~LPEImport()
{

}

void LPEImport::operator =(LPEFile& lPeFile)
{
    SetInfo(lPeFile.GetInfo());
}
//得到整个首个导入表
PIMAGE_IMPORT_DESCRIPTOR LPEImport::GetFirstImportDesc()
{
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	pImportDesc=(PIMAGE_IMPORT_DESCRIPTOR)GetDirectoryEntryToData(IMAGE_DIRECTORY_ENTRY_IMPORT);
	if(!pImportDesc)
		return NULL;	 
	return  pImportDesc;
}
//得到共有多少个dll
int LPEImport::GetImportDesCount()
{
	int i=0;
	PIMAGE_IMPORT_DESCRIPTOR pTempImport=GetFirstImportDesc();
	while (pTempImport->FirstThunk)
	{
		++i;
		++pTempImport;
	}
	return i;
}

// 得到某个dll导入表
PIMAGE_IMPORT_DESCRIPTOR LPEImport::GetImportDesc(int index)
{
    int i=GetImportDesCount();
	if (index>=i)
	{
		return NULL;
	}
	PIMAGE_IMPORT_DESCRIPTOR pTempImport=GetFirstImportDesc();
	return &pTempImport[index];
}

//得到某个API 函数的名字
PIMAGE_IMPORT_BY_NAME LPEImport::GetImportFucByName(DWORD RavThunk)
{
	PIMAGE_NT_HEADERS     pNtH=NULL;
	PIMAGE_IMPORT_BY_NAME pApiName=NULL;
	
	pApiName=(PIMAGE_IMPORT_BY_NAME)RvaToPtr(RavThunk);

	return pApiName;
}

DWORD LPEImport::GetImportThunk(PIMAGE_IMPORT_DESCRIPTOR pImportDesc)
{
	return pImportDesc->OriginalFirstThunk?pImportDesc->OriginalFirstThunk:pImportDesc->FirstThunk;
}

BOOL LPEImport::ReLocalImport(DWORD dwRVA)
{
    int iCount=GetImportDesCount();
	LPESection  lPeSection;
	lPeSection.SetInfo(GetInfo());

	for (int i=0;i<iCount;i++)
	{
		PIMAGE_IMPORT_DESCRIPTOR  pImport=GetImportDesc(i);
		DWORD dwOrThunk=pImport->OriginalFirstThunk;
		DWORD dwIatThunk=pImport->FirstThunk;

		pImport->OriginalFirstThunk+=(dwRVA-0x2000);
		pImport->Name+=(dwRVA-0x2000);
		pImport->FirstThunk+=(dwRVA-0x2000);

		while(*(DWORD*)RvaToPtr(dwOrThunk)!=0)
		{
           *(DWORD*)RvaToPtr(dwOrThunk)+=(dwRVA-0x2000);
		   *(DWORD*)RvaToPtr(dwIatThunk)+=(dwRVA-0x2000);
		   dwOrThunk+=sizeof(DWORD);
		   dwIatThunk+=sizeof(DWORD);
		}
	}
	return TRUE;
}
