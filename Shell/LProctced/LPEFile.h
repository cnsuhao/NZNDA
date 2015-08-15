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
	HANDLE hFile;    //打开的文件句柄
	HANDLE hMapping;  //文件映射到内存的句柄
	LPVOID ImageBase;  //文件的数据
	DWORD  dwFileSize;  //文件的大小
}  MAP_FILE_STRUCT,*PMAP_FILE_STRUCT;

enum{
	    LFILE_FROM_NOMAL=0,       //文件打开时的处理方式
		LFILE_FROM_FILE,
		LFILE_FROM_MEMORY
};
//基本的PE处理 可以得到dos头 NT头 节 等信息
class LPEFile  
{
public:
	BOOL LoadPEFile(BYTE* lpBuffer,DWORD dwSize);          //载入PE文件
	LPVOID RvaToPtr(DWORD dwRVA);                          //计算偏移
	LPVOID GetDirectoryEntryToData(USHORT DirectoryEntry);   
	PIMAGE_SECTION_HEADER GetSection(int nIndex);           //得到节的信息
	BOOL IsPEFile();
	PIMAGE_SECTION_HEADER GetFirstSectionHeader();         //得到第一个节
	int GetSectionCount();                                 //得到节的个数
	PIMAGE_DATA_DIRECTORY GetDataDirectory(int nIndex);     //得到数据目录
	PIMAGE_DATA_DIRECTORY GetDataDirectory();               //得到数据目录
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
	//用于标识文件的打开状态//用于标识文件的打开状态
    BYTE bLoadStact;    
    //用于存储打开的文件句柄，文件数据，文件大小

protected:
    MAP_FILE_STRUCT *pstMapFile;
};

#endif // !defined(AFX_LPEFILE_H__869EE97E_7A5F_4BA7_8828_7FEE11C91D3C__INCLUDED_)
