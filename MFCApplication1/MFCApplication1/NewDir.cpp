// NewDir.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "NewDir.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// NewDir 对话框

IMPLEMENT_DYNAMIC(NewDir, CDialogEx)

NewDir::NewDir(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NewDir, pParent)
{
	m_dirname = _T("");
}

NewDir::~NewDir()
{
}

void NewDir::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dirname);
}


BEGIN_MESSAGE_MAP(NewDir, CDialogEx)
//	ON_BN_CLICKED(IDC_newDirBUTTON, &NewDir::OnBnClickednewdirbutton)
//	ON_BN_CLICKED(IDC_cancelNewDirBUTTON2, &NewDir::OnBnClickedcancelnewdirbutton2)
END_MESSAGE_MAP()


// NewDir 消息处理程序
void NewDir::OnOK()
{
	// TODO: Add extra validation here
	isnew = true;
	CDialog::OnOK();
}

void NewDir::OnCancel()
{
	// TODO: Add extra cleanup here
	isnew = false;
	CDialog::OnCancel();
}
