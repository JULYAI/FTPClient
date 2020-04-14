#pragma once


// Rest 对话框

class Rest : public CDialogEx
{
	DECLARE_DYNAMIC(Rest)

public:
	Rest(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Rest();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Rest };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedcontinuebutton();
	afx_msg void OnBnClickedcoverbutton();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
};
