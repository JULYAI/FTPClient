#pragma once


// RemoveDir 对话框

class RemoveDir : public CDialogEx
{
	DECLARE_DYNAMIC(RemoveDir)

public:
	RemoveDir(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RemoveDir();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RemoveDir };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedremovebutton();
//	afx_msg void OnBnClickedcancelremovebutton();
	afx_msg void OnBnClickedremovebutton();
};
