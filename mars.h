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

// AHC 库
#include "ahcplanefitter.h"


// 相机内参 These parameters should read from config file
const double camera_factor = 1000;
const double camera_cx = 633.776;
const double camera_cy = 346.241;
const double camera_fx = 918.431;
const double camera_fy = 915.632;


/*struct Image3DClound
{
    int ww, hh;
    std::vector<Eigen::Vector3d> pc3d;

    inline int width() const { return ww; }
    inline int height() const { return hh; }
    inline bool get(const int row, const int col, double& x, double& y, double& z) const {
        const int index = row * ww + col;
        x=pc3d[index][0]; y=pc3d[index][1]; z=pc3d[index][2];
        if(pcl_isnan(z)==0 || z == 0) //return false if current depth is NaN
        {
            return false;
        }

        return true;
    }
};*/


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

    Image3DClound GenPly(cv::Mat depth);

    void GenPlane(PlaneFilter3 &planefilter, std::vector<std::vector<int> > &planeContour, cv::Mat &depth, std::vector<cv::Mat> &vecPlane);

    void FitPlane(const cv::Mat &points, cv::Mat& plane);

    void CalculateDist(std::vector<float>dx, std::vector<float>dy, std::vector<float>dz, float* plane, std::vector<float> &dist);
}

#endif // MARS_H
