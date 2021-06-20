#include "camera_realsense.h"
#include <QThread>

Camera_RealSense::Camera_RealSense()
{
    m_Flag = false;
}

bool Camera_RealSense::Start()
{
    rs2::config cfg;
    rs2::colorizer c;
    //cfg.enable_stream(RS2_STREAM_DEPTH, 640, 360, RS2_FORMAT_Z16, 30); //RS2_STREAM_DEPTH  RS2_STREAM_COLOR
    //pipe.start(cfg);
    pipe.start(); //add: does rs really open?

    rs2::align align_to_color(RS2_STREAM_COLOR);
    rs2::frameset frames, aligned_frames;

    for (int i = 0; i < 30; i++)
    {
        frames = pipe.wait_for_frames();
    }

    m_Flag = true;
    while(m_Flag)
    {
        frames = pipe.wait_for_frames();
        aligned_frames = align_to_color.process(frames);
        rs2::frame depth_frame = aligned_frames.get_depth_frame(); //frames
        rs2::frame color_frame = aligned_frames.get_color_frame(); //frames

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
    if(m_Flag)
    {
        m_Flag = false;
        QThread::sleep(1);
        pipe.stop();
    }
}

cv::Mat Camera_RealSense::DepthFrame()
{
    return m_ColorizedDepth;
}

cv::Mat Camera_RealSense::ColorFrame()
{
    return m_Color;
}
