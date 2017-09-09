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


// ��Ŷ �а� �м�
int CPacket::Analyze()
{
	// �ش��б� 
	int nRst = RecvBySize((char*)&m_pkt, sizeof(CPacket::PacketData) ); 
    if(nRst == -1) return -1;

	// �ν��ڰ� Ʋ���� Fail ����
    if(m_pkt.identify[0] != 'P' ||
       m_pkt.identify[1] != 'S' ||
	   m_pkt.identify[2] != 'W' 
	) return -1;
    
    // Data �б�
	int nRead   = m_pkt.length;
	pPackData = new byte[nRead];

	nRst = RecvBySize((char*)pPackData, nRead); 
    if(nRst == -1) return -1;
	
	return 0;
}

// �ش� ����
int CPacket::SetHeader(BYTE req, BYTE ack)
{
	m_pkt.identify[0] = 'P';
	m_pkt.identify[1] = 'S';
	m_pkt.identify[2] = 'W';
	
	m_pkt.encode      = 0;
	m_pkt.req         = req;

    return 0;
}

// �ش� ����
int CPacket::SetData(BYTE* pData, int nSize)
{
	pPackData = new byte[nSize];
	CopyMemory(pPackData, pData, nSize);

	m_pkt.length  = nSize;
	return 0;
}

// ��Ŷ ����� ������
int CPacket::SendPacket(int nSize, byte* pData)
{
	// Packet ������
	int bytesSent = send( m_socket, (char*)pData, nSize, 0 );
	if(SOCKET_ERROR == bytesSent){	
		return -1;
	}
    
	return 0;
}


// ��Ŷ ����� ������2
int CPacket::SendPacket()
{
	// Header Packet ������
	SendPacket(sizeof(PacketData), (BYTE*)&m_pkt );

	return SendPacket(m_pkt.length, (BYTE*)pPackData );
  
}


// Header ���� �б�
BYTE*  CPacket::GetPacketData()
{
	return pPackData;
}

// Packet ���� ��ü ��������
CPacket::PacketData*  CPacket::GetPacketInfo()
{
    return &m_pkt;
}

// ũ�⸸ŭ ���������� ���۸� �Ѵ�.
int CPacket::RecvBySize(char* pData, int nSize)
{
	int nReadSize = 0;
	int nReserved = nSize;
	
    while(1){
		
		// ������ �̺�Ʈ ó����
		int  nRead = recv(m_socket, pData + nReadSize, nReserved, 0);

		nReadSize += nRead;
		nReserved -= nRead;

		if(nReadSize == nSize) return 0;
		if(nRead     == -1   ) { return nRead;}
		if(nReadSize == 0    ) return 0;
	}


	return nReadSize;
}