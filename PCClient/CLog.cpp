#include "stdafx.h"
#include "Clog.h"
static CLog* l = new CLog(); 


// 전역적인 로그인스턴스를 가져온다. 
CLog* GetLogInstance()
{
    return l;
}

// 
CLog::CLog()
{
	h = CreateFile(L"\\SpyManager.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	CloseHandle(h);
}

CLog::~CLog()
{
    //CloseHandle(h);
}

void CLog::Write(TCHAR* msg)
{
	h = CreateFile(L"\\SpyManager.txt", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(h == INVALID_HANDLE_VALUE){ 
		h = CreateFile(L"\\SpyManager.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	SetFilePointer(h, 0, NULL, FILE_END);

	SYSTEMTIME tm;
	GetLocalTime(&tm);

	TCHAR szMSG[1024 *2];
	memset(szMSG, 0, sizeof(TCHAR) * 1024 * 2);
    
	wsprintf(szMSG, L"[%d-%d-%d %d:%d:%d]",
	    tm.wYear,
		tm.wMonth,
		tm.wDay,
		tm.wHour,
		tm.wMinute,
		tm.wSecond
	);
	
	_tcscat(szMSG, msg);
	_tcscat(szMSG, L"\r\n");
	
	int  nSize   = _tcslen(szMSG); 
	int  newSize = nSize * 2 + 1;
	DWORD  nWritten;
	char* pBuf   = new char[newSize];
	memset(pBuf, 0x00, newSize);

	wcstombs(pBuf, szMSG, newSize - 1 );
	WriteFile( h, pBuf, strlen(pBuf), &nWritten, NULL);

	delete [] pBuf;

	CloseHandle(h);

}

void CLog::Write(LPCWSTR msg)
{
	h = CreateFile(L"\\SpyManager.txt", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(h == INVALID_HANDLE_VALUE){ 
		h = CreateFile(L"\\SpyManager.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	SetFilePointer(h, 0, NULL, FILE_END);

	SYSTEMTIME tm;
	GetLocalTime(&tm);

	TCHAR szMSG[1024 *2];
	memset(szMSG, 0, sizeof(TCHAR) * 1024 * 2);

	wsprintf(szMSG, L"[%d-%d-%d %d:%d:%d]",
		tm.wYear,
		tm.wMonth,
		tm.wDay,
		tm.wHour,
		tm.wMinute,
		tm.wSecond
		);

	_tcscat(szMSG, msg);
	_tcscat(szMSG, L"\r\n");

	int  nSize   = _tcslen(szMSG); 
	int  newSize = nSize * 2 + 1;
	DWORD  nWritten;
	char* pBuf   = new char[newSize];
	memset(pBuf, 0x00, newSize);

	wcstombs(pBuf, szMSG, newSize - 1 );
	WriteFile( h, pBuf, strlen(pBuf), &nWritten, NULL);

	delete [] pBuf;

	CloseHandle(h);

}