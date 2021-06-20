#include "camera_thread.h"


Camera_Thread::Camera_Thread(QObject *parent)
{

}

Camera_Thread::~Camera_Thread()
{
    emit isDone();  //∑¢ÀÕExit–≈∫≈
}

void Camera_Thread::run()
{
    m_CameraRS.Start();
}

void Camera_Thread::stop()
{
    m_CameraRS.Stop();
}

cv::Mat Camera_Thread::frame(int type)
{
    if(type == DEPTH_IMAGE)
        return m_CameraRS.DepthFrame();
    else //if(type == COLOR_IMAGE)
        return m_CameraRS.ColorFrame();
}


void Camera_Thread::onJob()
{

}
