#include <iostream>
#include <QThread>
#include "MyTcpListener.h"

using namespace std;




int main()
{
    // 1. Create listener object
    CListenerImpl tcp_listener;
    // 2. Create component object (and binding with listener object)
    CTcpPullServerPtr tcp_pServer(&tcp_listener);

    // 3. Start component object
    if(!tcp_pServer->Start((LPCTSTR)"0.0.0.0", 8088))
    {
        exit(1);
    }

    std::cout << "TCP server start succeed!  tony2278 - loveu1061@126.com"<<std::endl;
    std::cout << "q or Q for quit."<<std::endl;

    /* wait for exit */
    while(1)
    {
        char c = (char)getchar();
        if( c == 27 || c == 'q' || c == 'Q' )
        {
            break;
        }
        else
        {
            QThread::msleep(500);
        }
    }

    // 6. (optional) Stop component object
    tcp_pServer->Stop();

    return 0;

    // 7. Destroy component object automatically
    // 8. Destroy listener object automatically
}








