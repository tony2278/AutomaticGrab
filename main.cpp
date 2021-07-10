#include "mainwindow.h"
#include <QApplication>

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <DbgHelp.h>

/*
 * MiniDump/MiniDump.cpp

 * https://github.com/zhaoyangma/MiniDump/blob/master/MiniDump.cpp
 *
 */
// ����Dump�ļ�
void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,EXCEPTION_POINTERS* pException)
{
    // ����Dump�ļ�
    HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    // Dump��Ϣ
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;
    // д��Dump�ļ�����
    MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hDumpFile,MiniDumpNormal,&dumpInfo,NULL,NULL);
    // �ر��ļ����
    CloseHandle(hDumpFile);
}

// ����Unhandled Exception�Ļص�����
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
    // ���ﵯ��һ������Ի����˳�����
    //FatalAppExit(-1,_T("*** Unhandled Exception!***"));
    // ����Dump�ļ�
    CreateDumpFile(_T("1.dmp"),pException);

    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ���ô���Unhandled Exception�Ļص�����
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

    MainWindow w;
    w.show();

    return a.exec();
}
