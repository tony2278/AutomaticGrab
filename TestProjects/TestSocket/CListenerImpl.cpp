#include "CListenerImpl.h"
#include <iostream>
#include <string>
#include <vector>

EnHandleResult CListenerImpl::OnPrepareListen(ITcpServer *pSender, SOCKET soListen)
{
    std::cout<<"OnPrepareListen"<<std::endl;
    TCHAR szAddress[50];
    int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
    USHORT usPort;

    pSender->GetListenAddress(szAddress, iAddressLen, usPort);
    PostOnPrepareListen(szAddress, usPort);
    return HR_OK;
}

EnHandleResult CListenerImpl::OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient)
{
    std::cout<<"OnAccept"<<std::endl;
    BOOL bPass = TRUE;
    TCHAR szAddress[50];
    int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
    USHORT usPort;

    pSender->GetRemoteAddress(dwConnID, szAddress, iAddressLen, usPort);

    /*CString m_strAddress;
    if(!m_strAddress.IsEmpty())
    {
        if(m_strAddress.CompareNoCase(szAddress) == 0)
            bPass = FALSE;
    }*/

    PostOnAccept(dwConnID, szAddress, usPort, bPass);

    if(bPass) pSender->SetConnectionExtra(dwConnID, new TPkgInfo(true, sizeof(TPkgHeader)));

    return bPass ? HR_OK : HR_ERROR;
}

EnHandleResult CListenerImpl::OnHandShake(ITcpServer *pSender, CONNID dwConnID)
{
    std::cout<<"OnHandShake"<<std::endl;
    return HR_OK;
}

/*
EnHandleResult CListenerImpl::OnReceive(ITcpServer *pSender, CONNID dwConnID, int iLength)
{
    std::cout<<"OnReceive1 "<<std::endl;

    TPkgInfo* pInfo			= FindPkgInfo(pSender, dwConnID);
    ITcpPullServer* pServer	= ITcpPullServer::FromS(pSender);

    std::string recv;
    if(pInfo != nullptr)
    {
        int required = pInfo->length;
        int remain = iLength;

        while(remain >= required)
        {
            remain -= required;
            CBufferPtr buffer(required);

            EnFetchResult result = pServer->Fetch(dwConnID, buffer, (int)buffer.Size());
            if(result == FR_OK)
            {
                if(pInfo->is_header)
                {
                    TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
                    //TRACE("[Server] head -> seq: %d, body_len: %d\n", pHeader->seq, pHeader->body_len);
                    required = pHeader->body_len;
                }
                else
                {
                    TPkgBody* pBody = (TPkgBody*)(BYTE*)buffer;
                    //TRACE("[Server] body -> name: %s, age: %d, desc: %s\n", pBody->name, pBody->age, pBody->desc);
                    required = sizeof(TPkgHeader);
                }

                pInfo->is_header = !pInfo->is_header;
                pInfo->length	 = required;

                PostOnReceive(dwConnID, buffer, (int)buffer.Size());
                if(!pSender->Send(dwConnID, buffer, (int)buffer.Size()))
                {
                    //std::cout<<recv<<std::endl;
                    std::cout<<(char *)buffer.Ptr()<<std::endl;
                    return HR_ERROR;
                }
            }
        }
    }

    //std::cout<<recv<<std::endl;
    return HR_OK;
}
*/

//https://github.com/laojiang19/HP-Socket-sample/blob/master/tcp_pull/serv.cpp
EnHandleResult CListenerImpl::OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)
{
    std::vector<BYTE> vec;

    ITcpPullServer* pServer = ITcpPullServer::FromS(pSender);
    // package size 16
    int required = 16;
    int remain = iLength;

    while(remain > 0)
    {
        BYTE buf[16];
        if (required > remain) {
            required = remain;
        }
        EnFetchResult result = pServer->Fetch(dwConnID, buf, required);
        if(result == FR_OK)
        {
            vec.insert(vec.end(), buf, buf+required);
            remain -= required;
        }
        else if (result == FR_DATA_NOT_FOUND)
        {
            //TODO
            //handle err
            return HR_ERROR;
        }
    }

    std::string res(vec.begin(), vec.end());
    std::cout<< res << std::endl;
    return HR_OK;
}

EnHandleResult CListenerImpl::OnReceive(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength)
{
    std::cout<<"OnReceive1 "<<std::endl;

    ITcpPullServer* pServer	= ITcpPullServer::FromS(pSender);
    TPkgInfo* pInfo			= FindPkgInfo(pSender, dwConnID);

    if(pInfo != nullptr)
    {
        int required = pInfo->length;
        CBufferPtr buffer(required);

        EnFetchResult result = pServer->Fetch(dwConnID, buffer, (int)buffer.Size());
        if(result == FR_OK)
        {
            if(pInfo->is_header)
            {
                TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
                //TRACE("[Server] head -> seq: %d, body_len: %d\n", pHeader->seq, pHeader->body_len);
                required = pHeader->body_len;
            }
            else
            {
                TPkgBody* pBody = (TPkgBody*)(BYTE*)buffer;
                //TRACE("[Server] body -> name: %s, age: %d, desc: %s\n", pBody->name, pBody->age, pBody->desc);
                required = sizeof(TPkgHeader);
            }

            pInfo->is_header = !pInfo->is_header;
            pInfo->length	 = required;

            PostOnReceive(dwConnID, buffer, (int)buffer.Size());
        }
    }


    return HR_OK;
}

EnHandleResult CListenerImpl::OnSend(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength)
{
    std::cout<<"OnSend: "<<pData<<std::endl;
    PostOnSend(dwConnID, pData, iLength);
    return HR_OK;
}

EnHandleResult CListenerImpl::OnClose(ITcpServer *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
    std::cout<<"OnClose"<<std::endl;
    iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
    PostOnError(dwConnID, enOperation, iErrorCode);

    RemovePkgInfo(pSender, dwConnID);
    return HR_OK;
}

EnHandleResult CListenerImpl::OnShutdown(ITcpServer *pSender)
{
    std::cout<<"OnShutdown"<<std::endl;
    PostOnShutdown();
    return HR_OK;
}


TPkgInfo* CListenerImpl::FindPkgInfo(ITcpServer* pSender, CONNID dwConnID)
{
    PVOID pInfo = nullptr;

    pSender->GetConnectionExtra(dwConnID, &pInfo);

    return (TPkgInfo*)pInfo;
}

void CListenerImpl::RemovePkgInfo(ITcpServer* pSender, CONNID dwConnID)
{
    TPkgInfo* pInfo = FindPkgInfo(pSender, dwConnID);
    if(pInfo != nullptr)
        delete pInfo;
}


EnFetchResult IPullClientImpl::Fetch(unsigned char *pData, int iLength)
{
    std::string str = (char *)pData;
    recv = str;
    return FR_OK;
}
