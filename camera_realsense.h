#ifndef CAMERA_REALSENSE_H
#define CAMERA_REALSENSE_H

#include <mutex>
#include "include_RealSense/Intel-RealSense-SDK-2.0/include/librealsense2/rs.hpp"
#include "opencv2/opencv.hpp"

class Camera_RealSense
{
public:
    Camera_RealSense();

    bool Start();
    void Stop();

    cv::Mat DepthFrame();
    cv::Mat ColorFrame();

private:
    bool m_Flag;
    rs2::pipeline pipe;
    cv::Mat m_ColorizedDepth;
    cv::Mat m_Color;
    std::mutex m_Lock;
};

#endif // CAMERA_REALSENSE_H
