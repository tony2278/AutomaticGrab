#-------------------------------------------------
#
# Project created by QtCreator 2021-06-19T12:12:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoHand
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    formabout.cpp \
        mainwindow.cpp \
    camera_realsense.cpp \
    camera_thread.cpp \
    mars.cpp

HEADERS  += mainwindow.h \
    camera_realsense.h \
    camera_thread.h \
    common1.h \
    formabout.h \
    mars.h

FORMS    += mainwindow.ui \
    formabout.ui


QMAKE_CFLAGS_DEBUG += -MDd
QMAKE_CXXFLAGS_DEBUG += -MDd
QMAKE_CFLAGS_RELEASE += -MD
QMAKE_CXXFLAGS_RELEASE += -MD

#-------Halcon Config
INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/include/halconcpp

#-------OpenCV Config
INCLUDEPATH += $$PWD/opencv/build/include
INCLUDEPATH += $$PWD/opencv/build/include/opencv2

#-------PCL Config
INCLUDEPATH += $$PWD/include_PCL/PCL-1.9.1/3rdParty/Boost/include/boost-1_68
INCLUDEPATH += $$PWD/include_PCL/PCL-1.9.1/3rdParty/Eigen/eigen3
INCLUDEPATH += $$PWD/include_PCL/PCL-1.9.1/3rdParty/FLANN/include
INCLUDEPATH += $$PWD/include_PCL/PCL-1.9.1/3rdParty/Qhull/include
INCLUDEPATH += $$PWD/include_PCL/PCL-1.9.1/3rdParty/VTK/include
INCLUDEPATH += $$PWD/include_PCL/PCL-1.9.1/include/pcl-1.9
INCLUDEPATH += $$PWD/include_PCL/OpenNI2/Include

#-------RealSense Config
INCLUDEPATH += $$PWD/include_RealSense/Intel-RealSense-SDK-2.0/include
INCLUDEPATH += $$PWD/include_RealSense/Intel-RealSense-SDK-2.0/samples

Debug:
{
#-------OpenCV Config Lib
LIBS += $$PWD/lib_OpenCV/OpenCV4.5.0_Release/lib/opencv_world452d.lib
}
Release:
{
LIBS += $$PWD/lib_OpenCV/OpenCV4.5.0_Release/lib/opencv_world452.lib
}
#-------PCL Config Lib
#LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/Boost/lib/*.lib
#LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/Eigen/eigen3/*.lib
#LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/FLANN/lib/*.lib
#LIBS += $$PWD/lib_PCL/PCL-1.9.1/lib/*.lib
#LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/Qhull/lib/*.lib
#LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/VTK/lib/*.lib
#LIBS += $$PWD/lib_PCL/OpenNI2/Lib/*.lib
#LIBS += $$PWD/lib_PCL/debug/lib/gd.lib

#-------RealSense Config Lib
#LIBS += $$PWD/lib_RealSense/Intel-RealSense-SDK-2.0/lib/x64/realsense2.lib

#}
#Release:
#{
#-------Halcon Config Lib
LIBS += $$PWD/lib/x64-win64/halconcpp.lib
LIBS += $$PWD/lib/x64-win64/halcon.lib

#-------PCL Config Lib
LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/Boost/lib/libboost_system-vc141-mt-gd-x64-1_68.lib
LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/Boost/lib/*.lib
LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/Eigen/eigen3/*.lib
LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/FLANN/lib/*.lib
LIBS += $$PWD/lib_PCL/PCL-1.9.1/lib/*.lib
LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/Qhull/lib/*.lib
LIBS += $$PWD/lib_PCL/PCL-1.9.1/3rdParty/VTK/lib/*.lib
LIBS += $$PWD/lib_PCL/OpenNI2/Lib/*.lib
LIBS += $$PWD/lib_PCL/release/lib/gd.lib

#-------RealSense Config Lib
LIBS += $$PWD/lib_RealSense/Intel-RealSense-SDK-2.0/lib/x64/realsense2.lib
#}







