/***************************************************************************
	제목: packet Class
	작성자: 박성완(adsloader@naver.com)
	작성일: 2009.3.3
	목적  : 패킷 전송 및 분석 클래스 
	참고  : Fixed Size
***************************************************************************/

#ifndef __PSW_PACKET__
#define __PSW_PACKET__

#include "stdafx.h"
#include <winsock.h>

class CPacket
{
public:

	// 커맨드 정의 
	// 커맨드 정의 
	enum reqCMD{
		PKT_SHELL  = 0,
		PKT_PYTHON    
	};
	

public:

	#pragma pack(push, 1)
	// Header 정보
 	typedef struct tagPacketHeader{
		CHAR   identify[3];  // 인식자 PSW
		int    length;       // 데이터 크기 
		BYTE   encode;       // 클라이언트 문자열 인코딩 종류(아직 미정)  
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

	// Data영역
	BYTE*   pPackData;
	
private:
	SOCKET m_socket;

	PacketData m_pkt;
    
	// nSize읽을때까지 계속 버퍼링 및 블럭한다.
	int RecvBySize(char* pData, int nSize);
};

#endif
