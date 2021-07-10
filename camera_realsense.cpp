#include "camera_realsense.h"
#include <QThread>
#include "common1.h"

Camera_RealSense::Camera_RealSense(QObject *parent): QObject(parent)
{
    m_Flag = false;
}

bool Camera_RealSense::Start()
{
    g_ThreadFlag = true;
    rs2::config cfg;
    rs2::colorizer c;
    //cfg.enable_stream(RS2_STREAM_DEPTH, 640, 360, RS2_FORMAT_Z16, 30); //RS2_STREAM_DEPTH  RS2_STREAM_COLOR
    cfg.enable_stream(RS2_STREAM_DEPTH);
    cfg.enable_stream(RS2_STREAM_COLOR);
    pipe.start(cfg); //add: does rs really open?

    rs2::frameset frameset;
    for (int i = 0; i < 30; i++)
    {
        frameset = pipe.wait_for_frames();
    }

    while(1)
    {
        g_ThreadMutex.lock();
        m_Flag = g_ThreadFlag;
        g_ThreadMutex.unlock();

        if(!m_Flag)
        {
            pipe.stop();
            m_Color = cv::Mat();
            m_ColorizedDepth = cv::Mat();
            break;
        }

        frameset = pipe.wait_for_frames();
        if(frameset.size() < 1)
        {
            QThread::msleep(10);
            continue;
        }

        rs2::align align_to_color(RS2_STREAM_COLOR);
        frameset = align_to_color.process(frameset);
        rs2::frame depth_frame = frameset.get_depth_frame(); //frames
        rs2::frame color_frame = frameset.get_color_frame(); //frames

        rs2::frame colorized_depth = c.colorize(depth_frame);

        // Get the depth frame's dimensions
        //float width = depth_frame.get_width();
        //float height = depth_frame.get_height();
        cv::Mat depth(cv::Size(1280, 720), CV_8UC3, (void *)colorized_depth.get_data(), cv::Mat::AUTO_STEP);
        cv::Mat color(cv::Size(1280, 720), CV_8UC3, (void *)color_frame.get_data(), cv::Mat::AUTO_STEP);

        m_Lock.lock();
        m_ColorizedDepth = depth;
        m_Color = color;
        m_Lock.unlock();
        QThread::msleep(10);
    }

    return true;
}

void Camera_RealSense::Stop()
{
    pipe.stop();
    QThread::sleep(10);
}

cv::Mat Camera_RealSense::DepthFrame()
{
    if(m_ColorizedDepth.empty())
    {
        return cv::Mat();
    }
    else
    {
        return m_ColorizedDepth.clone();
    }
}

cv::Mat Camera_RealSense::ColorFrame()
{
    if(m_Color.empty())
    {
        return cv::Mat();
    }
    else
    {
        return m_Color.clone();
    }
}

cv::Mat Camera_RealSense::frame(int type)
{
    if(type == DEPTH_IMAGE)
        return DepthFrame();
    else //if(type == COLOR_IMAGE)
        return ColorFrame();
}

