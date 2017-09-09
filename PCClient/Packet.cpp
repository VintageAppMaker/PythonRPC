#include "stdafx.h"
#include "Packet.h"

CPacket::CPacket(SOCKET s)
{
    m_socket = s;
	ZeroMemory(&m_pkt, sizeof(PacketData) );
}

CPacket::~CPacket()
{
   
}


// 패킷 읽고 분석
int CPacket::Analyze()
{
	// 해더읽기 
	int nRst = RecvBySize((char*)&m_pkt, sizeof(CPacket::PacketData) ); 
    if(nRst == -1) return -1;

	// 인식자가 틀리면 Fail 리턴
    if(m_pkt.identify[0] != 'P' ||
       m_pkt.identify[1] != 'S' ||
	   m_pkt.identify[2] != 'W' 
	) return -1;
    
    // Data 읽기
	int nRead   = m_pkt.length;
	pPackData = new byte[nRead];

	nRst = RecvBySize((char*)pPackData, nRead); 
    if(nRst == -1) return -1;
	
	return 0;
}

// 해더 세팅
int CPacket::SetHeader(BYTE req, BYTE ack)
{
	m_pkt.identify[0] = 'P';
	m_pkt.identify[1] = 'S';
	m_pkt.identify[2] = 'W';
	
	m_pkt.encode      = 0;
	m_pkt.req         = req;

    return 0;
}

// 해더 세팅
int CPacket::SetData(BYTE* pData, int nSize)
{
	pPackData = new byte[nSize];
	CopyMemory(pPackData, pData, nSize);

	m_pkt.length  = nSize;
	return 0;
}

// 패킷 만들어 보내기
int CPacket::SendPacket(int nSize, byte* pData)
{
	// Packet 보내기
	int bytesSent = send( m_socket, (char*)pData, nSize, 0 );
	if(SOCKET_ERROR == bytesSent){	
		return -1;
	}
    
	return 0;
}


// 패킷 만들어 보내기2
int CPacket::SendPacket()
{
	// Header Packet 보내기
	SendPacket(sizeof(PacketData), (BYTE*)&m_pkt );

	return SendPacket(m_pkt.length, (BYTE*)pPackData );
  
}


// Header 영역 읽기
BYTE*  CPacket::GetPacketData()
{
	return pPackData;
}

// Packet 정보 전체 가져오기
CPacket::PacketData*  CPacket::GetPacketInfo()
{
    return &m_pkt;
}

// 크기만큼 읽을때까지 버퍼링 한다.
int CPacket::RecvBySize(char* pData, int nSize)
{
	int nReadSize = 0;
	int nReserved = nSize;
	
    while(1){
		
		// 윈도우 이벤트 처리용
		int  nRead = recv(m_socket, pData + nReadSize, nReserved, 0);

		nReadSize += nRead;
		nReserved -= nRead;

		if(nReadSize == nSize) return 0;
		if(nRead     == -1   ) { return nRead;}
		if(nReadSize == 0    ) return 0;
	}


	return nReadSize;
}