/***************************************************************************
	����: packet Class
	�ۼ���: �ڼ���(adsloader@naver.com)
	�ۼ���: 2009.3.3
	����  : ��Ŷ ���� �� �м� Ŭ���� 
	����  : Fixed Size
***************************************************************************/

#ifndef __PSW_PACKET__
#define __PSW_PACKET__

#include "stdafx.h"
#include <winsock.h>

class CPacket
{
public:

	// Ŀ�ǵ� ���� 
	// Ŀ�ǵ� ���� 
	enum reqCMD{
		PKT_SHELL  = 0,
		PKT_PYTHON    
	};
	

public:

	#pragma pack(push, 1)
	// Header ����
 	typedef struct tagPacketHeader{
		CHAR   identify[3];  // �ν��� PSW
		int    length;       // ������ ũ�� 
		BYTE   encode;       // Ŭ���̾�Ʈ ���ڿ� ���ڵ� ����(���� ����)  
		BYTE   req;          // 0:shell, 1:python

		   		
	} PacketData;
	#pragma pack(pop)

public:
	CPacket(SOCKET s);
	virtual ~CPacket();
    
	int Analyze();
	int SetHeader( BYTE req, BYTE ack);
	int SendPacket(int nSize, byte* pData);
    int SetData(byte* pData, int nSize);

	int SendPacket();
    
	BYTE*        GetPacketData();
	PacketData*  GetPacketInfo();

	// Data����
	BYTE*   pPackData;
	
private:
	SOCKET m_socket;

	PacketData m_pkt;
    
	// nSize���������� ��� ���۸� �� ���Ѵ�.
	int RecvBySize(char* pData, int nSize);
};

#endif
