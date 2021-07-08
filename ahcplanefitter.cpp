#include "ahcplanefitter.h"


AHCPlaneFitter::AHCPlaneFitter()
{

}

void AHCPlaneFitter::ReadPly()
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr input(new pcl::PointCloud<pcl::PointXYZ>);
    if (pcl::io::loadPLYFile<pcl::PointXYZ>("./Image/Ply/pointcloud1.ply", *input) == -1)
    {
         PCL_ERROR("Couldnot read file.\n");
         system("pause");
         return;
     }

    //pcl::PointCloud<pcl::PointXYZ> input;
    //pcl::PointCloud<pcl::PointXYZRGB> cloud;
    //pcl::copyPointCloud(input, cloud);
    pcl::copyPointCloud(*input, cloud);
}

//process a new frame of point cloud
void AHCPlaneFitter::onNewCloud()
{
    //const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr pcloud = &cloud;

    //fill RGB
    if(rgb.empty() || rgb.rows!=cloud.height || rgb.cols!=cloud.width) {
        rgb.create(cloud.height, cloud.width, CV_8UC3);
        seg.create(cloud.height, cloud.width, CV_8UC3);
    }
    for(int i=0; i<(int)cloud.height; ++i) {
        for(int j=0; j<(int)cloud.width; ++j) {
            const pcl::PointXYZRGB& p=cloud.at(j,i);
            if(!pcl_isnan(p.z)) {
                rgb.at<cv::Vec3b>(i,j)=cv::Vec3b(p.b,p.g,p.r);
            } else {
                rgb.at<cv::Vec3b>(i,j)=cv::Vec3b(255,255,255);//whiten invalid area
            }
        }
    }

    //run PlaneFitter on the current frame of point cloud
    RGBDImage rgbd(cloud);
    //Timer timer(1000);
    //timer.tic();

#if 0
    pf.run(&rgbd, 0, &seg);
#else
    std::vector< std::vector<int> > membership;
    pf.run(&rgbd, &membership, 0);
    {
        static cv::Mat my_color_map;
        my_color_map.reserve(256);
        static const unsigned char default_colors[10][3] =
        {
            { 255, 0, 0 },
            { 255, 255, 0 },
            { 100, 20, 50 },
            { 0, 30, 255 },
            { 10, 255, 60 },
            { 80, 10, 100 },
            { 0, 255, 200 },
            { 10, 60, 60 },
            { 255, 0, 128 },
            { 60, 128, 128 }
        };
        if (my_color_map.empty()) {
            for (int i = 0; i < 10; ++i) {
                my_color_map.push_back(cv::Vec3b(default_colors[i]));
            }
            for (int i = 10; i < 256; ++i) {
                my_color_map.push_back(cv::Vec3b(rand() % 255, rand() % 255, rand() % 255));
            }
            my_color_map.at<cv::Vec3b>(255) = cv::Vec3b(0, 0, 0);
        }
        seg = cv::Mat::zeros(pf.height, pf.width, CV_8UC3);
        for (int i = 0; i < membership.size(); ++i) {
            const std::vector<int>& mi = membership[i];
            for (int k = 0; k < mi.size(); ++k) {
                const int pixid = mi[k];
                const int py = pixid / pf.width;
                const int px = pixid - py*pf.width;
                seg.at<cv::Vec3b>(py, px) = my_color_map.at<cv::Vec3b>(i);
            }
        }
        //cv::applyColorMap(tmp, seg, cv::COLORMAP_JET);
    }
#endif
    //double process_ms=timer.toc();

    //blend segmentation with rgb
    cv::cvtColor(seg,seg,cv::COLOR_RGB2BGR);
    seg=(rgb+seg)/2.0;

    //show frame rate
    std::stringstream stext;
    //stext<<"Frame Rate: "<<(1000.0/process_ms)<<"Hz";
    cv::putText(seg, stext.str(), cv::Point(15,15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,0,1));

    cv::imshow("rgb", rgb);
    cv::imshow("seg", seg);
    cv::waitKey();
}

void AHCPlaneFitter::SetPF()
{
    // related to T_mse
    int depthSigma = (int)(pf.params.depthSigma * 1e7);
    int mergeMSETol = (int)pf.params.stdTol_merge;
    int initMSETol = (int)pf.params.stdTol_init;

    // related to T_ang
    int z_near = (int)pf.params.z_near;
    int z_far = (int)pf.params.z_far;
    int angle_near = (int)15;
    int angle_far = (int)90;
    int simThMergeDeg = 60;
    int simThRefDeg = 30;

    // related to T_dz
    int depthAlpha = (int)(pf.params.depthAlpha * 100);
    int depthChangeTol = (int)(pf.params.depthChangeTol * 100);

    // other
    int minSupport = (int)pf.minSupport;
    int doRefine = (int)pf.doRefine;
    int erodeType = (int)pf.erodeType;

    static bool reported = false;
    pf.params.depthSigma = depthSigma*1e-7;
    pf.params.stdTol_init = (double)initMSETol;
    pf.params.stdTol_merge = (double)mergeMSETol;

    pf.params.z_near = (double)z_near;
    pf.params.z_far = (double)z_far;
    pf.params.angle_near = (double)MACRO_DEG2RAD(angle_near);
    pf.params.angle_far = (double)MACRO_DEG2RAD(angle_far);
    pf.params.similarityTh_merge = std::cos(MACRO_DEG2RAD(simThMergeDeg));
    pf.params.similarityTh_refine = std::cos(MACRO_DEG2RAD(simThRefDeg));

    pf.params.depthAlpha = (double)depthAlpha*0.01;
    pf.params.depthChangeTol = (double)depthChangeTol*0.01;

    pf.minSupport=minSupport;
    pf.doRefine=doRefine!=0;
    pf.erodeType = (ahc::ErodeType)erodeType;
}









