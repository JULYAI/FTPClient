#pragma once
#include "FTPCmd.h"
#include "Rest.h"

// FTPClient 对话框
typedef struct
{
	char szFileName[256];//文件名
	char szFileDate[20]; //文件日期
	char szFileSize[20];//文件大小
	int nType; //
}FILE_FTP_INFO;

typedef struct
{
	BOOL	m_bDirectory;
	WORD	m_bSec;
	CString length;
	CString m_strSec;
	CString m_strDate;
	CString m_strName;
	CString m_strLen;
	int m_index;
}FILE_FTP_INFO2;

class FTPClient : public CDialogEx
{
	DECLARE_DYNAMIC(FTPClient)

public:
	FTPClient(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FTPClient();

	void  FindLocFile(CString path);
	void OnInsertFile(CListCtrl* pListCtrl, FILE_FTP_INFO* pIn);
	void GetFileIcon(CString& fileName, int* iIcon, int* iIconSel = NULL);
	void GetFileIcon2(CString& fileName, int* iIcon, int* iIconSel = NULL);
	//bool TransformLine(CString strLine,FILE_FTP_INFO2 &temp);
	bool InsertServerFile(FILE_FTP_INFO2 temp);
	void RemoveLocalDirectory(CString m_localpath, CString filename);
	
	FTPCmd ftpCmd;
	HANDLE pThreadDown;
	HANDLE pThreadUp;
	HANDLE pThreadSatus;
	HANDLE pThreadTime;
	UINT m_dwID;
	CString serpath;
	CString locpath;
	CString locfile;

	CImageList* m_pDragImageList;
	BOOL m_bDragging;
	HCURSOR m_hDragCursor;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTPClient };
#endif
	enum { IDD = IDD_FTPClient };
	CProgressCtrl	m_progress;
	CListCtrl	m_serverlist;
	CListCtrl	m_locallist;
	CString	m_localpath;
	CString	m_username;
	CString	m_password;
	int		m_port;
	CString	m_serverpath;
	CString	m_filename;
	CString	m_finished;
	CString	m_finishtime;
	CString	m_speed;
	CString	m_usedtime;
	CString	m_server;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	CImageList m_ctImageList;
	int dicon;
	bool isclick;
	Rest* rd;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedviewbutton();
	afx_msg void OnBnClickedcreatelocalbutton();
	afx_msg void OnBnClickeddeletelocalbutton();
	afx_msg void OnBnClickedbacklocalbutton();
	afx_msg void OnBnClickedrefreshlocalbutton5();
	afx_msg void OnBnClickedbackserverbutton();
	afx_msg void OnBnClickedcreateserverbutton();
	afx_msg void OnBnClickeddeleteserverbutton();
	afx_msg void OnBnClickedrefreshserverbutton();
	afx_msg void OnBnClickedconnectbutton();
	afx_msg void OnBnClickedbreakbutton();
	afx_msg void OnBnClickedexitbutton();
	afx_msg void OnBnClickeduploadbutton();
	afx_msg void OnBnClickeddownloadbutton();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnInitDialog();
//	afx_msg void OnLvnBegindragLocalfilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkLocalfilelist(NMHDR* pNMHDR, LRESULT* pResult);
};
