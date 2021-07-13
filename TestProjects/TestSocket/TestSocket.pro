
TEMPLATE = app

CONFIG += core console c++11

TARGET = TestSocket

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CFLAGS_DEBUG += -MDd
QMAKE_CXXFLAGS_DEBUG += -MDd
QMAKE_CFLAGS_RELEASE += -MD
QMAKE_CXXFLAGS_RELEASE += -MD

HEADERS += \
    CListenerImpl.h \
    MyTcpListener.h \
    helper.h


SOURCES += \
        CListenerImpl.cpp \
        MyTcpListener.cpp \
        helper.cpp \
        main.cpp

#-------Socket
INCLUDEPATH += $$PWD/../../include_HPSocket

Debug:
{
    #-------Socket lib
    LIBS += $$PWD/../../lib_HPSocket/x64/HPSocket_D.lib
}
Release:
{
    #-------Socket lib
    LIBS += $$PWD/../../lib_HPSocket/x64/HPSocket.lib
}




