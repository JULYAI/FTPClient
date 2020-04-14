// RemoveDir.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "RemoveDir.h"
#include "afxdialogex.h"


// RemoveDir 对话框

IMPLEMENT_DYNAMIC(RemoveDir, CDialogEx)

RemoveDir::RemoveDir(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RemoveDir, pParent)
{

}

RemoveDir::~RemoveDir()
{
}

void RemoveDir::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RemoveDir, CDialogEx)
//	ON_BN_CLICKED(IDC_removeBUTTON, &RemoveDir::OnBnClickedremovebutton)
//	ON_BN_CLICKED(IDC_cancelRemoveBUTTON, &RemoveDir::OnBnClickedcancelremovebutton)
ON_BN_CLICKED(IDC_removeBUTTON, &RemoveDir::OnBnClickedremovebutton)
END_MESSAGE_MAP()


// RemoveDir 消息处理程序

void RemoveDir::OnBnClickedremovebutton()
{
	// TODO: 删除非空目录
	CDialog::OnOK();
}
