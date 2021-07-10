#ifndef FITPLANE_H
#define FITPLANE_H

#include <iostream>
#include <string>

// PCL ¿â
#include <pcl/console/parse.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>

#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/sample_consensus/sac_model_sphere.h>
//#include <pcl/visualization/pcl_visualizer.h>
#include <boost/thread/thread.hpp>


class FitPlane
{
public:
    int FitPlanebyRansac(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);
    //boost::shared_ptr<pcl::visualization::PCLVisualizer> simpleVis(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud,pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud_in);


};

#endif // FITPLANE_H
