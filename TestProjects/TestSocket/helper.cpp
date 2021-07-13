//#include "stdafx.h"
#include "helper.h"
#include <iostream>
#include <string>

#include <ws2tcpip.h>

#if !defined(stscanf_s)
	#ifdef _UNICODE
		#define stscanf_s	swscanf_s
	#else
		#define stscanf_s	sscanf_s
	#endif
#endif



info_msg* info_msg::Construct(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent, LPCTSTR lpszName)
{
	return new info_msg(dwConnID, lpszEvent, iContentLength, lpszContent, lpszName);
}

void info_msg::Destruct(info_msg* pMsg)
{
	delete pMsg;
}

info_msg::info_msg(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent, LPCTSTR lpszName): connID(dwConnID), evt(lpszEvent), contentLength(iContentLength), content(lpszContent), name(nullptr)
{
	if(lpszName)
	{
		int len = lstrlen(lpszName);

		if(len > 0)
		{
			name = new TCHAR[len + 1];
			memcpy((LPSTR)name, lpszName, (len + 1) * sizeof(TCHAR));
		}
	}
}

info_msg::~info_msg()
{
	if(name)
		delete[] name;

	if(contentLength > 0)
		delete[] content;
}

void PostOnSend(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
    //wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_SEND", content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnSendTo(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
    //wsprintf(lpszContent, _T("<%s#%d> (%d bytes)"), lpszAddress, usPort, iLength);
	int content_len = lstrlen(lpszContent);
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_SEND", content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnReceive(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
    //wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_RECEIVE", iLength, (LPTSTR)pData, lpszName);

    PostInfoMsg(msg);
}

void PostOnReceiveFrom(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
}

void PostOnReceiveCast(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
}

void PostOnClose(CONNID dwConnID, LPCTSTR lpszName)
{
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_CLOSE", 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnError(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszName)
{
    //LPTSTR lpszContent = new TCHAR[100];
    //wsprintf(lpszContent, _T("OP: %d, CODE: %d"), enOperation, iErrorCode);
    //int content_len = lstrlen(lpszContent);
}

void PostOnError2(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pBuffer, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[150];
    //wsprintf(lpszContent, _T("<%s#%d> OP: %d, CODE: %d (DATA: 0x%X, LEN: %d>"), lpszAddress, usPort, enOperation, iErrorCode, pBuffer, iLength);
	int content_len = lstrlen(lpszContent);
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_ERROR", content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnAccept(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, BOOL bPass, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
    //wsprintf(lpszContent, _T("%s (%s#%d)"), bPass ? _T("PASS") : _T("REJECT"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_ACCEPT", content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnAccept2(CONNID dwConnID, LPCTSTR lpszName)
{
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_ACCEPT", 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnHandShake(CONNID dwConnID, LPCTSTR lpszName)
{
    info_msg* msg = info_msg::Construct(dwConnID, (LPCTSTR)"EVT_ON_HAND_SHAKE", 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnPrepareListen(LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
}

void PostOnPrepareConnect(CONNID dwConnID, LPCTSTR lpszName)
{
}

void PostOnConnect(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
}

void PostOnConnect2(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
}

void PostOnConnect3(CONNID dwConnID, LPCTSTR lpszName)
{
}

void PostOnShutdown(LPCTSTR lpszName)
{
    info_msg* msg = info_msg::Construct(0, (LPCTSTR)"EVT_ON_SHUTDOWN", 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostInfoMsg(info_msg* msg)
{
    //PostMessage(USER_INFO_MSG, (WPARAM)msg);
    //info_msg::Destruct(msg);
    char buf[1024] = {0};
    memcpy(buf, msg->content, msg->contentLength);
    std::cout<<std::string(buf)<<std::endl;
}

CBufferPtr* GeneratePkgBuffer(DWORD seq, LPCTSTR lpszName, short age, LPCTSTR lpszDesc)
{
    //LPCSTR name = T2A((LPTSTR)lpszName);
    //LPCSTR desc = T2A((LPTSTR)lpszDesc);
    LPCSTR name = "";
    LPCSTR desc = "";
	int desc_len = (int)strlen(desc) + 1;
	int body_len = offsetof(TPkgBody, desc) + desc_len;

	TPkgBody* pBody = (TPkgBody*)_alloca(body_len);
	memset(pBody, 0, body_len);

	pBody->age = age;
	strcpy(pBody->name, name);
	strcpy(pBody->desc, desc);

	TPkgHeader header;
	header.seq = seq;
	header.body_len = body_len;

	return GeneratePkgBuffer(header, *pBody);
}

CBufferPtr* GeneratePkgBuffer(const TPkgHeader& header, const TPkgBody& body)
{
	int header_len	= sizeof(TPkgHeader);
	int body_len	= header.body_len;
	
	CBufferPtr* pBuffer = new CBufferPtr(header_len + body_len);

	memcpy(pBuffer->Ptr(), (BYTE*)&header, header_len);
	memcpy(pBuffer->Ptr() + header_len, (BYTE*)&body, body_len);

	return pBuffer;
}












