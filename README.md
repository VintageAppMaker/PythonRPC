# PythonRPC

[원본블로그](http://blog.naver.com/adsloader/50136799758)

### 목적

보편화된 서버개발 순서는 일반적으로 다음과 같다. 
 
  1. 패킷정의
  2. flow 작성
  2. 서버 적용, 클라이언트 적용
  3. 테스트

이러다보니 특정기능(패킷의 프로토콜 추가) 하나만 추가되어도 서버와 클라이언트에서 해야할 것들이 무척 성가시다. 그래서 최근에는 RPC(Remote Process Call)가 나오게 되었다. 
RPC(Remote Process Call)는 클라이언트에서는 함수를 호출하고 서버에서는 함수를 구현하여 통신하는 구조로 XML-RPC나 SOAP이 이에 속한다. RPC는 서버와 클라이언트 개발자가 프로토콜에 신경쓸 일이 없어져서 무척 편리하다. 그러나 단점으로는 생각보다 빠르지도 않고 설치에 적잖은 노력을 해야 한다.
python의 경우, 우분투에 빌트인 된 프로그램이다. 그리고 강력한 서드파티 엔진들이 많다. 아래 내용들은 python을 이용하여 다음과 같은 것들을 구현했다.

패킷 수정없이 서버에 신기능 추가
클라이언트에서 서버에 있는 리눅스 쉘과 파이썬 명령어를 리모트로 호출(RPC)

간단히 말해서 "Python으로 RPC(Remote Process Call)을 쉽게 따라해본다"가 목적이다.

### 통신구조

클라이언트에서 패킷(해더 + 문자열)에다 python 또는 Linux shell 커맨드인지 지정을 하고 서버의 함수나 명령어를 요청하면 python 서버에서는 그 내용을 분석하고 요청한 기능을 수행한 후, 결과값을 패킷(해더 + 문자열)로 전송한다.

![원본블로그](http://postfiles12.naver.net/20120317_139/adsloader_1331992174889d6OiT_JPEG/3.PNG?type=w2)

### 해더

~~~C

#pragma pack(push, 1)
      // Header 정보
      typedef struct tagPacketHeader{
          CHAR   identify[3];  // 인식자 PSW
          int    length;       // 데이터 크기 
          BYTE   encode;       // 클라이언트 문자열 인코딩 종류(아직 미정)  
          BYTE   req;          // 0:shell, 1:python
     } PacketData;
  #pragma pack(pop)

~~~

문자열 인코딩은 ubuntu는 "utf-8"이며 윈도우의 경우 "cp949" 이다.

### ubuntu server

~~~python

# -*- coding: utf-8 -*-
#  작성자: 박 성완(adsloader@naver.com)
#  목적  : 리눅스용 편리한 서버 사용  
#  작성일: 2012.03.17  

import socket
import threading
import SocketServer
import os
import struct
import binascii
import array
import base64

# packet 처리용 클래스 
class Packet:
    
    # 생성자: 변수 초기화용 
    def __init__(self):

        self.m_pData     = []    # Data 값           
        self.m_HDRSize   = 9     # packet Header Size 
        
        #packet 관련 정의
        self.struct   = ' 3B l B B'
        self.identify = []
        
    # data 읽기함수     
    def ReadData(self, pRead):
        tmp = []
        try:
            s   = struct.Struct("<" + self.struct)
            tmp = s.unpack(pRead)
            print "unpack hdr:" ,  tmp
 
            # 필드 자르기 
            self.identify = tmp[0:3]
            self.length   = tmp[3]
            self.encode   = tmp[4]
            self.req      = tmp[5]
        
        except:
            print "Structure Format Error!!"
            return 
        
        
    # data 쓰기함수     
    def WriteData(self, clientID, req, pWrite):
        try:
            
            rst =[]
            
            tmp = []
            tmp += [ord('P'), ord('S'), ord('W')]
            tmp.append(clientID)
            tmp.append(req)
            tmp.append(0)
            
            tmp += pWrite
            sFormat =  "<" + self.struct + " %dB" %  len(pWrite)
            print sFormat
            s = struct.Struct(sFormat)
            rst = s.pack(*tmp)
                        
            return rst
            
        except:
            print "Structure Format Error!!"
            return 


# 클라이언트에서 호출할 함수
def TestFunc(str):
    return "Test Func called with (%s)" % str

class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
    # command를 실행하기  
    def doShellCommand(self, sCmd):
        pingaling = os.popen(u"%s"% sCmd)
        sline = ""
             
        while 1:
            line = pingaling.readline()
            sline += line
            if not line: break

        return sline 

    # python command를 실행하기  
    def doPythonCommand(self, sCmd):
        result = ""
        exec(sCmd)
        print result
        return result  

    def MakeSendCommand(self,p , sMsg ):
        return p.WriteData(len(sMsg) + 1, 3, self.MakeString(sMsg))

    def MakeString(self, sMsg):
        # data영역 만들기 
        Data = []
        #sData = bytes(sMsg) 
        sData = sMsg 
        for a in sData:
            Data.append ( ord(a) )
        Data.append(0)
        return Data

    def handle(self):
         
        data    = []
        rcvdata = []
       
        # function table 설정  
        CMD = {0:self.doShellCommand,1: self.doPythonCommand}        

        p = Packet()
        nReadSize = p.m_HDRSize
        MaxSize   = nReadSize 
        bHeader   = 0
        
        while True:
            rcvdata  = self.request.recv(nReadSize)
          
            # 소켓종료
            if rcvdata == "":
                break
            
            #  버퍼링  
            if len(data) == 0 :
                data = rcvdata
            else:
                data += rcvdata
            
            # 패킷크기만큼 대기 한다. 
            if len(data) < MaxSize:
                continue
            
            # Header분석 
            if bHeader  == 0: 
                p.ReadData(data)
                nReadSize = p.length                 
                MaxSize   = p.m_HDRSize + nReadSize
                bHeader   = 1
                continue 

            sData = unicode(data[9:-1],"cp949")
            print sData

            # 펑션테이블을 dictionary로 제어하다. 
            sline = CMD.get(p.req, self.doShellCommand )(sData)
            wData = self.MakeSendCommand(p, sline )
            self.request.send(wData) 
            
            data = []
        
    def finish(self):
        try:
            print "closed"
        
        except:
            print "unknown error"
        
class ThreadedTCPServer(SocketServer.TCPServer):
   pass 

if __name__ == "__main__":
    HOST, PORT = "", 9080

    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
    ip, port = server.server_address

    # Start a thread with the server -- that thread will then start one
    # more thread for each request
    server_thread = threading.Thread(target=server.serve_forever)

    # Exit the server thread when the main thread terminates
    
    # False일 경우는 프로그램 종료안하고 데몬이 실행됨.
    server_thread.setDaemon(False)
    server_thread.start()
    
~~~

### 실행하기 


1. 우분투에서 ububtu_server.py를 실행한다.
2. 윈도우에서 클라이언트 소스를 vs2008에서 컴파일하여 실행한다.
3. 서버에서 클라이언트의 접속을 처리한다.

![](http://postfiles4.naver.net/20120317_275/adsloader_1331988102442g7dyt_PNG/2.PNG?type=w2)

4. 클라이언트에서  result = TestFunc('메롱')  을 파이썬 명령어로 요청한다.
5. 클라이언트에서  ps  | grep 'python' && ls -al 을 리눅스 쉘 명령어로 요청한다.

![](http://postfiles13.naver.net/20120317_76/adsloader_133198810209475JOu_PNG/1.PNG?type=w2)



