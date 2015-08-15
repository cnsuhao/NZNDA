// LPEExport.cpp: implementation of the LPEExport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LPEExport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPEExport::LPEExport()
{

}

LPEExport::~LPEExport()
{

}

void LPEExport::operator =(LPEFile& lPeFile)
{
    SetInfo(lPeFile.GetInfo());
}

PIMAGE_EXPORT_DIRECTORY LPEExport::GetExportTab()
{
     PIMAGE_EXPORT_DIRECTORY pExportDesc;
	 pExportDesc=(PIMAGE_EXPORT_DIRECTORY)GetDirectoryEntryToData(IMAGE_DIRECTORY_ENTRY_EXPORT);
     if (pExportDesc==NULL)
     {
         return NULL;
     }
	 return pExportDesc;
}

int LPEExport::GetFuncCount()
{
   PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
   if (pExportDesc)
   {
        return pExportDesc->NumberOfFunctions;
   }
   return -1;
}

int LPEExport::GetNameCount()
{
    PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
        return pExportDesc->NumberOfNames;
	}
   return -1;
}

DWORD LPEExport::GetFirstFunc()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
       return pExportDesc->AddressOfFunctions;
	}
    return 0;
}

DWORD LPEExport::GetFunc(int nIndex)
{
   if (nIndex<GetFuncCount())
   {
       DWORD dwFirstFunc=GetFirstFunc();
	   return dwFirstFunc+nIndex*4;
   }
   return 0;
}

DWORD LPEExport::GetFuncValue(int nIndex)
{
    DWORD dwFuncRva=GetFunc(nIndex);
	if (dwFuncRva)
	{
		return *(DWORD*)RvaToPtr(dwFuncRva);
	}
}

DWORD LPEExport::GetFirstName()
{
	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
		return pExportDesc->AddressOfNames;
	}
    return 0;
}

DWORD LPEExport::GetName(int nIndex)
{
    if (nIndex<GetNameCount())
	{
		DWORD dwFirstName=GetFirstName();
		return dwFirstName+nIndex*4;
	}
   return 0;
} 

char* LPEExport::GetNameValue(int nIndex)
{
    DWORD dwNameRva=GetName(nIndex);
	if (dwNameRva)
	{
		DWORD dwNameValueRva=*(DWORD*)RvaToPtr(dwNameRva);
        return (char*)RvaToPtr(dwNameValueRva);
	}
	return NULL;
}

DWORD LPEExport::GetFirstNameOrd()
{
   	PIMAGE_EXPORT_DIRECTORY pExportDesc=GetExportTab();
	if (pExportDesc)
	{
		return pExportDesc->AddressOfNameOrdinals;
	}
    return 0;
}

DWORD LPEExport::GetNameOrd(int nIndex)
{
	if (nIndex<GetNameCount())
	{
		DWORD dwFirstNameOrd=GetFirstNameOrd();
		return dwFirstNameOrd+nIndex*2;
	}
   return 0;
}

WORD LPEExport::GetNameOrdValue(int nIndex)
{
      DWORD dwNameOrdRva=GetNameOrd(nIndex);
	  if (dwNameOrdRva)
	  {
		  return *(WORD*)RvaToPtr(dwNameOrdRva);
	  }
}
