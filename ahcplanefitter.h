#ifndef AHCPLANEFITTER_H
#define AHCPLANEFITTER_H

#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#ifdef USE_OPENNI1
#include <pcl/io/openni_grabber.h>
#else
#include <pcl/io/openni2_grabber.h>
#include <pcl/io/openni2/openni.h>
#endif

#include "opencv2/opencv.hpp"

#include "AHCPlaneFitter.hpp"

//code modify from: peac-master  - https://github.com/ai4ce/peac

// pcl::PointCloud interface for our ahc::PlaneFitter
template<class PointT>
struct OrganizedImage3D {
    const pcl::PointCloud<PointT>& cloud;
    //NOTE: pcl::PointCloud from OpenNI uses meter as unit,
    //while ahc::PlaneFitter assumes mm as unit!!!
    const double unitScaleFactor;

    OrganizedImage3D(const pcl::PointCloud<PointT>& c) : cloud(c), unitScaleFactor(1000) {}
    int width() const { return cloud.width; }
    int height() const { return cloud.height; }
    bool get(const int row, const int col, double& x, double& y, double& z) const {
        const PointT& pt=cloud.at(col,row);
        x=pt.x*unitScaleFactor; y=pt.y*unitScaleFactor; z=pt.z*unitScaleFactor;
        return pcl_isnan(z)==0; //return false if current depth is NaN
    }
};


typedef OrganizedImage3D<pcl::PointXYZRGB> RGBDImage;
typedef ahc::PlaneFitter<RGBDImage> PlaneFitter;


class AHCPlaneFitter
{
public:
    AHCPlaneFitter();

    void onNewCloud();

    void SetPF();

    void ReadPly();

protected:
    PlaneFitter pf;
    cv::Mat rgb, seg;
    bool done;

private:
    pcl::PointCloud<pcl::PointXYZRGB> cloud;

};

#endif // AHCPLANEFITTER_H
