// FTPClient.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "FTPClient.h"
#include "afxdialogex.h"
#include "AboutBox.h"
#include "NewDir.h"
#include "FTPCmd.h"
#include "RemoveDir.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CTime starttime;
volatile long filelength;
volatile long length;
volatile long usedsecond;
volatile BOOL m_bRun;
CString filequeue;

bool TransformLine(CString strLine, FILE_FTP_INFO2& temp)
{
	if (strLine.IsEmpty())
		return FALSE;

	char ch = strLine.GetAt(0);
	if (ch == 'd' || ch == 'D') {
		temp.m_bDirectory = TRUE;
	}
	else
		if (ch == '-')
			temp.m_bDirectory = FALSE;
		else {
			if (strLine.Find("<DIR>") != -1 || strLine.Find("<dir>") != -1)
				temp.m_bDirectory = TRUE;
			else
				temp.m_bDirectory = FALSE;
			if (strLine.GetLength() < 40)
				return FALSE;
			temp.m_strName = strLine.Mid(39);
			temp.m_strDate = strLine.Left(18);
			temp.m_strName.TrimLeft();
			temp.m_strName.TrimRight();
			return TRUE;
		}
	temp.m_strSec = strLine.Mid(0, 10);
	int ndx = strLine.Find(':');
	if (ndx == -1) {
		if ((ndx = strLine.Find("Jan")) != -1);
		else if ((ndx = strLine.Find("Feb")) != -1);
		else if ((ndx = strLine.Find("Mar")) != -1);
		else if ((ndx = strLine.Find("Apr")) != -1);
		else if ((ndx = strLine.Find("May")) != -1);
		else if ((ndx = strLine.Find("Jun")) != -1);
		else if ((ndx = strLine.Find("Jul")) != -1);
		else if ((ndx = strLine.Find("Aug")) != -1);
		else if ((ndx = strLine.Find("Sep")) != -1);
		else if ((ndx = strLine.Find("Oct")) != -1);
		else if ((ndx = strLine.Find("Nov")) != -1);
		else if ((ndx = strLine.Find("Dec")) != -1);

		temp.m_strName = strLine.Mid(ndx + 12);
		temp.m_strName.TrimLeft();
		temp.m_strName.TrimRight();
		temp.m_strDate = strLine.Mid(ndx, 12);
		temp.m_strLen = strLine.Left(ndx - 1);
		temp.m_strLen = temp.m_strLen.Right(temp.m_strLen.GetLength() - temp.m_strLen.ReverseFind(' '));

	}
	else {
		temp.m_index = ndx;
		temp.m_strName = strLine.Mid(ndx + 3);
		temp.m_strName.TrimLeft();
		temp.m_strName.TrimRight();
		temp.m_strDate = strLine.Mid(ndx - 9, 12);
		temp.m_strLen = strLine.Left(ndx - 10);
		temp.m_strLen = temp.m_strLen.Right(temp.m_strLen.GetLength() - temp.m_strLen.ReverseFind(' '));
	}

	int h = atoi(temp.m_strLen);
	if (h > (1024 * 1024))
	{
		temp.m_strLen.Format("%.2f", h * 1.0 / (1024 * 1024));
		temp.m_strLen += "MB";
	}
	else if (h > 1024)
	{
		temp.m_strLen.Format("%d", h / 1024);
		temp.m_strLen += "KB";
	}
	else {
		temp.m_strLen.Format("%d", h);
		temp.m_strLen += "B";
	}
	return true;
}
void DownloadDirectory(FTPCmd* Cmd, CString serverpath, CString dirname, CString locpath)
{
	int ndx = 0, flag = 0;
	CString fqueue;
	CList<FILE_FTP_INFO2, FILE_FTP_INFO2> l;
	::SetCurrentDirectory(locpath);
	::CreateDirectory(dirname, 0);
	Cmd->Command("CWD " + serverpath + "/" + dirname);
	Cmd->Command("PWD");
	Cmd->List();
	while (Cmd->GetFileItems(ndx))
	{
		flag = 1;
		FILE_FTP_INFO2 temp;
		TransformLine(Cmd->m_strLine, temp);
		if (temp.m_bDirectory == false)
			fqueue = fqueue + "等待传输    " + temp.m_strName + "\r\n";
		if (temp.m_strName != "." && temp.m_strName != "..")
			l.AddTail(temp);
	}
	filequeue = fqueue + filequeue;
	::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
	if (flag == 0)return;
	FILE_FTP_INFO2 t;
	POSITION pos = l.GetHeadPosition();
	while (pos != l.GetTailPosition())
	{
		t = l.GetAt(pos);
		if (t.m_bDirectory == true)
			DownloadDirectory(Cmd, serverpath + "/" + dirname, t.m_strName, locpath + dirname + "\\");
		else
		{
			Cmd->Command("CWD " + serverpath + "/" + dirname);
			Cmd->Command("PWD");

			filequeue = "正在传输" + filequeue.Right(filequeue.GetLength() - 8);
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_fileNameSTATIC, t.m_strName + " " + t.m_strLen);
			filelength = atoi(t.m_strLen);
			if (t.m_strLen.Find("M") != -1)
				filelength *= 1024 * 1024;
			else if (t.m_strLen.Find("K") != -1)
				filelength *= 1024;
			starttime = CTime::GetCurrentTime();
			length = 0;
			usedsecond = 0;
			Cmd->MoveFile(t.m_strName, locpath + dirname + "\\" + t.m_strName, Cmd->ispasv, Cmd->isins, 1, &length, &filelength);
			int i = filequeue.Find("等待传输");
			if (i == -1)
				filequeue = "";
			else
				filequeue = filequeue.Right(filequeue.GetLength() - i);
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
		}
		l.GetNext(pos);
	}
	t = l.GetAt(l.GetTailPosition());
	if (t.m_bDirectory == true)
		DownloadDirectory(Cmd, serverpath + "/" + dirname, t.m_strName, locpath + dirname + "\\");
	else
	{
		Cmd->Command("CWD " + serverpath + "/" + dirname);
		Cmd->Command("PWD");

		filequeue = "正在传输" + filequeue.Right(filequeue.GetLength() - 8);
		::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
		::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_fileNameSTATIC, t.m_strName + " " + t.m_strLen);
		filelength = atoi(t.m_strLen);
		if (t.m_strLen.Find("M") != -1)
			filelength *= 1024 * 1024;
		else if (t.m_strLen.Find("K") != -1)
			filelength *= 1024;
		starttime = CTime::GetCurrentTime();
		length = 0;
		usedsecond = 0;
		Cmd->MoveFile(t.m_strName, locpath + dirname + "\\" + t.m_strName, Cmd->ispasv, Cmd->isins, 1, &length, &filelength);
		int i = filequeue.Find("等待传输");
		if (i == -1)
			filequeue = "";
		else
			filequeue = filequeue.Right(filequeue.GetLength() - i);
		::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
	}

}
void UploadDirectory(FTPCmd* Cmd, CString localpath, CString dirname, CString serverpath)
{
	Cmd->Command("CWD " + serverpath);
	Cmd->Command("PWD");
	Cmd->Command("MKD " + dirname);
	CFileFind fileFind;
	CString fqueue;
	BOOL bContinue;
	::SetCurrentDirectory(localpath + dirname + "\\");
	bContinue = fileFind.FindFile("*.*");
	CString str;
	while (bContinue)
	{
		bContinue = fileFind.FindNextFile();
		str = fileFind.GetFileName();
		if (str == "." || str == "..")	continue;
		if (fileFind.IsDirectory() == false)
			fqueue = fqueue + "等待传输    " + str + "\r\n";
	}
	filequeue = fqueue + filequeue;
	::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
	::SetCurrentDirectory(localpath + dirname + "\\");
	bContinue = fileFind.FindFile("*.*");
	while (bContinue)
	{
		bContinue = fileFind.FindNextFile();
		str = fileFind.GetFileName();
		if (str == "." || str == "..")	continue;
		if (fileFind.IsDirectory())
			UploadDirectory(Cmd, localpath + dirname + "\\", str, serverpath + "/" + dirname);
		else
		{
			CString strlen;
			Cmd->Command("CWD " + serverpath + "/" + dirname);
			Cmd->Command("PWD");
			filelength = fileFind.GetLength();
			if (filelength > (1024 * 1024))
			{
				strlen.Format("%.2f", filelength * 1.0 / (1024 * 1024));
				strlen += "MB";
			}
			else if (filelength > 1024)
			{
				strlen.Format("%d", filelength / 1024);
				strlen += "KB";
			}
			else {
				strlen.Format("%d", filelength);
				strlen += "B";
			}
			starttime = CTime::GetCurrentTime();
			length = 0;
			usedsecond = 0;

			filequeue = "正在传输" + filequeue.Right(filequeue.GetLength() - 8);
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_fileNameSTATIC, str + " " + strlen);
			Cmd->MoveFile(str, localpath + dirname + "\\" + str, Cmd->ispasv, Cmd->isins, 0, &length, &filelength);
			int i = filequeue.Find("等待传输");
			if (i == -1)
				filequeue = "";
			else
				filequeue = filequeue.Right(filequeue.GetLength() - i);
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, filequeue);
		}
	}

}
void DeleteServerDirectory(FTPCmd* Cmd, CString serverpath, CString dirname)
{
	Cmd->Command("CWD " + serverpath);
	Cmd->Command("PWD");
	Cmd->Command("RMD " + dirname);
	if (Cmd->m_retmsg.Find("not") == -1)
		return;
	int ndx = 0, flag = 0;
	CList<FILE_FTP_INFO2, FILE_FTP_INFO2> l;
	Cmd->Command("CWD " + serverpath + "/" + dirname);
	Cmd->Command("PWD");
	Cmd->List();
	while (Cmd->GetFileItems(ndx))
	{
		flag = 1;
		FILE_FTP_INFO2 temp;
		TransformLine(Cmd->m_strLine, temp);
		if (temp.m_strName != "." && temp.m_strName != "..")
			l.AddTail(temp);
	}
	if (flag == 0)return;
	FILE_FTP_INFO2 t;
	POSITION pos = l.GetHeadPosition();
	while (pos != l.GetTailPosition())
	{
		t = l.GetAt(pos);
		if (t.m_bDirectory == true)
			DeleteServerDirectory(Cmd, serverpath + "/" + dirname, t.m_strName);
		else
		{
			Cmd->Command("CWD " + serverpath + "/" + dirname);
			Cmd->Command("PWD");
			Cmd->Command("DELE " + t.m_strName);
		}
		l.GetNext(pos);
	}
	t = l.GetAt(l.GetTailPosition());
	if (t.m_bDirectory == true)
		DeleteServerDirectory(Cmd, serverpath + "/" + dirname, t.m_strName);
	else
	{
		Cmd->Command("CWD " + serverpath + "/" + dirname);
		Cmd->Command("PWD");
		Cmd->Command("DELE " + t.m_strName);
	}
	Cmd->Command("CWD " + serverpath);
	Cmd->Command("PWD");
	Cmd->Command("RMD " + dirname);
}
UINT  __stdcall ThreadTime(LPVOID lpParam) {
	CTime time;
	CString strTime, strTime2;
	while (m_bRun)
	{
		time = CTime::GetCurrentTime();
		CTimeSpan time1 = time - starttime;
		usedsecond = time1.GetTotalSeconds();
		strTime = time1.Format("%H:%M:%S");
		::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_usedTimeSTATIC, strTime);
		if (usedsecond != 0 && length != 0) {
			int second = (filelength - length) / (length / usedsecond);
			int day = second / (24 * 3600);
			second -= day * 24 * 3600;
			int hour = second / 3600;
			second -= hour * 3600;
			int minute = second / 60;
			second -= minute * 60;
			CTimeSpan time2(day, hour, minute, second);
			strTime2 = time2.Format("%H:%M:%S");
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_finishedTimeSTATIC, strTime2);
		}
		Sleep(1000);
	}
	return 0;
}
UINT  __stdcall ThreadSatus(LPVOID lpParam) {
	FTPClient* pThis = reinterpret_cast<FTPClient*>(lpParam);
	CString strspeed;

	while (m_bRun) {
		CString s;

		if (length < 1024)
			s.Format("%dB", length);
		else if (length < 1024 * 1024)
			s.Format("%.2fKB", length * 1.0 / 1024);
		else
			s.Format("%.2fMB", length * 1.0 / 1024 / 1024);
		::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_STATICFINISHI, s);

		pThis->m_progress.SetPos(length * 100.0 / filelength);

		if (usedsecond != 0) {
			int speed = length / usedsecond;
			if (speed < 1024)
				strspeed.Format("%dB/s", speed);
			else if (speed < 1024 * 1024)
				strspeed.Format("%.2fKB/s", speed * 1.0 / 1024);
			else
				strspeed.Format("%.2fMB/s", speed * 1.0 / 1024 / 1024);
			::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_speedSTATIC, strspeed);
		}




		Sleep(100);
	}
	return 0;
}

UINT  __stdcall ThreadDownload(LPVOID lpParam) {
	FTPClient* pThis = reinterpret_cast<FTPClient*>(lpParam);
	if (filelength > 0) {
		::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, "当前传输    " + pThis->serpath);
		pThis->ftpCmd.MoveFile(pThis->serpath, pThis->locpath, pThis->ftpCmd.ispasv, pThis->ftpCmd.isins, 1, &length, &filelength);
	}
	else
		DownloadDirectory(&(pThis->ftpCmd), pThis->m_serverpath, pThis->serpath, pThis->m_localpath);

	pThis->m_locallist.DeleteAllItems();
	pThis->FindLocFile(pThis->m_localpath);
	m_bRun = false;
	::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, "传输完毕！");
	filequeue = "";
	AfxMessageBox("文件下载完毕！");
	return 0;
}
UINT  __stdcall ThreadUpload(LPVOID lpParam) {
	FTPClient* pThis = reinterpret_cast<FTPClient*>(lpParam);
	CString t;
	if (filelength > 0) {
		::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, "当前传输    " + pThis->serpath);
		pThis->ftpCmd.MoveFile(pThis->serpath, pThis->locpath, pThis->ftpCmd.ispasv, pThis->ftpCmd.isins, 0, &length, &filelength);
	}
	else
		UploadDirectory(&(pThis->ftpCmd), pThis->m_localpath, pThis->locfile, pThis->m_serverpath);
	pThis->ftpCmd.Command("CWD " + pThis->m_serverpath);
	pThis->ftpCmd.Command("PWD");
	pThis->ftpCmd.List();
	int ndx = 0;
	pThis->m_serverlist.DeleteAllItems();
	while (pThis->ftpCmd.GetFileItems(ndx))
	{
		FILE_FTP_INFO2 temp;
		TransformLine(pThis->ftpCmd.m_strLine, temp);
		if (temp.m_strName != "." && temp.m_strName != "..")
			pThis->InsertServerFile(temp);
	}
	m_bRun = false;
	::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT, "传输完毕！");
	filequeue = "";
	AfxMessageBox("文件上传完毕！");
	return 0;
}
// FTPClient 对话框

IMPLEMENT_DYNAMIC(FTPClient, CDialogEx)

FTPClient::FTPClient(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FTPClient, pParent)
{
	m_localpath = _T("C:\\");//默认本地路径
	m_username = _T("");//用户名
	m_password = _T("");//密码
	m_port = 21; //端口
	m_serverpath = _T("");
	m_filename = _T("");
	m_finished = _T("");
	m_finishtime = _T("");
	m_speed = _T("");
	m_usedtime = _T("");
	m_server = _T("");
	isclick = false;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

FTPClient::~FTPClient()
{
}

void FTPClient::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_serverAddrEDIT, m_server);
	DDX_Text(pDX, IDC_portEDIT, m_port);
	DDX_Text(pDX, IDC_userNameEDIT, m_username);
	DDX_Text(pDX, IDC_passwordEDIT, m_password);
	DDX_Text(pDX, IDC_localPathEDIT, m_localpath);
	DDX_Text(pDX, IDC_serverPathEDIT, m_serverpath);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_ServerFileList, m_serverlist);
	DDX_Control(pDX, IDC_LocalFileList, m_locallist);
	DDX_Text(pDX, IDC_fileNameSTATIC, m_filename);
	DDX_Text(pDX, IDC_speedSTATIC, m_speed);
	DDX_Text(pDX, IDC_usedTimeSTATIC, m_usedtime);
	DDX_Text(pDX, IDC_STATICFINISHI, m_finished);
	DDX_Text(pDX, IDC_finishedTimeSTATIC, m_finishtime);
}


BEGIN_MESSAGE_MAP(FTPClient, CDialogEx)

	ON_BN_CLICKED(IDC_viewBUTTON, &FTPClient::OnBnClickedviewbutton)
	ON_BN_CLICKED(IDC_createLocalBUTTON, &FTPClient::OnBnClickedcreatelocalbutton)
	ON_BN_CLICKED(IDC_deleteLocalBUTTON, &FTPClient::OnBnClickeddeletelocalbutton)
	ON_BN_CLICKED(IDC_backLocalBUTTON, &FTPClient::OnBnClickedbacklocalbutton)
	ON_BN_CLICKED(IDC_refreshLocalBUTTON5, &FTPClient::OnBnClickedrefreshlocalbutton5)
	ON_BN_CLICKED(IDC_backServerBUTTON, &FTPClient::OnBnClickedbackserverbutton)
	ON_BN_CLICKED(IDC_createServerBUTTON, &FTPClient::OnBnClickedcreateserverbutton)
	ON_BN_CLICKED(IDC_deleteServerBUTTON, &FTPClient::OnBnClickeddeleteserverbutton)
	ON_BN_CLICKED(IDC_refreshServerBUTTON, &FTPClient::OnBnClickedrefreshserverbutton)
	ON_BN_CLICKED(IDC_connectBUTTON, &FTPClient::OnBnClickedconnectbutton)
	ON_BN_CLICKED(IDC_breakBUTTON, &FTPClient::OnBnClickedbreakbutton)
	ON_BN_CLICKED(IDC_uploadBUTTON, &FTPClient::OnBnClickeduploadbutton)
	ON_BN_CLICKED(IDC_downloadBUTTON, &FTPClient::OnBnClickeddownloadbutton)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
//	ON_NOTIFY(LVN_BEGINDRAG, IDC_LocalFileList, &FTPClient::OnLvnBegindragLocalfilelist)
	ON_NOTIFY(NM_DBLCLK, IDC_LocalFileList, &FTPClient::OnNMDblclkLocalfilelist)
END_MESSAGE_MAP()


// FTPClient 消息处理程序
BOOL FTPClient::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// IDM_ABOUTBOX must be in the system command range.
	//ASSERT((IDD_ABOUTBOX & 0xFFF0) == IDD_ABOUTBOX);
	//ASSERT(IDD_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDD_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO:  在此添加额外的初始化
	//创建ImageList
	HIMAGELIST hImageList;
	//定义文件信息变量
	SHFILEINFO shFi;
	//获取文件图标信息
	hImageList = (HIMAGELIST)SHGetFileInfo("c:\\", 0, &shFi, sizeof(shFi),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	//将文件图象列表存入m_ctImageList
	m_ctImageList.m_hImageList = hImageList;
	m_locallist.SetImageList(&m_ctImageList, LVSIL_SMALL);
	m_serverlist.SetImageList(&m_ctImageList, LVSIL_SMALL);
	m_locallist.InsertColumn(0, _T("名称"), 2, 200);
	m_locallist.InsertColumn(1, _T("大小"), 3, 60);
	m_locallist.InsertColumn(2, _T("修改时间"), 0, 140);
	m_locallist.DeleteAllItems();
	FindLocFile(m_localpath);
	m_serverlist.InsertColumn(0, _T("名称"), 2, 160);
	m_serverlist.InsertColumn(1, _T("大小"), 3, 60);
	m_serverlist.InsertColumn(2, _T("修改时间"), 0, 140);
	m_serverlist.InsertColumn(3, _T("权限"), 0, 100);
	m_serverlist.DeleteAllItems();
	GetDlgItem(IDC_breakBUTTON)->EnableWindow(FALSE);
	//	AlienCmd.set(&m_message);
	((CButton*)GetDlgItem(IDC_motiveRADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_passiveRADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_continueRADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_coverRADIO))->SetCheck(0);
	m_progress.SetRange(0, 99);
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void FTPClient::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//return;
	//if (m_bDragging)
	//{
	//	CRect rect;
	//	m_locallist.GetWindowRect(&rect);
	//	ClientToScreen(&point);
	//	rect.PtInRect(point);
	//	SetCursor(m_hDragCursor);
	//	m_pDragImageList->DragMove(point);
	//}

	//CDialog::OnMouseMove(nFlags, point);
}


void FTPClient::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	//				   // TODO: 在此处添加消息处理程序代码
	//				   // 不为绘图消息调用 CDialogEx::OnPaint()
	//if (IsIconic())
	//{
	//	CPaintDC dc(this); // device context for painting

	//	SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

	//	// Center icon in client rectangle
	//	int cxIcon = GetSystemMetrics(SM_CXICON);
	//	int cyIcon = GetSystemMetrics(SM_CYICON);
	//	CRect rect;
	//	GetClientRect(&rect);
	//	int x = (rect.Width() - cxIcon + 1) / 2;
	//	int y = (rect.Height() - cyIcon + 1) / 2;

	//	// Draw the icon
	//	dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	//{
	//	CDialog::OnPaint();
	//}
}


HCURSOR FTPClient::OnQueryDragIcon()
{
	//// TODO: 在此添加消息处理程序代码和/或调用默认值
	return (HCURSOR)m_hIcon;
	////return CDialogEx::OnQueryDragIcon();
}


void FTPClient::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//if ((nID & 0xFFF0) == IDD_ABOUTBOX)
	//{
	//	AboutBox dlgAbout;
	//	dlgAbout.DoModal();
	//}
	//else
	//{
	//	CDialog::OnSysCommand(nID, lParam);
	//}
}
//=========================================================
void  FTPClient::FindLocFile(CString path)
{

	FILE_FTP_INFO* pInfo = new FILE_FTP_INFO;
	CFileFind fileFind;
	CFile f;

	UINT i = 0;
	CTime time;
	BOOL bContinue;
	::SetCurrentDirectory(path);
	bContinue = fileFind.FindFile("*.*");
	CString str;
	while (bContinue)
	{
		bContinue = fileFind.FindNextFile();
		if (fileFind.IsHidden())continue;
		str = fileFind.GetFileName();
		if (str == ".." || str == ".")continue;
		strcpy(pInfo->szFileName, str);
		fileFind.GetLastWriteTime(time);
		//	str=time.Format("%x");
	//		str=time.GetYear()+"-"+time.GetMonth()+"-"+time.GetDay()+" "+time.GetHour()+":"+time.GetMinute()+":"+time.GetSecond();
		str.Format("%d-%d-%d %d:%d:%d", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
		strcpy(pInfo->szFileDate, str);
		if (fileFind.IsDirectory())
		{
			strcpy(pInfo->szFileSize, "");
			pInfo->nType = 1;
			OnInsertFile(&m_locallist, pInfo);
		}
		else
		{
			int h = fileFind.GetLength();
			if (h > (1024 * 1024))
			{
				str.Format("%.2f", h * 1.0 / (1024 * 1024));
				str += "MB";
			}
			else if (h > 1024)
			{
				str.Format("%d", h / 1024);
				str += "KB";
			}
			else {
				str.Format("%d", h);
				str += "B";
			}
			strcpy(pInfo->szFileSize, str);
			pInfo->nType = 0;
			OnInsertFile(&m_locallist, pInfo);
		}
	}
	delete pInfo;
	fileFind.Close();

}
void FTPClient::OnInsertFile(CListCtrl* pListCtrl, FILE_FTP_INFO* pIn)
{
	FILE_FTP_INFO* pInfo = (FILE_FTP_INFO*)pIn;
	LVITEM	lvIt;
	int iIcon, nItem;
	nItem = pListCtrl->GetItemCount();
	CString fileName = pInfo->szFileName;
	GetFileIcon(fileName, &iIcon, NULL);
	if (pInfo->nType == 1)dicon = iIcon;
	lvIt.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvIt.iImage = iIcon;
	lvIt.lParam = pInfo->nType;
	lvIt.pszText = pInfo->szFileName;
	lvIt.iSubItem = 0;
	lvIt.iItem = nItem;
	int iPos = pListCtrl->InsertItem(&lvIt);
	lvIt.mask = LVIF_TEXT;
	lvIt.iItem = iPos;
	lvIt.pszText = pInfo->szFileSize;
	lvIt.iSubItem = 1;
	pListCtrl->SetItem(&lvIt);
	lvIt.mask = LVIF_TEXT;
	lvIt.pszText = pInfo->szFileDate;
	lvIt.iSubItem = 2;
	pListCtrl->SetItem(&lvIt);
	//m_ctrlLocalFile.SetItem(&lvIt);


	//IsFree=TRUE;
}
void FTPClient::OnBnClickedviewbutton()
{
	// TODO: 浏览本地目录
	CString str;
	BROWSEINFO bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = "Select folder";
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return;
	SHGetPathFromIDList(idl, str.GetBuffer(MAX_PATH));
	str.ReleaseBuffer();
	m_localpath = str;//为对话框中与一编辑框对应的CString型变量，保存并显示选中的路径。
	if (str.GetAt(str.GetLength() - 1) != '\\')
		m_localpath += "\\";
	::SetCurrentDirectory("d:\\");
	m_locallist.DeleteAllItems();
	FindLocFile(m_localpath);
	UpdateData(FALSE);
}
void FTPClient::GetFileIcon(CString& fileName, int* iIcon, int* iIconSel)
{

	CString str = fileName;
	if (str.Right(1) != "\\")
		str += "\\";
	SHFILEINFO shFi;
	if (!SHGetFileInfo(str, 0, &shFi, sizeof(shFi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_DISPLAYNAME))
		//	if(!SHGetFileInfo("stggt.bmp",0,&shFi,sizeof(shFi),SHGFI_ICON|SHGFI_USEFILEATTRIBUTES))
	{
		AfxMessageBox("111");
		return;
	}
	*iIcon = shFi.iIcon;
	DestroyIcon(shFi.hIcon);
	if (iIconSel)
	{
		if (!SHGetFileInfo(str, 0, &shFi, sizeof(shFi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_OPENICON | SHGFI_DISPLAYNAME))
		{
			AfxMessageBox("222");
			return;
		}
		*iIconSel = shFi.iIcon;
		DestroyIcon(shFi.hIcon);
	}
	return;
}
void FTPClient::GetFileIcon2(CString& fileName, int* iIcon, int* iIconSel)
{
	CString str = fileName;
	if (str.Right(1) != "\\")
		str += "\\";
	SHFILEINFO shFi;
	if (!SHGetFileInfo(fileName, 0, &shFi, sizeof(shFi), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
	{
		AfxMessageBox("111");
		return;
	}
	*iIcon = shFi.iIcon;
	DestroyIcon(shFi.hIcon);
	if (iIconSel)
	{
		if (!SHGetFileInfo(str, 0, &shFi, sizeof(shFi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_OPENICON | SHGFI_DISPLAYNAME))
		{
			AfxMessageBox("222");
			return;
		}
		*iIconSel = shFi.iIcon;
		DestroyIcon(shFi.hIcon);
	}
	return;
}
void FTPClient::OnBnClickedbacklocalbutton()
{
	// TODO: 本地文件夹返回上一级目录
	if (m_localpath.GetLength() < 4)return;
	int j;
	for (j = m_localpath.GetLength() - 2; j >= 0; j--)
		if (m_localpath.GetAt(j) == '\\')break;
	m_localpath = m_localpath.Left(j);
	m_localpath += "\\";
	::SetCurrentDirectory(m_localpath);
	m_locallist.DeleteAllItems();
	FindLocFile(m_localpath);
	UpdateData(FALSE);
}

void FTPClient::OnBnClickedcreatelocalbutton()
{
	// TODO: 本地文件夹新建目录
	NewDir g;
	g.DoModal();
	if (g.GetIsNew() == true)
	{
		::SetCurrentDirectory(m_localpath);
		::CreateDirectory(g.DirName(), 0);
		m_locallist.DeleteAllItems();
		FindLocFile(m_localpath);
	}
}

void FTPClient::OnBnClickeddeletelocalbutton()
{
	// TODO: 本地文件夹删除文件或目录
	UpdateData(true);
	if (m_locallist.GetSelectionMark() == -1) {
		AfxMessageBox("请选择一个文件或目录进行删除！");
		return;
	}
	CString filename;
	filename = m_locallist.GetItemText(m_locallist.GetSelectionMark(), 0);
	CFileFind fileFind;
	::SetCurrentDirectory(m_localpath);
	if (!fileFind.FindFile(filename)) {
		AfxMessageBox("文件或目录不存在！");
		return;
	}
	fileFind.FindNextFile();
	if (!fileFind.IsDirectory())
		::DeleteFile(filename);
	else
		RemoveLocalDirectory(m_localpath, filename);
	m_locallist.DeleteAllItems();
	FindLocFile(m_localpath);
}
void FTPClient::RemoveLocalDirectory(CString localpath, CString filename)
{
	::SetCurrentDirectory(localpath);
	if (RemoveDirectory(filename))
		return;
	CFileFind fileFind;
	BOOL bContinue;
	::SetCurrentDirectory(localpath + filename + "\\");
	bContinue = fileFind.FindFile("*.*");
	CString str;
	while (bContinue)
	{
		bContinue = fileFind.FindNextFile();
		str = fileFind.GetFileName();
		if (str == "." || str == "..")	continue;
		if (fileFind.IsDirectory())
			RemoveLocalDirectory(localpath + filename + "\\", str);
		else
			::DeleteFile(str);
	}
	::SetCurrentDirectory(localpath);
	fileFind.FindFile(filename);
	RemoveDirectory(filename);
}
void FTPClient::OnBnClickedrefreshlocalbutton5()
{
	// TODO: 本地文件夹刷新文件列表
	::SetCurrentDirectory(m_localpath);
	m_locallist.DeleteAllItems();
	FindLocFile(m_localpath);
	UpdateData(FALSE);
}


void FTPClient::OnBnClickedbackserverbutton()
{
	// TODO: 服务器端返回上一级目录
	UpdateData(true);
	if (m_serverpath == "/")return;
	//AfxMessageBox(m_serverpath);
	int j, ndx = 0;
	j = m_serverpath.GetLength() - 1;
	while (m_serverpath.Mid(j, 1) != "/" && j >= 0)j--;
	if (j == 0)
		m_serverpath = "/";
	else
		m_serverpath = m_serverpath.Mid(0, j);
	m_serverlist.DeleteAllItems();
	ftpCmd.Command("CWD " + m_serverpath);
	ftpCmd.Command("PWD");
	ftpCmd.List();
	while (ftpCmd.GetFileItems(ndx))
	{
		FILE_FTP_INFO2 temp;
		TransformLine(ftpCmd.m_strLine, temp);
		if (temp.m_strName != "." && temp.m_strName != "..")
			InsertServerFile(temp);
	}
	UpdateData(FALSE);
}

void FTPClient::OnBnClickedcreateserverbutton()
{
	// TODO: 服务器端新建目录
	int ndx = 0;
	NewDir g;
	g.DoModal();
	if (g.GetIsNew() == true)
	{
		m_serverlist.DeleteAllItems();
		ftpCmd.Command("MKD " + g.DirName());
		ftpCmd.Command("PWD");
		ftpCmd.List();
		while (ftpCmd.GetFileItems(ndx))
		{
			FILE_FTP_INFO2 temp;
			TransformLine(ftpCmd.m_strLine, temp);
			if (temp.m_strName != "." && temp.m_strName != "..")
				InsertServerFile(temp);
		}

	}

}

void FTPClient::OnBnClickeddeleteserverbutton()
{
	// TODO: 服务器端删除文件或目录
	if (m_serverlist.GetSelectionMark() == -1)
	{
		AfxMessageBox("请选择一个文件或目录进行删除！");
		return;
	}
	int ndx = 0;
	CString sec;
	sec = m_serverlist.GetItemText(m_serverlist.GetSelectionMark(), 3);
	if (sec.GetAt(0) == 'd') {
		ftpCmd.Command("RMD " + m_serverlist.GetItemText(m_serverlist.GetSelectionMark(), 0));
		if (ftpCmd.m_retmsg.Find("not") != -1)
		{
			RemoveDir r;
			if (r.DoModal() == IDOK)
				DeleteServerDirectory(&ftpCmd, m_serverpath, m_serverlist.GetItemText(m_serverlist.GetSelectionMark(), 0));
		}
	}
	else
		ftpCmd.Command("DELE " + m_serverlist.GetItemText(m_serverlist.GetSelectionMark(), 0));
	ftpCmd.Command("CWD " + m_serverpath);
	ftpCmd.Command("PWD");
	m_serverlist.DeleteAllItems();
	ftpCmd.List();
	ndx = 0;
	while (ftpCmd.GetFileItems(ndx))
	{
		FILE_FTP_INFO2 temp;
		TransformLine(ftpCmd.m_strLine, temp);
		if (temp.m_strName != "." && temp.m_strName != "..")
			InsertServerFile(temp);
	}
}

void FTPClient::OnBnClickedrefreshserverbutton()
{
	// TODO: 服务器端刷新文件列表
	int ndx = 0;
	m_serverlist.DeleteAllItems();
	ftpCmd.Command("CWD " + m_serverpath);
	ftpCmd.Command("PWD");
	ftpCmd.List();
	while (ftpCmd.GetFileItems(ndx))
	{
		FILE_FTP_INFO2 temp;
		TransformLine(ftpCmd.m_strLine, temp);
		if (temp.m_strName != "." && temp.m_strName != "..")
			InsertServerFile(temp);
	}
	UpdateData(FALSE);
}

void FTPClient::OnBnClickedconnectbutton()
{
	// TODO: 连接服务器
	UpdateData(true);
	int ndx = 0;
	if (!ftpCmd.LogIn(m_server, m_port, m_username, m_password, "", "", "", "", 1080, 0))
	{
		//m_Message=m_Message+m_UID+"登录失败。"+"\r\n";
		UpdateData(false);
		return;
	}
	else
	{
		ftpCmd.List();

		while (ftpCmd.GetFileItems(ndx))
		{
			FILE_FTP_INFO2 temp;
			//	AfxMessageBox(AlienCmd.m_strLine);
			TransformLine(ftpCmd.m_strLine, temp);
			if (temp.m_strName != "." && temp.m_strName != "..")
				InsertServerFile(temp);
			m_serverpath = "/";
		}

	}
	GetDlgItem(IDC_breakBUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_connectBUTTON)->EnableWindow(FALSE);
	UpdateData(false);

}

bool FTPClient::InsertServerFile(FILE_FTP_INFO2 temp)
{
	LV_ITEM lvi;
	int ndx = m_serverlist.GetItemCount();
	int iIcon;

	if (temp.m_bDirectory)
		iIcon = dicon;
	else
		GetFileIcon2(temp.m_strName, &iIcon, NULL);
	CString s;
	s.Format("%d", iIcon);
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvi.iImage = iIcon;
	lvi.lParam = 0;
	lvi.pszText = temp.m_strName.GetBuffer(0);
	temp.m_strName.ReleaseBuffer();
	lvi.iSubItem = 0;
	lvi.iItem = ndx;
	ndx = m_serverlist.InsertItem(&lvi);

	lvi.mask = LVIF_TEXT;
	lvi.pszText = temp.m_strLen.GetBuffer(0);
	temp.m_strLen.ReleaseBuffer();
	lvi.iItem = ndx;
	lvi.iSubItem = 1;
	m_serverlist.SetItem(&lvi);

	lvi.pszText = temp.m_strDate.GetBuffer(0);
	temp.m_strDate.ReleaseBuffer();
	lvi.iItem = ndx;
	lvi.iSubItem = 2;
	m_serverlist.SetItem(&lvi);

	lvi.pszText = temp.m_strSec.GetBuffer(0);
	temp.m_strSec.ReleaseBuffer();
	lvi.iItem = ndx;
	lvi.iSubItem = 3;
	m_serverlist.SetItem(&lvi);

	return true;
}


void FTPClient::OnBnClickedbreakbutton()
{
	// TODO: 断开与服务器的连接
	if (m_bRun) {
		ftpCmd.Send("ABOR");
		ftpCmd.Response();
		ftpCmd.Response();
	}
	m_bRun = false;
	ftpCmd.LogOut();
	m_serverpath.Empty();
	m_serverlist.DeleteAllItems();
	GetDlgItem(IDC_breakBUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_connectBUTTON)->EnableWindow(TRUE);
	UpdateData(false);
}

void FTPClient::OnBnClickeduploadbutton()
{
	// TODO: 从本地上传文件至服务器
	UpdateData(TRUE);
	if (m_locallist.GetSelectionMark() == -1) {
		AfxMessageBox("请选择一个文件或目录进行上传！");
		return;
	}
	CString filename;
	locfile = filename = m_locallist.GetItemText(m_locallist.GetSelectionMark(), 0);
	CString strlength = (m_locallist.GetItemText(m_locallist.GetSelectionMark(), 1));
	if (strlength.GetLength() < 1)
		strlength = "0";
	filelength = atoi(strlength);
	if (strlength.Find("M") != -1)
		filelength *= 1024 * 1024;
	else if (strlength.Find("K") != -1)
		filelength *= 1024;
	m_filename.Format("%s %s", filename, strlength);
	m_bRun = true;
	starttime = CTime::GetCurrentTime();
	length = 0;
	usedsecond = 0;
	UpdateData(false);
	if (m_localpath.Right(1) != "\\")
		locpath = m_localpath + "\\";
	else
		locpath = m_localpath;
	locpath += filename;
	serpath = filename;
	UpdateData(TRUE);

	ftpCmd.ispasv = ((CButton*)GetDlgItem(IDC_passiveRADIO))->GetCheck();
	ftpCmd.isins = ((CButton*)GetDlgItem(IDC_continueRADIO))->GetCheck();

	pThreadTime = (HANDLE)_beginthreadex(NULL,
		0,
		ThreadTime,
		NULL,
		0,
		&m_dwID);
	pThreadUp = (HANDLE)_beginthreadex(NULL,
		0,
		ThreadUpload,
		(void*)this,
		0,
		&m_dwID);
	pThreadSatus = (HANDLE)_beginthreadex(NULL,
		0,
		ThreadSatus,
		(void*)this,
		0,
		&m_dwID);
}

void FTPClient::OnBnClickeddownloadbutton()
{
	// TODO: 远程下载文件至本地
	UpdateData(TRUE);
	if (m_serverlist.GetSelectionMark() == -1) {
		AfxMessageBox("请选择一个文件或目录进行下载！");
		return;
	}
	CString filename;
	filename = m_serverlist.GetItemText(m_serverlist.GetSelectionMark(), 0);
	CString strlength = (m_serverlist.GetItemText(m_serverlist.GetSelectionMark(), 1));
	filelength = atoi(strlength);
	if (strlength.Find("M") != -1)
		filelength *= 1024 * 1024;
	else if (strlength.Find("K") != -1)
		filelength *= 1024;
	m_filename.Format("%s %s", filename, strlength);
	m_bRun = true;
	starttime = CTime::GetCurrentTime();
	length = 0;
	usedsecond = 0;
	UpdateData(false);
	serpath = filename;
	if (m_localpath.Right(1) != "\\")
		locpath = m_localpath + "\\";
	else
		locpath = m_localpath;
	locpath += filename;

	ftpCmd.ispasv = ((CButton*)GetDlgItem(IDC_passiveRADIO))->GetCheck();
	ftpCmd.isins = ((CButton*)GetDlgItem(IDC_continueRADIO))->GetCheck();

	pThreadTime = (HANDLE)_beginthreadex(NULL,
		0,
		ThreadTime,
		NULL,
		0,
		&m_dwID);
	pThreadDown = (HANDLE)_beginthreadex(NULL,
		0,
		ThreadDownload,
		(void*)this,
		0,
		&m_dwID);
	pThreadSatus = (HANDLE)_beginthreadex(NULL,
		0,
		ThreadSatus,
		(void*)this,
		0,
		&m_dwID);
}








//void FTPClient::OnLvnBegindragLocalfilelist(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	return;
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
//	// TODO: Add your control notification handler code here
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;    	
//	*pResult = 0;
//	int nItem = pNMListView->iItem;
//	POINT pt = pNMListView->ptAction;
//
//	m_pDragImageList = m_locallist.CreateDragImage(nItem, &pt);
//	m_bDragging = TRUE;
//	m_pDragImageList->BeginDrag(0, CPoint(0, 0));
//	m_pDragImageList->DragEnter(NULL, pt);
//	SetCapture();
//	SetCursor(m_hDragCursor);
//	AfxMessageBox("ax");
//	*pResult = 0;
//}


void FTPClient::OnNMDblclkLocalfilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 双击打开目录
	if (m_locallist.GetSelectionMark() == -1)return;
	NMLVDISPINFO* pLocInfo = (NMLVDISPINFO*)pNMHDR;
	CString str, str1, filename, buf;
	int iIcon, iIconSel;
	str.Format("%d", pLocInfo->item);
	int nItem = atoi(str);
	DWORD dwFlag = m_locallist.GetItemData(nItem);
	if (dwFlag == 0)
	{
		AfxMessageBox("这是一个数据文件，不是目录！");
		return;
	}
	str = m_locallist.GetItemText(atoi(str), 0);
	m_localpath += str + "\\";
	::SetCurrentDirectory(m_localpath);
	m_locallist.DeleteAllItems();
	FindLocFile(m_localpath);
	UpdateData(FALSE);
	*pResult = 0;
	*pResult = 0;
}
