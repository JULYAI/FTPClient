#if !defined(AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_)
#define AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif

class FTPClientDlg;
class FTPCmd
{
public:
	FTPCmd();
	~FTPCmd();
	BOOL Open(CString serverHost, int serverPort);
	void Close();
	BOOL Send(CString outputstr);
	BOOL Response();
	BOOL Command(CString command);
	BOOL GetFileItems(int& ndx);
	BOOL List();
	BOOL MoveFile(CString remotefile, CString localfile, BOOL pasv, BOOL ins, BOOL get, volatile long *length, volatile long * filelength);
	BOOL LogIn(CString hostname, int hostport, CString username, CString password, CString acct, CString sHost, CString sUsername, CString sPassword, int sPort, int logintype);
	void LogOut();
public:
	CString m_retmsg;
	CByteArray m_buf;
	CString m_strLine;
	CString m_ftpHost;
	CString m;
	SOCKET m_Ctrlsoc; //¿ØÖÆÃüÁî´«ÊäÌ×½Ó×Ö
	int resnum;
	char m_errString[100];
	bool ispasv;
	bool isins;
};

#endif
