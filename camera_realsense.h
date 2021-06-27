#ifndef CAMERA_REALSENSE_H
#define CAMERA_REALSENSE_H

#include <QObject>
#include <mutex>
#include "include_RealSense/Intel-RealSense-SDK-2.0/include/librealsense2/rs.hpp"
#include "opencv2/opencv.hpp"
#include "common1.h"


class Camera_RealSense: public  QObject
{
    Q_OBJECT
public:
    explicit Camera_RealSense(QObject *parent = 0);

    cv::Mat DepthFrame();
    cv::Mat ColorFrame();
    cv::Mat frame(int type);

public slots:
    bool Start();
    void Stop();

private:
    bool m_Flag;
    rs2::pipeline pipe;
    cv::Mat m_ColorizedDepth;
    cv::Mat m_Color;
    std::mutex m_Lock;
};

#endif // CAMERA_REALSENSE_H
