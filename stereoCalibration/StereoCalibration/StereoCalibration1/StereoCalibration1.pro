TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        readme.cpp \
        stereo_calib.cpp \
        stereo_match.cpp



#-------OpenCV Config
INCLUDEPATH += $$PWD/../../../include_OpenCV/include
INCLUDEPATH += $$PWD/../../../include_OpenCV/include/opencv2


Debug:
{
#-------OpenCV Config Lib
LIBS += $$PWD/../../../lib_OpenCV/OpenCV4.5.0_Release/lib/opencv_world452d.lib
}
Release:
{
#-------OpenCV Config Lib
LIBS += $$PWD/../../../lib_OpenCV/OpenCV4.5.0_Release/lib/opencv_world452.lib
}





