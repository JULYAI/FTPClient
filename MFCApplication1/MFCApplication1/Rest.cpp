// Rest.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "Rest.h"
#include "afxdialogex.h"


// Rest 对话框

IMPLEMENT_DYNAMIC(Rest, CDialogEx)

Rest::Rest(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_Rest, pParent)
{

}

Rest::~Rest()
{
}

void Rest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Rest, CDialogEx)
	ON_BN_CLICKED(IDC_continueBUTTON, &Rest::OnBnClickedcontinuebutton)
	ON_BN_CLICKED(IDC_coverBUTTON, &Rest::OnBnClickedcoverbutton)
END_MESSAGE_MAP()


// Rest 消息处理程序


void Rest::OnBnClickedcontinuebutton()
{
	// TODO: 选择续传
	CDialog::OnOK();
}


void Rest::OnBnClickedcoverbutton()
{
	// TODO: 选择覆盖
}


void Rest::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}


void Rest::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnOK();
}


BOOL Rest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
