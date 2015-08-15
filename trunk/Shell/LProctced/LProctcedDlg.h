// LProctcedDlg.h : header file
//

#if !defined(AFX_LPROCTCEDDLG_H__A93415ED_E194_4A59_96E4_C02F63D83622__INCLUDED_)
#define AFX_LPROCTCEDDLG_H__A93415ED_E194_4A59_96E4_C02F63D83622__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLProctcedDlg dialog
#include "LPEFile.h"
#include <vector>

using std::vector;

class CLProctcedDlg : public CDialog
{
// Construction
public:
	BOOL LoadPE(LPEFile* lPeFile,LPEFile* lPEStup);
	CLProctcedDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLProctcedDlg)
	enum { IDD = IDD_LPROCTCED_DIALOG };
	CString	m_strKey;
	CString	m_strFileName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLProctcedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLProctcedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonOpen();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



public:
	bool	Encry(BYTE bFirstKey);
	bool	Decry(BYTE bFirstKey);
	BYTE XOREncryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
	void XORDecryData(BYTE* lpBuffer,DWORD dwSize,BYTE bKey);
	bool	EncryOfBlock(BYTE bFirstKey,DWORD dwIndexBlock);
	bool	RepairProtData(DWORD dwIndex);
private:
	vector<DWORD>		vdLength;		//长度
	vector<bool>		vbEncryState;	//加密状态
	vector<PVOID>		vpAddress;		//起始地址
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LPROCTCEDDLG_H__A93415ED_E194_4A59_96E4_C02F63D83622__INCLUDED_)
