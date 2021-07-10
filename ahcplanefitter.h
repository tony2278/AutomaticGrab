#ifndef AHCPLANEFITTER_H
#define AHCPLANEFITTER_H

#include <vector>


//PCL
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

struct Image3D
{
    int ww, hh;
    std::vector<Eigen::Vector3d> pcloud3d;

    int width() const { return ww; }
    int height() const { return hh; }
    bool get(const int row, const int col, double& x, double& y, double& z) const {
        const int index = row * ww + col;
        x=pcloud3d[index][0]; y=pcloud3d[index][1]; z=pcloud3d[index][2];
        if(pcl_isnan(z)==0 || z == 0) //return false if current depth is NaN
        {
            return false;
        }

        return true;
    }
};

typedef OrganizedImage3D<pcl::PointXYZRGB> RGBDImage;
typedef OrganizedImage3D<pcl::PointXYZ> RGBImage;
typedef ahc::PlaneFitter<RGBDImage> PlaneFitter1;
typedef ahc::PlaneFitter<RGBImage> PlaneFitter2;
typedef ahc::PlaneFitter<Image3D> PlaneFilter3;

// 定义点云类型
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;
typedef pcl::PointCloud<Image3D> PointCloud_;

class AHCPlaneFitter
{
public:
    AHCPlaneFitter();

    void onNewCloud();

    void SetPF();

    void ReadPly();

protected:
    PlaneFitter1 pf1;
    PlaneFitter2 pf2;
    PlaneFilter3 pf3;
    cv::Mat rgb, seg;
    bool done;

private:
    pcl::PointCloud<pcl::PointXYZRGB> cloud;
    pcl::PointCloud<pcl::PointXYZ> cloud2;
};

#endif // AHCPLANEFITTER_H
