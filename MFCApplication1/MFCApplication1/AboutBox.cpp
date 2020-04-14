// AboutBox.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "AboutBox.h"
#include "afxdialogex.h"


// AboutBox 对话框

IMPLEMENT_DYNAMIC(AboutBox, CDialogEx)

AboutBox::AboutBox(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUTBOX, pParent)
{

}

AboutBox::~AboutBox()
{
}

void AboutBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AboutBox, CDialogEx)
END_MESSAGE_MAP()


// AboutBox 消息处理程序
