﻿#pragma once


// AboutBox 对话框

class AboutBox : public CDialogEx
{
	DECLARE_DYNAMIC(AboutBox)

public:
	AboutBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~AboutBox();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
