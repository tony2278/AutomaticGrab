#ifndef PLANESEGMENT_H
#define PLANESEGMENT_H

#include <iostream>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>


class PlaneSegment
{
public:
    PlaneSegment();

    int SegmentPCL(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);
};

#endif // PLANESEGMENT_H
