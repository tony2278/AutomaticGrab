#include "planesegment.h"


//https://pointclouds.org/documentation/tutorials/planar_segmentation.html#planar-segmentation
//modify by tony2278


PlaneSegment::PlaneSegment()
{

}

//class SACSegmentation 怎样获得分割的平面?
//PCL利用RANSAC自行拟合分割平面
//https://www.cnblogs.com/billwong/archive/2019/04/12/10697927.html
//https://blog.csdn.net/weixin_43464623/article/details/116498687

int PlaneSegment::SegmentPCL(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
    //pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

    // Fill in the cloud data
    /*cloud->width  = 15;
    cloud->height = 1;
    cloud->points.resize(cloud->width * cloud->height);

    // Generate the data
    for(auto& point: *cloud)
    {
        point.x = 1024 * rand() /(RAND_MAX + 1.0f);
        point.y = 1024 * rand() /(RAND_MAX + 1.0f);
        point.z = 1.0;
    }

    // Set a few outliers
    (*cloud)[0].z = 2.0;
    (*cloud)[3].z = -2.0;
    (*cloud)[6].z = 4.0;

    std::cerr << "Point cloud data: " << cloud->size() << " points" << std::endl;
    for(const auto& point: *cloud)
        std::cerr << "    " << point.x << " "
                            << point.y << " "
                            << point.z << std::endl;
    */

    pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
    pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
    // Create the segmentation object
    pcl::SACSegmentation<pcl::PointXYZ> seg;
    // Optional
    seg.setOptimizeCoefficients(true);
    // Mandatory
    seg.setModelType(pcl::SACMODEL_PLANE);
    seg.setMethodType(pcl::SAC_RANSAC);
    seg.setDistanceThreshold(0.01);

    seg.setInputCloud(cloud);
    seg.segment(*inliers, *coefficients);

    if(inliers->indices.size() == 0) //检查一下是否有平面分割出来
    {
        PCL_ERROR("Could not estimate a planar model for the given dataset.\n");
        return(-1);
    }

    std::cerr << "Model coefficients: " << coefficients->values[0] << " "
                                      << coefficients->values[1] << " "
                                      << coefficients->values[2] << " "
                                      << coefficients->values[3] << std::endl;

    std::cerr << "Model inliers: " << inliers->indices.size() << std::endl;
    /*for(const auto& idx: inliers->indices)
    std::cerr << idx << "    " << cloud->points[idx].x << " "
                               << cloud->points[idx].y << " "
                               << cloud->points[idx].z << std::endl;
    */

    pcl::ExtractIndices<pcl::PointXYZ> extract;  //抽取分割得到的平面点云
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plane(new pcl::PointCloud<pcl::PointXYZ>);

    extract.setInputCloud(cloud);
    extract.setIndices(inliers);
    extract.setNegative(false);
    extract.filter(*cloud_plane);
    std::cout << "PointCloud representing the planar component: " << cloud_plane->size() << " data points." << std::endl;
    //PointCloud representing the planar component: 61490 data points

    return(0);
}
