#ifndef MARS_H
#define MARS_H

#include <QImage>
#include "HalconCpp.h"
#include "HDevThread.h"

#include "opencv2/imgproc/types_c.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

namespace Mars
{

    QImage cvMat2QImage(const cv::Mat& mat);

    //cv::Mat QImage2cvMat(QImage image);

    bool HImage2QImage(HalconCpp::HImage &from, QImage &to);

    bool QImage2HImage(QImage &from, HalconCpp::HImage &to);

    HalconCpp::HObject MatToHImage(cv::Mat& cv_img);

    cv::Mat HImageToMat(HalconCpp::HObject &H_img);
}

#endif // MARS_H
