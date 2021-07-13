#ifndef TT1HELPER_H
#define TT1HELPER_H

#include "../../include_HPSocket/HPSocket/SocketInterface.h"
#include "Common/BufferPtr.h"


enum EnAppState
{
    ST_STARTING, ST_STARTED, ST_CONNECTING, ST_CONNECTED1, ST_STOPPING, ST_STOPPED
};
struct info_msg
{
	LPCTSTR name;
	CONNID connID;
	LPCTSTR evt;
	int contentLength;
	LPCTSTR content;

	static info_msg* Construct(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength = 0, LPCTSTR lpszContent = nullptr, LPCTSTR lpszName = nullptr);
	static void Destruct(info_msg* pMsg);

private:
	info_msg(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength = 0, LPCTSTR lpszContent = nullptr, LPCTSTR lpszName = nullptr);
	~info_msg();
};

struct TPkgHeader 
{
	DWORD seq;
	int body_len;
};

struct TPkgBody 
{
	char name[30];
	short age;
	char desc[1];
};

struct TPkgInfo
{
	bool is_header;
	int length;

	TPkgInfo(bool header = true, int len = sizeof(TPkgHeader)) : is_header(header), length(len) {}
	void Reset() {is_header = true, length = sizeof(TPkgHeader);}
	~TPkgInfo() {}
};



CBufferPtr* GeneratePkgBuffer(DWORD seq, LPCTSTR lpszName, short age, LPCTSTR lpszDesc);
CBufferPtr* GeneratePkgBuffer(const TPkgHeader& header, const TPkgBody& body);

void PostOnSend(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnSendTo(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnReceive(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnReceiveFrom(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnReceiveCast(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName = nullptr);
void PostOnClose(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnError(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszName = nullptr);
void PostOnError2(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pBuffer, int iLength, LPCTSTR lpszName = nullptr);
void PostOnAccept(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, BOOL bPass, LPCTSTR lpszName = nullptr);
void PostOnAccept2(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnHandShake(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnPrepareListen(LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void PostOnPrepareConnect(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnConnect(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void PostOnConnect2(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName = nullptr);
void PostOnConnect3(CONNID dwConnID, LPCTSTR lpszName = nullptr);
void PostOnShutdown(LPCTSTR lpszName = nullptr);

void PostInfoMsg(info_msg* msg);


#endif

