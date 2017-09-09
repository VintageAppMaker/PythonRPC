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
