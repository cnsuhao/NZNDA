// LProctcedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LProctced.h"
#include "LProctcedDlg.h"
#include "LPEFile.h"
#include "LPESection.h"
#include "LPEReLocal.h"
#include "LPEImport.h"

#include <iostream>
#include <fstream>
#include <istream>
#include <vector>

using namespace std;




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About


struct _PROCMSG
{
	DWORD dwStact;            //一个标志
	DWORD dwImageBase;   //源程序的基址
	DWORD dwImportRVA;    //源程序导入表的位置
	DWORD dwEntryRVA;      //源程序入口点

};





_PROCMSG g_ProcMsg={0x12345678};

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLProctcedDlg dialog

CLProctcedDlg::CLProctcedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLProctcedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLProctcedDlg)
	m_strKey = _T("");
	m_strFileName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLProctcedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLProctcedDlg)
	DDX_Text(pDX, IDC_EDIT_KEY, m_strKey);
	DDV_MaxChars(pDX, m_strKey, 1);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strFileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLProctcedDlg, CDialog)
	//{{AFX_MSG_MAP(CLProctcedDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLProctcedDlg message handlers

BOOL CLProctcedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLProctcedDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLProctcedDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLProctcedDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLProctcedDlg::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE,"exe","",OFN_OVERWRITEPROMPT,"文件(*.exe)|*.exe||");
    if (dlg.DoModal()!=IDOK)
    {
		return;
    }
	m_strFileName=dlg.GetPathName();
	UpdateData(FALSE);
}

void XORData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)
{
   for (DWORD d=0;d<dwSize;d++)
   {
	   lpBuffer[d]^=bKey;
   }
}
void CLProctcedDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	UpdateData(TRUE);
	LPEFile lPeFile,lPeStup;
	LPESection lPeSection,lPeStupSection;
	LPEReLocal lPeReloc,lStupReloc;
	LPEImport   lPeStupImport;
	DWORD dwSectionSize=1024;
    
	g_ProcMsg.dwStact=m_strKey.GetAt(0);  //加密的密码
    if (!LoadPE(&lPeFile,&lPeStup))
    {
		MessageBox("载入文件出错！");
    }
	//保存信息
	g_ProcMsg.dwEntryRVA=lPeFile.GetNtOptionalHeader()->AddressOfEntryPoint;  //**入口点
	g_ProcMsg.dwImageBase=lPeFile.GetNtOptionalHeader()->ImageBase;            //**基地址


	lPeStupSection=lPeStup;
	LPBYTE lpStupBuffer=lPeStupSection.GetSectionData(0,&dwSectionSize);

	//----------------加入引导段并重定位引导段-------------------------
	//添加新的引导节
	lPeSection=lPeFile;
	lPeSection.AddSection(".lang",dwSectionSize,IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_EXECUTE);
    PIMAGE_SECTION_HEADER pStupSection=lPeFile.GetSection(lPeFile.GetSectionCount()-1);
	//重定位自身
    lStupReloc=lPeStup;
    lStupReloc.RelocationMySelf(pStupSection->VirtualAddress,lPeFile.GetNtOptionalHeader()->ImageBase);
	lPeSection.WriteSectionData(lPeSection.GetSectionCount()-1,0,lpStupBuffer,&dwSectionSize);
	
	//改变源程序入口点
    lPeFile.GetNtOptionalHeader()->AddressOfEntryPoint=pStupSection->VirtualAddress+lPeStup.GetNtOptionalHeader()->AddressOfEntryPoint-0x1000;
	//----------------加入导入表并修复导入表-------------------------
    lPeSection.AddSection(".rlang",dwSectionSize,IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|IMAGE_SCN_CNT_INITIALIZED_DATA);
	PIMAGE_SECTION_HEADER  pImportSection=lPeFile.GetSection(lPeFile.GetSectionCount()-1);
	
	lPeStupImport=lPeStup;
    lPeStupImport.ReLocalImport(pImportSection->VirtualAddress);
	lpStupBuffer=lPeStupSection.GetSectionData(1,&dwSectionSize);
	lPeSection.WriteSectionData(lPeSection.GetSectionCount()-1,0,lpStupBuffer,&dwSectionSize);
	
	//改变导入表的位置
	PIMAGE_DATA_DIRECTORY pImageDataDir=(PIMAGE_DATA_DIRECTORY)lPeStup.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
	DWORD dwStupImportRVA=pImageDataDir->VirtualAddress;
	dwStupImportRVA-=0x2000;
	dwStupImportRVA+=pImportSection->VirtualAddress;

     pImageDataDir=(PIMAGE_DATA_DIRECTORY)lPeFile.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
	 g_ProcMsg.dwImportRVA=pImageDataDir->VirtualAddress;    //**保存原程序的导入表位置
	 pImageDataDir->VirtualAddress=dwStupImportRVA;
	 
	 
	 
	 //加密数据
	 //lpStupBuffer=(BYTE*)lPeSection.RvaToPtr(g_ProcMsg.dwEntryRVA);

	  //获取OEP所在区段信息
	 DWORD dwLengthOfOEPSection;
	 BYTE* StartOEPSection = lPeSection.GetSectionData(lPeSection.RvaToSectionIndex(g_ProcMsg.dwEntryRVA),&dwLengthOfOEPSection);

     //判断该区段长度是否达到4*1024BYTE.不足的,按照一个4*1024处理.
	 DWORD EnCryNumber=dwLengthOfOEPSection/(4*1024);
	 DWORD RemainderTemp=dwLengthOfOEPSection%(4*1024);

	 lpStupBuffer=(BYTE*)lPeSection.RvaToPtr((DWORD)StartOEPSection);
	 

	 if (0!=RemainderTemp)
	 {
		 ++EnCryNumber;
	 }
	 
	 for (DWORD i=0;i<EnCryNumber;++i)
	 {
		 vpAddress.push_back(lpStupBuffer+i*4*1024);
		 vbEncryState.push_back(false);//初始状态未加密
		 vdLength.push_back(1024*4);


	 }


	 //写配置文件
	 ofstream ofA("conf.data",ios::binary);
	 if (0==ofA)
	 {
		 return;
	 }
	 ofA.write((CHAR*)&g_ProcMsg.dwStact,sizeof(DWORD));
	 DWORD dwTempSize=vdLength.size();

	 ofA.write((CHAR*)&dwTempSize,sizeof(DWORD));
	 for (DWORD i=0;i<vpAddress.size();i++)
	 {
		 ofA.write((CHAR*)&vdLength[i],sizeof(DWORD));
		 int itmpA=vbEncryState[i];
		 ofA.write((CHAR*)&itmpA,sizeof(int));
		 ofA.write((CHAR*)&vpAddress[i],sizeof(PVOID));
	 }

	 ofA.close();


	 //写入引导的相关信息 如入口点 基址 导入表位置
	 lpStupBuffer=lPeSection.GetSectionData(lPeFile.GetSectionCount()-2,&dwSectionSize);
	 memcpy(lpStupBuffer,&g_ProcMsg,sizeof(g_ProcMsg));

	 Encry(g_ProcMsg.dwStact);//按照指定信息进行加密
	 //属性保护
	for (DWORD i=0;i<vpAddress.size();++i)
	{
		RepairProtData(i);
	}
	 //XORData(lpStupBuffer,0x100,g_ProcMsg.dwStact);
	 MessageBox("ok");
	
}

//取掉属性
bool CLProctcedDlg::RepairProtData(DWORD dwIndex)
{
	DWORD dwRet=0;

	DWORD OldProtect	= 0;//重要的值,用来存放保护属性
	PDWORD lpOldProtect =&OldProtect;

	DWORD OldProtectB	= 0;//一个不会使用的值,用于存放保护属性
	PDWORD  lpOldProtectB=&OldProtectB;

	if(0!=VirtualProtect(vpAddress[dwIndex],vdLength[dwIndex],PAGE_NOACCESS,lpOldProtect))
	{
		return true;
	}
	return false;
}




//加密指定块
bool	CLProctcedDlg::EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock)
{
	//如果本段代码已经加密了
	if (true==vbEncryState[dwIndexBlock])
	{
		return true;
	}

	if (0==dwIndexBlock)
	{

		for (DWORD dSize=0;dSize<vdLength[dwIndexBlock];++dSize)
		{
			((BYTE*)(vpAddress[dwIndexBlock]))[dSize]^=bFirstKey;

		}
	}
	else
	{
		if(true==vbEncryState[dwIndexBlock-1])
		{
			//变形上一个加密段,获取密钥
			BYTE NowKey=XOREncryData((BYTE*)vpAddress[dwIndexBlock-1],vdLength[dwIndexBlock-1],0);	
			//用密钥加密自身
			for (DWORD dSize=0;dSize<vdLength[dwIndexBlock];++dSize)
			{
				((BYTE*)(vpAddress[dwIndexBlock]))[dSize]^=NowKey;
			}
			//恢复上一个被变形的加密段
			XORDecryData((BYTE*)vpAddress[dwIndexBlock-1],vdLength[dwIndexBlock-1],NowKey);
		}
		else
		{
			EncryOfBlock(bFirstKey,dwIndexBlock-1);//调用自己.
		}
	}
	vbEncryState[dwIndexBlock]=true;
	return true;//已经加密
}


bool	CLProctcedDlg::Encry(BYTE bFirstKey)
{
	for (DWORD dwIndexBlock=0;dwIndexBlock<vdLength.size();++dwIndexBlock)
	{
		EncryOfBlock(bFirstKey,dwIndexBlock);

	}

	return  true;
}
BYTE CLProctcedDlg::XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//加密得到密钥
{
	for (DWORD d=0;d<dwSize;++d)
	{
		lpBuffer[d]=lpBuffer[d]+bKey;
		bKey=lpBuffer[d]^bKey;
	}
	return bKey;
}


void CLProctcedDlg::XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//解密还原
{
	for (signed int d=(dwSize-1);d>=0;--d)
	{
		bKey=lpBuffer[d]^bKey;
		lpBuffer[d]=lpBuffer[d]-bKey;
	}
}





BOOL CLProctcedDlg::LoadPE(LPEFile *lPeFile, LPEFile *lPEStup)
{
    HGLOBAL hRes;
	HRSRC   hResInfo;
	DWORD   dwFileSize;
	BYTE*   lpDataBuffer;
	
	hResInfo=FindResource(NULL,MAKEINTRESOURCE(IDR_STUP),"STUP");
	if (hResInfo==NULL)
	{
		return false;
	}

	hRes=LoadResource(NULL,hResInfo);
	if (hRes==NULL)
	{
		return FALSE;
	}

	dwFileSize=SizeofResource(NULL,hResInfo);
	lpDataBuffer=(BYTE*)LocalAlloc(LPTR,dwFileSize);
	ZeroMemory(lpDataBuffer,dwFileSize);
	memcpy(lpDataBuffer,hRes,dwFileSize);
	FreeResource(hRes);

	lPEStup->LoadPEFile(lpDataBuffer,dwFileSize);

	lPeFile->LoadPEFile(m_strFileName.GetBuffer(0));
	
	return TRUE;

}
