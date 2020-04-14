#pragma once


// NewDir 对话框

class NewDir : public CDialogEx
{
	DECLARE_DYNAMIC(NewDir)

public:
	NewDir(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~NewDir();
	CString DirName() { return m_dirname; }
	bool GetIsNew() { return isnew; }

// 对话框数据
	enum { IDD = IDD_NewDir };
	CString	m_dirname;
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NewDir };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	// Implementation
protected:
	bool isnew;
	// Generated message map functions
	//{{AFX_MSG(GetDirName)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickednewdirbutton();
//	afx_msg void OnBnClickedcancelnewdirbutton2();
};
