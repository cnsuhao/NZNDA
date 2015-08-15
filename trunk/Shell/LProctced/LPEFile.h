// LPEFile.h: interface for the LPEFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LPEFILE_H__869EE97E_7A5F_4BA7_8828_7FEE11C91D3C__INCLUDED_)
#define AFX_LPEFILE_H__869EE97E_7A5F_4BA7_8828_7FEE11C91D3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _MAP_FILE_STRUCT
{
	HANDLE hFile;    //�򿪵��ļ����
	HANDLE hMapping;  //�ļ�ӳ�䵽�ڴ�ľ��
	LPVOID ImageBase;  //�ļ�������
	DWORD  dwFileSize;  //�ļ��Ĵ�С
}  MAP_FILE_STRUCT,*PMAP_FILE_STRUCT;

enum{
	    LFILE_FROM_NOMAL=0,       //�ļ���ʱ�Ĵ���ʽ
		LFILE_FROM_FILE,
		LFILE_FROM_MEMORY
};
//������PE���� ���Եõ�dosͷ NTͷ �� ����Ϣ
class LPEFile  
{
public:
	BOOL LoadPEFile(BYTE* lpBuffer,DWORD dwSize);          //����PE�ļ�
	LPVOID RvaToPtr(DWORD dwRVA);                          //����ƫ��
	LPVOID GetDirectoryEntryToData(USHORT DirectoryEntry);   
	PIMAGE_SECTION_HEADER GetSection(int nIndex);           //�õ��ڵ���Ϣ
	BOOL IsPEFile();
	PIMAGE_SECTION_HEADER GetFirstSectionHeader();         //�õ���һ����
	int GetSectionCount();                                 //�õ��ڵĸ���
	PIMAGE_DATA_DIRECTORY GetDataDirectory(int nIndex);     //�õ�����Ŀ¼
	PIMAGE_DATA_DIRECTORY GetDataDirectory();               //�õ�����Ŀ¼
	PIMAGE_OPTIONAL_HEADER GetNtOptionalHeader();           
	PIMAGE_FILE_HEADER GetNtFileHeader();
	PIMAGE_NT_HEADERS GetNtHeader();          
	PIMAGE_DOS_HEADER GetDosHeader();
	DWORD GetImageSize();
	BYTE* GetImage();
	bool Free();
	BOOL LoadPEFile(char* lpFilename);
	BOOL SetInfo(MAP_FILE_STRUCT *pMapFileMsg);
	MAP_FILE_STRUCT* GetInfo();
	void operator=(LPEFile& lPeFile);
	LPEFile();
	virtual ~LPEFile();

private:
	//���ڱ�ʶ�ļ��Ĵ�״̬//���ڱ�ʶ�ļ��Ĵ�״̬
    BYTE bLoadStact;    
    //���ڴ洢�򿪵��ļ�������ļ����ݣ��ļ���С

protected:
    MAP_FILE_STRUCT *pstMapFile;
};

#endif // !defined(AFX_LPEFILE_H__869EE97E_7A5F_4BA7_8828_7FEE11C91D3C__INCLUDED_)
