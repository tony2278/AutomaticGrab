#include "fitplane.h"


//PCL���ƽ�������ģ��
//https://blog.csdn.net/suyunzzz/article/details/99441110


//����һ�����ӻ�����
/*
boost::shared_ptr<pcl::visualization::PCLVisualizer> FitPlane::simpleVis (pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud,pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud_in)
{
    // --------------------------------------------
    // -----Open 3D viewer and add point cloud-----
    // --------------------------------------------
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
    int v1(0);
    int v2(0);

    viewer->createViewPort(0,0,0.5,1,v1);
    viewer->createViewPort(0.5,0,1,1,v2);
    viewer->setBackgroundColor (0, 0, 0,v1);
    viewer->setBackgroundColor(0,180,100,v2);
    viewer->addPointCloud<pcl::PointXYZ> (cloud, "sample cloud",v1);//��ʾ����ĵ��ƣ������
    viewer->addPointCloud<pcl::PointXYZ>(cloud_in,"cloud_in",v2);
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud",v1);
    viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,3,"cloud_in",v2);

    viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR,233,0,0,"cloud_in",v2);//ԭʼ������ɫΪ��ɫ
    viewer->addCoordinateSystem (1.0);
    viewer->initCameraParameters();
    return (viewer);
}*/

int FitPlane::FitPlanebyRansac()
{
    srand(time(NULL));
    // initialize PointClouds
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr final (new pcl::PointCloud<pcl::PointXYZ>);

    std::string argument = "-f";

    // populate our PointCloud with points
    cloud->width    = 5000;
    cloud->height   = 1;
    cloud->is_dense = false;
    cloud->points.resize (cloud->width * cloud->height);
    for (size_t i = 0; i < cloud->points.size (); ++i)
    {
        if (argument.compare("-f") == 0  || argument.compare("-sf") == 0 )
        {
          cloud->points[i].x =  rand () / (RAND_MAX + 1.0);
          cloud->points[i].y =  rand () / (RAND_MAX + 1.0);
          if (i % 5 == 0)
            cloud->points[i].z =  rand () / (RAND_MAX + 1.0);
          else if(i % 2 == 0)
            cloud->points[i].z =  sqrt( 1 - (cloud->points[i].x * cloud->points[i].x)
                                          - (cloud->points[i].y * cloud->points[i].y));
          else
            cloud->points[i].z =  - sqrt( 1 - (cloud->points[i].x * cloud->points[i].x)
                                            - (cloud->points[i].y * cloud->points[i].y));
        }
        else
        {
          cloud->points[i].x =  rand () / (RAND_MAX + 1.0);
          cloud->points[i].y =  rand () / (RAND_MAX + 1.0);
          if( i % 5 == 0)
            cloud->points[i].z = rand () / (RAND_MAX + 1.0);
          else
            cloud->points[i].z = -1 * (cloud->points[i].x + cloud->points[i].y);
        }
    }

    std::vector<int> inliers;

    // created RandomSampleConsensus object and compute the appropriated model
    pcl::SampleConsensusModelSphere<pcl::PointXYZ>::Ptr
    model_s(new pcl::SampleConsensusModelSphere<pcl::PointXYZ> (cloud));
    pcl::SampleConsensusModelPlane<pcl::PointXYZ>::Ptr
    model_p (new pcl::SampleConsensusModelPlane<pcl::PointXYZ> (cloud));
    if(argument.compare("-f") == 0)
    {
        pcl::RandomSampleConsensus<pcl::PointXYZ> ransac (model_p);
        ransac.setDistanceThreshold (.01);
        ransac.computeModel();
        ransac.getInliers(inliers);
    }
    else if (argument.compare("-sf") == 0 )
    {
        pcl::RandomSampleConsensus<pcl::PointXYZ> ransac (model_s);
        ransac.setDistanceThreshold (0.008);
        ransac.computeModel();
        ransac.getInliers(inliers); //���ز��ҵ����ڵ�
    }

    std::cout<<"cloud_in: "<<cloud->size()<<std::endl;
    std::cout<<"inlier.size: "<<inliers.size()<<std::endl;

    // copies all inliers of the model computed to another PointCloud
    pcl::copyPointCloud<pcl::PointXYZ>(*cloud, inliers, *final);//����inliers������������ĵ���cloud�еĵ���ѡ��final���

    std::cerr<<"final: "<<final->size()<<std::endl;
    // creates the visualization object and adds either our orignial cloud or all of the inliers
    // depending on the command line arguments specified.
    ///boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;
    if( (argument.compare("-f")  == 0) || (argument.compare("-sf") == 0) )
        {
        ///viewer = simpleVis(final,cloud);
        ///
        }
    else{
        ///viewer = simpleVis(cloud,cloud);
    }
    ///while (!viewer->wasStopped ())
    {
    ///    viewer->spinOnce (100);
    ///    boost::this_thread::sleep (boost::posix_time::microseconds (100000));
    }
        return 0;
}


