#include "pch.h"
#include "MFCApplication1.h"
#include "FTPClient.h"
#include "FTPCmd.h"

FTPCmd::FTPCmd()
{
	m_retmsg = "";
}

FTPCmd::~FTPCmd()
{
	Close();
}

BOOL FTPCmd::Open(CString serverHost, int serverPort)
{
	int nRet = 0;
	//Create socket
	m_Ctrlsoc = socket(AF_INET, SOCK_STREAM, 0);

	if (m_Ctrlsoc = SOCKET_ERROR) {
		nRet = closesocket(m_Ctrlsoc);
		::WSACleanup();
		return FALSE;
	}
	struct sockaddr_in sockAddr;
	sockAddr.sin_addr.S_un.S_addr = inet_addr((char *)serverHost.GetBuffer(serverHost.GetLength()));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(serverPort);
	//Connect to the server
	if (0 != (connect(m_Ctrlsoc, (sockaddr*)&sockAddr, sizeof(sockAddr)))) {
		return FALSE;
	}
	return TRUE;
}

void FTPCmd::Close()
{
	::shutdown(m_Ctrlsoc, 2);
	::closesocket(m_Ctrlsoc);
	::WSACleanup();
	return;
}

BOOL FTPCmd::Send(CString outputstr)
{
	int nRet = 0;
	outputstr += "\r\n";
	(m) = (m)+"[R] " + outputstr;
	::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT2,m);
	nRet = ::send(this->m_Ctrlsoc, 
		(char *)outputstr.GetBuffer(outputstr.GetLength()),
		outputstr.GetLength(),0);
	if (nRet == 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL FTPCmd::Response()
{
	int nRet = 0;
	char *readbuf = new char[1024];
	memset(readbuf, 0, 1024);
	nRet = ::recv(this->m_Ctrlsoc, readbuf, 1024, 0);
	CString temp(readbuf);
	CString m = m+ "[R] " + temp.Left(nRet - 1) + "\r\n";
	::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_queueEDIT2, m);  // 需要在Dlg文件中声明
	if (nRet == 0) {
		delete[] readbuf;
		return FALSE;
	}
	else {
		m_retmsg = readbuf;
	}
	// Get the 1st digit of the return code (indicates primary result)
	if (m_retmsg.GetLength() > 0) {
		resnum = m_retmsg.GetAt(0) - 48;
	}
	delete[] readbuf;
	return TRUE;
}

BOOL FTPCmd::Command(CString command)
{
	if (command != "" && !Send(command)) return FALSE;
	if ((!Response()) || (resnum != 2)) return FALSE;
	return TRUE;
}

BOOL FTPCmd::GetFileItems(int & ndx)
{
	m_strLine.Empty();
	int nBytes = m_buf.GetSize();
	BOOL bLine = FALSE; // reach the blank line or not
	while (bLine == FALSE && ndx < nBytes)
	{
		char ch = (char)(m_buf.GetAt(ndx));
		switch (ch)
		{
		case '\n': 
			bLine = TRUE;
			break;
		default:   
			m_strLine += ch;
			break;
		}
		++ndx;
	}
	m_strLine = m_strLine.Left(m_strLine.GetLength() - 1);
	return bLine;
}

BOOL FTPCmd::List()
{
	CString localhost, temp, strOut, remotehost;
	UINT localsock = 0, i;
	CFile datafile;
	SOCKET sockSrvr;			//Listen to the ftpsocket，wait for the connection
	SOCKET datachannel;			//socket for data transport
	int num = 0, sum, nRet;
	const int BUFSIZE = 4096;
	DWORD lpArgument = 0;

	m_buf.RemoveAll();
	m_buf.SetSize(BUFSIZE);
	if (!Command(CString("TYPE A")))
		return FALSE; // binary mode

	struct  sockaddr_in saSrc;
	struct  sockaddr_in saDest;
	int addr_size;
	addr_size = sizeof(sockaddr_in);
	// Get local IP address and control channel port
	memset(&saSrc, 0, sizeof(sockaddr_in));
	nRet = ::getsockname(m_Ctrlsoc, (LPSOCKADDR)&saSrc, &addr_size);
	if (nRet == SOCKET_ERROR)
		return FALSE;
	localhost = inet_ntoa(saSrc.sin_addr);
	while (1) {
		// Turn "." into ","
		if ((i = localhost.Find('.')) == -1) break;
		localhost.SetAt(i, ',');
	}

	//Create socket listener
	sockSrvr = socket(AF_INET, SOCK_STREAM, 0);
	if (sockSrvr == INVALID_SOCKET)
	{
		return FALSE;
	}
	//Bind socket
	memset(&saSrc, 0, sizeof(sockaddr_in));
	saSrc.sin_family = AF_INET;
	saSrc.sin_port = htons(0);
	nRet = ::bind(sockSrvr, (LPSOCKADDR)&saSrc, addr_size);
	if (nRet == SOCKET_ERROR)
	{
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	//Listen
	nRet = ::listen(sockSrvr, 2);
	if (nRet == SOCKET_ERROR)
	{
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	memset(&saSrc, 0, sizeof(sockaddr_in));
	nRet = ::getsockname(sockSrvr, (LPSOCKADDR)&saSrc, &addr_size);
	if (nRet == SOCKET_ERROR) {
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	localsock = ntohs(saSrc.sin_port);
	localhost.Format(localhost + ",%d,%d", localsock / 256, localsock % 256);
	if (!Command(CString("PORT ") + localhost)) {
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	//发送列表命令
	strOut = "";
	strOut += "LIST";
	if (!Send(strOut)) {
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	if (!Response()) {
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	if (this->m_retmsg.Find('226') == -1) {
		//Receive two lines of responses
		if (!Command(CString(""))) {
			::shutdown(sockSrvr, 2);
			::closesocket(sockSrvr);
			return FALSE;
		}
	}
	else {
		this->resnum = 2;

	}


	// Create data transmission socket
	// accepct ftpsocket connection
	if (this->resnum == 4) {
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	memset(&saDest, 0, sizeof(sockaddr_in));
	datachannel = ::accept(sockSrvr, (LPSOCKADDR)&saDest, &addr_size);
	if (datachannel == INVALID_SOCKET) {
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}
	if ((0 != ioctlsocket(datachannel, FIONBIO, &lpArgument))) {
		::shutdown(datachannel, 2);
		::closesocket(datachannel);
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}



	sum = 0;
	while (1) { // Get data
		if (!(num = ::recv(datachannel, (char *)(m_buf.GetData() + sum), BUFSIZE, 0))
			|| num == SOCKET_ERROR)
			break;
		TRACE("Received :%d\n", num);
		Sleep(0);
		sum += num;
		m_buf.SetSize(sum + BUFSIZE);
	}
	::shutdown(datachannel, 2);
	::closesocket(datachannel);
	::shutdown(sockSrvr, 2);
	::closesocket(sockSrvr);
	return TRUE;
}

BOOL FTPCmd::MoveFile(CString remotefile, CString localfile, BOOL pasv, BOOL ins, BOOL get, volatile long * length, volatile long * filelength)
{
	if (ins && !get)pasv = true;
	CString localhost, temp, remotehost, outputstr;
	CString strtemp;
	UINT localsock = 0, serversock, i, j;
	CFile datafile;
	SOCKET sockSrvr;
	SOCKET datachannel;
	long LocalFileLen = 0;
	BOOL LocalFileExist = 0;
	int num = 0, numread, numsent;
	const int BUFSIZE = 4096;
	char cbuf[BUFSIZE];
	DWORD lpArgument = 0;
	struct  sockaddr_in saSrc;
	struct  sockaddr_in saDest;
	struct sockaddr_in sockAddr;
	int addr_size = 0;
	int nRet = 0;
	int count = 0, t;
	addr_size = sizeof(sockaddr_in);
	//Get file length
	//Open local file
	if (datafile.Open(localfile, CFile::modeReadWrite)) {
		LocalFileLen = datafile.GetLength();
		LocalFileExist = TRUE;
	}
	else {
		if (get == 0) {
			m_retmsg = "Failure: Cannot open local file!";
			return FALSE;
		}
		else if (!(datafile.Open(localfile, CFile::modeWrite | CFile::modeCreate))) {
			m_retmsg = "Failure: Cannot open local file!";
			return FALSE;
		}
	}

	if (!Command(CString("TYPE I"))) {
		return FALSE; // binary transmission
	}

	if (pasv) { // passive mode
		if (!Command(CString("PASV"))) {
			datafile.Close();
			return FALSE;
		}// Get the temporary ip address and port number that the server sent
		if ((i = m_retmsg.Find('(')) == -1 || (j = m_retmsg.Find(')')) == -1) {
			datafile.Close();
			return FALSE;
		}
		temp = m_retmsg.Mid(i + 1, (j - i) - 1);
		i = temp.ReverseFind(',');
		serversock = _ttol(temp.Right(temp.GetLength() - (i + 1))); //Get the port number that server sent back
		temp = temp.Left(i);
		i = temp.ReverseFind(',');
		serversock += 256 * _ttol(temp.Right(temp.GetLength() - (i + 1))); // 
		remotehost = temp.Left(i);
		while (1) { // turn "," to "."
			if ((i = remotehost.Find(',')) == -1) break;
			remotehost.SetAt(i, '.');
		}
	}
	else {
		// Send local IP address to the server
		memset(&saSrc, 0, sizeof(sockaddr_in));
		nRet = ::getsockname(m_Ctrlsoc, (LPSOCKADDR)&saSrc, &addr_size);
		if (nRet == SOCKET_ERROR)
			return FALSE;
		localhost = inet_ntoa(saSrc.sin_addr);
		while (1) { 
			if ((i = localhost.Find('.')) == -1) break;
			localhost.SetAt(i, ',');
		}
		//Create socket listener
		sockSrvr = socket(AF_INET, SOCK_STREAM, 0);
		if (sockSrvr == INVALID_SOCKET)
		{
			return FALSE;
		}
		// Bind socket
		memset(&saSrc, 0, sizeof(sockaddr_in));
		saSrc.sin_family = AF_INET;
		saSrc.sin_port = htons(0);
		nRet = ::bind(sockSrvr, (LPSOCKADDR)&saSrc, addr_size);
		if (nRet == SOCKET_ERROR)
		{
			::shutdown(sockSrvr, 2);
			::closesocket(sockSrvr);
			return FALSE;
		}
		//Listen
		nRet = ::listen(sockSrvr, 2);
		if (nRet == SOCKET_ERROR)
		{
			::shutdown(sockSrvr, 2);
			::closesocket(sockSrvr);
			return FALSE;
		}
		// Get listen port
		memset(&saSrc, 0, sizeof(sockaddr_in));
		nRet = ::getsockname(sockSrvr, (LPSOCKADDR)&saSrc, &addr_size);
		if (nRet == SOCKET_ERROR) {
			::shutdown(sockSrvr, 2);
			::closesocket(sockSrvr);
			return FALSE;
		}
		// Send port number to server
		localsock = ntohs(saSrc.sin_port);
		localhost.Format(localhost + ",%d,%d", localsock / 256, localsock % 256);
		if (!Command(CString("PORT ") + localhost)) {
			::shutdown(sockSrvr, 2);
			::closesocket(sockSrvr);
			return FALSE;
		}
	}

	// Download
	// Send "REST" command to server
	if (get) {
		if (LocalFileExist == TRUE && ins == true) {
			outputstr.Format(CString("SIZE %s"), remotefile);
			if (!Send(outputstr)) {
				datafile.Close();
				return FALSE;
			}
			if (Response() && this->resnum == 2) {
				CString len = this->m_retmsg.Right(m_retmsg.GetLength() - 4);
				outputstr.Format(CString("REST %d"), LocalFileLen);
				if (!Send(outputstr)) {
					datafile.Close();
					return FALSE;
				}
				if (!Response() || this->resnum != 3) {
					// Re-transmit the file
					datafile.Close();
					LocalFileLen = 0;
					if (!(datafile.Open(localfile, CFile::modeWrite | CFile::modeCreate))) {
						return FALSE;
					}
				}
				else if (LocalFileLen < _ttoi(len)) {
					datafile.Seek(LocalFileLen, CFile::begin);
					(*filelength) = (*filelength) - LocalFileLen;
				}
			}
		}
	}
	// Upload
	// Send "STOR" command to server
	else {
		if (ins == true) {
			outputstr.Format(CString("SIZE %s"), remotefile);
			if (!Send(outputstr)) {
				datafile.Close();
				return FALSE;
			}
			if (Response() && this->resnum == 2) {
				CString len = this->m_retmsg.Right(m_retmsg.GetLength() - 4);
				outputstr.Format(CString("REST %s"), len);
				if (!Send(outputstr)) {
					datafile.Close();
					return FALSE;
				}
				if (Response() && this->resnum == 3) {
					datafile.Seek(_ttoi(len), CFile::begin);
					(*filelength) = (*filelength) - _ttoi(len);
				}
			}
		}
		strtemp.Format(CString("STOR %s"), remotefile);
		if (!Send(strtemp)) {
			datafile.Close();
			return FALSE;
		}
	}
	if (get) {
		if (!Send(CString(get ? "RETR " : "STOR ") + remotefile)) {
			datafile.Close();
			return FALSE;
		}
	}
	if (pasv) {// passive mode
		// Create socket
		datachannel = socket(AF_INET, SOCK_STREAM, 0);
		if (datachannel == SOCKET_ERROR)
		{
			return FALSE;
		}
		sockAddr.sin_addr.S_un.S_addr = inet_addr((char *)remotehost.GetBuffer(remotehost.GetLength()));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(serversock);

		// Connection initialization
		if (0 != (connect(datachannel, (sockaddr*)&sockAddr, sizeof(sockAddr)))) {
			nRet = closesocket(datachannel);
			return FALSE;
		}
	}
	if (!Response() || resnum != 1) {
		datafile.Close();
		return FALSE; // Get server's response
	}
	if (!pasv) {
		memset(&saDest, 0, sizeof(sockaddr_in));
		datachannel = ::accept(sockSrvr, (LPSOCKADDR)&saDest, &addr_size);
		if (datachannel == INVALID_SOCKET) {
			::shutdown(sockSrvr, 2);
			::closesocket(sockSrvr);
			return FALSE;
		}
	}

	if ((0 != ioctlsocket(datachannel, FIONBIO, &lpArgument))) {
		::shutdown(datachannel, 2);
		::closesocket(datachannel);
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);
		return FALSE;
	}

	// Data transmission
	while (1) {
		if (get) {
			if (!(num = ::recv(datachannel, cbuf, BUFSIZE, 0)) || num == SOCKET_ERROR)
				break;
			else {
				datafile.Write(cbuf, num);
			}

			count += num;
			(*length) = count;
		}
		else {
			if (!(numread = datafile.Read(cbuf, BUFSIZE))) break; //EOF
			if ((numsent = ::send(datachannel, cbuf, numread, 0)) == SOCKET_ERROR)
				break;
			if (numread != numsent) datafile.Seek(numsent - numread, CFile::current);
			count += numsent;
			(*length) = count;
		}
	}
	::shutdown(datachannel, 2);
	::closesocket(datachannel);
	if (this->m_retmsg.Find('226') == -1) {
		if (!Command(CString(""))) {
			return FALSE; 
		}
	}
	return TRUE; // Succeed in transmission
}

BOOL FTPCmd::LogIn(CString hostname, int hostport, CString username, CString password, CString acct, CString sHost, CString sUsername, CString sPassword, int sPort, int logintype)
{
	int port;
	CString buf, temp;
	temp = hostname;
	port = hostport;
	if (hostport != 21) hostname.Format(hostname + ":%d", hostport); 
	if (!Open(temp, port)) return FALSE;
	if (!Command(CString(""))) return FALSE; // Get initial connection information
	temp = CString("USER ") + username;
	if (!Send(temp)) {
		return FALSE;
	}if (!Response()) {
		return FALSE;
	}
	if (resnum != 2 && resnum != 3) return FALSE;
	temp = CString("PASS ") + password;
	if (!Send(temp)) {
		return FALSE;
	}if (!Response()) {
		return FALSE;
	}
	return TRUE;
}

void FTPCmd::LogOut()
{
	Send(CString("QUIT"));
	Response();
	Close();
}
