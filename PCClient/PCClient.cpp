
#include "stdafx.h"
#include "Packet.h"

char* UTF8ToANSI(const char *pszCode)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     nLength;

	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, bstrWide, nLength);

	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];

	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);

	return pszAnsi;
}

// 이벤트 전송 - 소켓접속 및 전송 일괄처리
int SendEvent(int nEvnet, int nSize, BYTE* pData)
{
	//	int nInt = sizeof(CPacket::PacketData);

	SOCKET sock;
	SOCKADDR_IN dest_sin;
	WORD wPort = 9080;
	int rc;

	// Create socket
	sock = socket( AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		return INVALID_SOCKET;
	}

	// Set up IP address to access
	memset (&dest_sin, 0, sizeof (dest_sin));
	dest_sin.sin_family = AF_INET;
	//dest_sin.sin_addr.S_un.S_addr = inet_addr ("10.0.2.15");
	dest_sin.sin_addr.S_un.S_addr = inet_addr ("192.168.0.17");
	dest_sin.sin_port = htons(wPort);

	// Connect to the device
	rc = connect( sock, (PSOCKADDR) &dest_sin, sizeof( dest_sin));
	if (rc == SOCKET_ERROR) {
		closesocket( sock );
		return INVALID_SOCKET;
	}

	// 값 저장, 패킷전송, 패킷수신
	CPacket pkt(sock);
	pkt.SetHeader(nEvnet, 0);

	// 데이터가 있다면 복사한다.
	if( nSize > 0){
		pkt.SetData(pData, nSize);
	}
	
	pkt.SendPacket();
	pkt.Analyze();

	// 읽은 정보 출력하기 
	char* szMsg = (char *)pkt.pPackData;
	printf ("recived(%d):%s\r\n", strlen(szMsg), UTF8ToANSI(szMsg));
    
	CPacket::PacketData* pInfo =  pkt.GetPacketInfo();
	closesocket (sock);
	
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	if ( ( WSAStartup(0x101,&wsaData) ) != 0) {
		WSACleanup();
		return -1;
	}

	char* szMsg  = "result = TestFunc('메롱')";
	SendEvent(CPacket::PKT_PYTHON, strlen(szMsg)+1, (BYTE*)szMsg);
	
	char* szMsg2 = "ps  | grep 'python' && ls -al";
	SendEvent(CPacket::PKT_SHELL, strlen(szMsg2)+1, (BYTE*)szMsg2);
	
	return 0;
}



