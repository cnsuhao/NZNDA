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
	DWORD dwStact;            //һ����־
	DWORD dwImageBase;   //Դ����Ļ�ַ
	DWORD dwImportRVA;    //Դ��������λ��
	DWORD dwEntryRVA;      //Դ������ڵ�

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
	CFileDialog dlg(TRUE,"exe","",OFN_OVERWRITEPROMPT,"�ļ�(*.exe)|*.exe||");
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
    
	g_ProcMsg.dwStact=m_strKey.GetAt(0);  //���ܵ�����
    if (!LoadPE(&lPeFile,&lPeStup))
    {
		MessageBox("�����ļ�����");
    }
	//������Ϣ
	g_ProcMsg.dwEntryRVA=lPeFile.GetNtOptionalHeader()->AddressOfEntryPoint;  //**��ڵ�
	g_ProcMsg.dwImageBase=lPeFile.GetNtOptionalHeader()->ImageBase;            //**����ַ


	lPeStupSection=lPeStup;
	LPBYTE lpStupBuffer=lPeStupSection.GetSectionData(0,&dwSectionSize);

	//----------------���������β��ض�λ������-------------------------
	//����µ�������
	lPeSection=lPeFile;
	lPeSection.AddSection(".lang",dwSectionSize,IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_EXECUTE);
    PIMAGE_SECTION_HEADER pStupSection=lPeFile.GetSection(lPeFile.GetSectionCount()-1);
	//�ض�λ����
    lStupReloc=lPeStup;
    lStupReloc.RelocationMySelf(pStupSection->VirtualAddress,lPeFile.GetNtOptionalHeader()->ImageBase);
	lPeSection.WriteSectionData(lPeSection.GetSectionCount()-1,0,lpStupBuffer,&dwSectionSize);
	
	//�ı�Դ������ڵ�
    lPeFile.GetNtOptionalHeader()->AddressOfEntryPoint=pStupSection->VirtualAddress+lPeStup.GetNtOptionalHeader()->AddressOfEntryPoint-0x1000;
	//----------------���뵼����޸������-------------------------
    lPeSection.AddSection(".rlang",dwSectionSize,IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|IMAGE_SCN_CNT_INITIALIZED_DATA);
	PIMAGE_SECTION_HEADER  pImportSection=lPeFile.GetSection(lPeFile.GetSectionCount()-1);
	
	lPeStupImport=lPeStup;
    lPeStupImport.ReLocalImport(pImportSection->VirtualAddress);
	lpStupBuffer=lPeStupSection.GetSectionData(1,&dwSectionSize);
	lPeSection.WriteSectionData(lPeSection.GetSectionCount()-1,0,lpStupBuffer,&dwSectionSize);
	
	//�ı䵼����λ��
	PIMAGE_DATA_DIRECTORY pImageDataDir=(PIMAGE_DATA_DIRECTORY)lPeStup.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
	DWORD dwStupImportRVA=pImageDataDir->VirtualAddress;
	dwStupImportRVA-=0x2000;
	dwStupImportRVA+=pImportSection->VirtualAddress;

     pImageDataDir=(PIMAGE_DATA_DIRECTORY)lPeFile.GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
	 g_ProcMsg.dwImportRVA=pImageDataDir->VirtualAddress;    //**����ԭ����ĵ����λ��
	 pImageDataDir->VirtualAddress=dwStupImportRVA;
	 
	 
	 
	 //��������
	 //lpStupBuffer=(BYTE*)lPeSection.RvaToPtr(g_ProcMsg.dwEntryRVA);

	  //��ȡOEP����������Ϣ
	 DWORD dwLengthOfOEPSection;
	 BYTE* StartOEPSection = lPeSection.GetSectionData(lPeSection.RvaToSectionIndex(g_ProcMsg.dwEntryRVA),&dwLengthOfOEPSection);

     //�жϸ����γ����Ƿ�ﵽ4*1024BYTE.�����,����һ��4*1024����.
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
		 vbEncryState.push_back(false);//��ʼ״̬δ����
		 vdLength.push_back(1024*4);


	 }


	 //д�����ļ�
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


	 //д�������������Ϣ ����ڵ� ��ַ �����λ��
	 lpStupBuffer=lPeSection.GetSectionData(lPeFile.GetSectionCount()-2,&dwSectionSize);
	 memcpy(lpStupBuffer,&g_ProcMsg,sizeof(g_ProcMsg));

	 Encry(g_ProcMsg.dwStact);//����ָ����Ϣ���м���
	 //���Ա���
	for (DWORD i=0;i<vpAddress.size();++i)
	{
		RepairProtData(i);
	}
	 //XORData(lpStupBuffer,0x100,g_ProcMsg.dwStact);
	 MessageBox("ok");
	
}

//ȡ������
bool CLProctcedDlg::RepairProtData(DWORD dwIndex)
{
	DWORD dwRet=0;

	DWORD OldProtect	= 0;//��Ҫ��ֵ,������ű�������
	PDWORD lpOldProtect =&OldProtect;

	DWORD OldProtectB	= 0;//һ������ʹ�õ�ֵ,���ڴ�ű�������
	PDWORD  lpOldProtectB=&OldProtectB;

	if(0!=VirtualProtect(vpAddress[dwIndex],vdLength[dwIndex],PAGE_NOACCESS,lpOldProtect))
	{
		return true;
	}
	return false;
}




//����ָ����
bool	CLProctcedDlg::EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock)
{
	//������δ����Ѿ�������
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
			//������һ�����ܶ�,��ȡ��Կ
			BYTE NowKey=XOREncryData((BYTE*)vpAddress[dwIndexBlock-1],vdLength[dwIndexBlock-1],0);	
			//����Կ��������
			for (DWORD dSize=0;dSize<vdLength[dwIndexBlock];++dSize)
			{
				((BYTE*)(vpAddress[dwIndexBlock]))[dSize]^=NowKey;
			}
			//�ָ���һ�������εļ��ܶ�
			XORDecryData((BYTE*)vpAddress[dwIndexBlock-1],vdLength[dwIndexBlock-1],NowKey);
		}
		else
		{
			EncryOfBlock(bFirstKey,dwIndexBlock-1);//�����Լ�.
		}
	}
	vbEncryState[dwIndexBlock]=true;
	return true;//�Ѿ�����
}


bool	CLProctcedDlg::Encry(BYTE bFirstKey)
{
	for (DWORD dwIndexBlock=0;dwIndexBlock<vdLength.size();++dwIndexBlock)
	{
		EncryOfBlock(bFirstKey,dwIndexBlock);

	}

	return  true;
}
BYTE CLProctcedDlg::XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//���ܵõ���Կ
{
	for (DWORD d=0;d<dwSize;++d)
	{
		lpBuffer[d]=lpBuffer[d]+bKey;
		bKey=lpBuffer[d]^bKey;
	}
	return bKey;
}


void CLProctcedDlg::XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey)//���ܻ�ԭ
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
