#ifndef MARS_H
#define MARS_H

#include <iostream>
#include <string>
#include <io.h>
#include <vector>
#include <QImage>

// Halcon 库
#include "HalconCpp.h"
#include "HDevThread.h"

// OpenCV 库
#include "opencv2/imgproc/types_c.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

// PCL 库
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>


// 定义点云类型
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;

// 相机内参 These parameters should read from config file
const double camera_factor = 1000;
const double camera_cx = 633.776;
const double camera_cy = 346.241;
const double camera_fx = 918.431;
const double camera_fy = 915.632;


namespace Mars
{
    QImage cvMat2QImage(const cv::Mat& mat);

    //cv::Mat QImage2cvMat(QImage image);

    bool HImage2QImage(HalconCpp::HImage &from, QImage &to);

    bool QImage2HImage(QImage &from, HalconCpp::HImage &to);

    HalconCpp::HObject MatToHImage(cv::Mat& cv_img);

    cv::Mat HImageToMat(HalconCpp::HObject &H_img);
}

namespace MyPointClound
{
    bool GenPly(cv::Mat depth, cv::Mat rgb);

}
#endif // MARS_H
