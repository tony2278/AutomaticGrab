#include "mars.h"
#include <QDebug>
#include <vector>
#include <string>



QImage Mars::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

/*
cv::Mat Mars::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}
*/

/**
 * @brief HImage2QImage 将 Halcon 的 HImage 转换为 Qt 的 QImage
 * @param from HImage ，暂时只支持 8bits 灰度图像和 8bits 的 3 通道彩色图像
 * @param to QImage ，这里 from 和 to 不共享内存。如果 to 的内存大小合适，那么就不用重新分配内存。所以可以加快速度。
 * @return  true 表示转换成功，false 表示转换失败
 * https://blog.csdn.net/liyuanbhu/article/details/91356988
 */
bool Mars::HImage2QImage(HalconCpp::HImage &from, QImage &to)
{
    Hlong width;
    Hlong height;
    from.GetImageSize(&width, &height);

    HalconCpp::HTuple channels = from.CountChannels();
    HalconCpp::HTuple type = from.GetImageType();

    if( strcmp(type[0].S(), "byte" )) // 如果不是 byte 类型，则失败
    {
        return false;
    }

    QImage::Format format;
    switch(channels[0].I())
    {
    case 1:
        format = QImage::Format_Grayscale8;
        break;
    case 3:
        format = QImage::Format_RGB32;
        break;
    default:
        return false;
    }

    if(to.width() != width || to.height() != height || to.format() != format)
    {
        to = QImage(static_cast<int>(width),
                    static_cast<int>(height),
                    format);
    }
    HalconCpp::HString Type;
    if(channels[0].I() == 1)
    {
        unsigned char * pSrc = reinterpret_cast<unsigned char *>( from.GetImagePointer1(&Type, &width, &height) );
        memcpy( to.bits(), pSrc, static_cast<size_t>(width) * static_cast<size_t>(height) );
        return true;
    }
    else if(channels[0].I() == 3)
    {
        uchar *R, *G, *B;
        from.GetImagePointer3(reinterpret_cast<void **>(&R),
                              reinterpret_cast<void **>(&G),
                              reinterpret_cast<void **>(&B), &Type, &width, &height);

        for(int row = 0; row < height; row ++)
        {
            QRgb* line = reinterpret_cast<QRgb*>(to.scanLine(row));
            for(int col = 0; col < width; col ++)
            {
                line[col] = qRgb(*R++, *G++, *B++);
            }
        }
        return true;
    }

    return false;
}

/**
 * @brief QImage2HImage 将 Qt QImage 转换为 Halcon 的 HImage
 * @param from 输入的 QImage
 * @param to 输出的 HImage ，from 和 to 不共享内存数据。 每次都会为 to 重新分配内存。
 * @return true 表示转换成功，false 表示转换失败。
 * https://blog.csdn.net/liyuanbhu/article/details/91356988
 */
bool Mars::QImage2HImage(QImage &from, HalconCpp::HImage &to)
{
    if(from.isNull()) return false;

    int width = from.width(), height = from.height();
    QImage::Format format = from.format();

    if(format == QImage::Format_RGB32 ||
            format == QImage::Format_ARGB32 ||
            format == QImage::Format_ARGB32_Premultiplied)
    {
        to.GenImageInterleaved(from.bits(), "rgbx", width, height, 0,  "byte", width, height, 0, 0, 8, 0);
        return true;
    }
    else if(format == QImage::Format_RGB888)
    {
        to.GenImageInterleaved(from.bits(), "rgb", width, height, 0,  "byte", width, height, 0, 0, 8, 0);
        return true;
    }
    else if(format == QImage::Format_Grayscale8 || format == QImage::Format_Indexed8)
    {
        to.GenImage1("byte", width, height, from.bits());
        return true;
    }
    return false;
}

HalconCpp::HObject Mars::MatToHImage(cv::Mat& cv_img)
{
    HalconCpp::HObject H_img;

    if (cv_img.channels() == 1)
    {
        int height = cv_img.rows, width = cv_img.cols;
        int size = height * width;
        uchar *temp = new uchar[size];

        memcpy(temp, cv_img.data, size);
        HalconCpp::GenImage1(&H_img, "byte", width, height, (Hlong)(temp));

        delete[] temp;
    }
    else if (cv_img.channels() == 3)
    {
        int height = cv_img.rows, width = cv_img.cols;
        int size= height * width;
        uchar *B = new uchar[size];
        uchar *G = new uchar[size];
        uchar *R = new uchar[size];

        for (int i = 0; i < height; i++)
        {
            uchar *p = cv_img.ptr<uchar>(i);
            for (int j = 0; j < width; j++)
            {
                B[i * width + j] = p[3 * j];
                G[i * width + j] = p[3 * j + 1];
                R[i * width + j] = p[3 * j + 2];
            }
        }
        HalconCpp::GenImage3(&H_img, "byte", width, height, (Hlong)(R), (Hlong)(G), (Hlong)(B));

        delete[] R;
        delete[] G;
        delete[] B;
    }
    return H_img;
}

cv::Mat Mars::HImageToMat(HalconCpp::HObject &H_img)
{
    cv::Mat cv_img;
    HalconCpp::HTuple channels, w, h;

    HalconCpp::ConvertImageType(H_img, &H_img, "byte");
    HalconCpp::CountChannels(H_img, &channels);

    if (channels.I() == 1)
    {
        HalconCpp::HTuple pointer;
        GetImagePointer1(H_img, &pointer, nullptr, &w, &h);
        int width = w.I(), height = h.I();
        int size = width * height;
        cv_img = cv::Mat::zeros(height, width, CV_8UC1);
        memcpy(cv_img.data, (void*)(pointer.L()), size);
    }

    else if (channels.I() == 3)
    {
        HalconCpp::HTuple pointerR, pointerG, pointerB;
        HalconCpp::GetImagePointer3(H_img, &pointerR, &pointerG, &pointerB, nullptr, &w, &h);
        int width = w.I(), height = h.I();
        //int size = width * height;
        cv_img = cv::Mat::zeros(height, width, CV_8UC3);
        uchar* R = (uchar*)(pointerR.L());
        uchar* G = (uchar*)(pointerG.L());
        uchar* B = (uchar*)(pointerB.L());
        for (int i = 0; i < height; ++i)
        {
            uchar *p = cv_img.ptr<uchar>(i);
            for (int j = 0; j < width; ++j)
            {
                p[3 * j] = B[i * width + j];
                p[3 * j + 1] = G[i * width + j];
                p[3 * j + 2] = R[i * width + j];
            }
        }
    }
    return cv_img;
}


bool MyPointClound::GenPly(cv::Mat depth, cv::Mat rgb)
{
    // 点云变量
    // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
    PointCloud::Ptr cloud(new PointCloud);
    // 遍历深度图
    for (int m = 0; m < depth.rows; m++)
    {
        for (int n = 0; n < depth.cols; n++)
        {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
                continue;
            // d 存在值，则向点云增加一个点
            PointT p;

            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z / camera_fy;

            if(rgb.empty())
            {
                p.b = 0;
                p.g = 255;
                p.r = 255;
            }
            else
            {
                // 从rgb图像中获取它的颜色
                // rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
                p.b = rgb.ptr<uchar>(m)[n * 3];
                p.g = rgb.ptr<uchar>(m)[n * 3 + 1];
                p.r = rgb.ptr<uchar>(m)[n * 3 + 2];
            }


            // 把p加入到点云中
            cloud->points.push_back(p);
        }
    }

    // 设置并保存点云
    cloud->height = 1;
    cloud->width = cloud->points.size();
    cloud->is_dense = false;
    std::string name = "./Image/Ply/pointcloud1";

    pcl::io::savePLYFile(name + ".ply", *cloud);   //将点云数据保存为ply文件
    //pcl::io::savePCDFile(name + ".pcd", *cloud);   //将点云数据保存为pcd文件
    // 清除数据并退出
    cloud->points.clear();
    return cloud->points.size();
}

Image3D MyPointClound::GenPly(cv::Mat depth)
{
    // 点云变量
    // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
    //PointCloud_::Ptr cloud(new PointCloud_);
    Image3D cloud;
    int index = 0;

    int space = depth.rows * depth.cols;
    cloud.pcloud3d.resize(space);
    cloud.ww = depth.cols;
    cloud.hh = depth.rows;

    // 遍历深度图
    for (int m = 0; m < depth.rows; m++)
    {
        for (int n = 0; n < depth.cols; n++)
        {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
            {
                cloud.pcloud3d[index++] = Eigen::Vector3d(0, 0, 0);
                continue;
            }

            // 计算这个点的空间坐标
            double z = double(d) / camera_factor;
            double x = (n - camera_cx) * z / camera_fx;
            double y = (m - camera_cy) * z / camera_fy;

            // 获取颜色
            //p.b = 0;
            //p.g = 255;
            //p.r = 255;

            // 把p加入到点云中
            cloud.pcloud3d[index++] = Eigen::Vector3d(x, y, z);
        }
    }

    // 设置并保存点云
    std::string name = "./Image/Ply/pointcloud2";
    //pcl::io::savePLYFile(name + ".ply", cloud);   //将点云数据保存为ply文件
    //pcl::io::savePCDFile(name + ".pcd", *cloud);   //将点云数据保存为pcd文件

    return cloud;
}

void MyPointClound::GenPlane(PlaneFilter3 &planefilter, std::vector<std::vector<int> > &planeContour, cv::Mat &depth, std::vector<cv::Mat> &vecPlane)
{
    int planeNumber = planefilter.extractedPlanes.size();
    if( planeNumber < 1)
    {
        return;
    }

    int index = 0;
    //std::vector<cv::Mat> vecPlane(planeNumber);
    vecPlane.resize(planeNumber);
    std::vector<int> vecIndex(planeNumber);

    for(int i=0; i <planeNumber; i++)
    {
        vecPlane[i] = cv::Mat(planeContour[i].size(), 3, CV_64FC1);
        vecIndex[i] = 0;
    }

    int row = vecPlane[0].rows;
    int col = vecPlane[0].cols;

    for (int m = 0; m < row; m++)
    {
        for (int n = 0; n < col; n++)
        {
            int planeId = planefilter.membershipImg.at<int>(m, n);
            for(int k=0; k<planeNumber; k++)
            {
                if(planeId == k)
                {
                    // 计算这个点的空间坐标
                    double *ptr = (double *)vecPlane[k].ptr(vecIndex[k]++);
                    double z = (double)(depth.at<unsigned short>(row, col));
                    double x = (n - camera_cx) * z / camera_fx;
                    double y = (m - camera_cy) * z / camera_fy;

                    // 获取值
                    ptr[0] = x;
                    ptr[1] = y;
                    ptr[2] = z;
                }
            }
        }
    }
}

/*
 *  最小二乘拟合平面，平面方程：Ax+By+Cz=D
 *  A = plane.at<float>(0,0)
 *  B = plane.at<float>(1,0)
 *  C = plane.at<float>(2,0)
 *  D = plane.at<float>(3,0)
 *
 *
 *  Create by KyJason 2018/01/25  注释added by tony2278
 *  https://my.oschina.net/u/1046919/blog/1612923
 *  拟合平面
 */

void MyPointClound::FitPlane(const cv::Mat &points, cv::Mat& plane)
{
    int rows = points.rows;
    int cols = points.cols;

    //Estimate geometric centroid.
    cv::Mat centroid = cv::Mat::zeros(1,cols,CV_32FC1);
    for(int i=0;i<cols;i++)
    {
        for(int j=0;j<rows;j++)
        {
            centroid.at<float>(0,i) += points.at<float>(j,i);
        }
        centroid.at<float>(0,i)/=rows;
    }

    // Subtract geometric centroid from each point.
    cv::Mat points2 = cv::Mat::ones(rows,cols,CV_32FC1);
    for(int i=0;i<rows;i++)
    {
        for(int j=0;j<cols;j++)
        {
            points2.at<float>(i,j) = points.at<float>(i,j) - centroid.at<float>(0,j) ;
        }
    }

    // Evaluate SVD of covariance matrix.
    cv::Mat A,W,U,V;
    cv::gemm(points2,points,1,NULL,0,A,CV_GEMM_A_T);
    cv::SVD::compute(A,W,U,V);

    // Assign plane coefficients by singular vector corresponding to smallest singular value.
    plane = cv::Mat::zeros(cols+1,1,CV_32FC1);
    for (int c = 0; c<cols; c++)
    {
        plane.at<float>(c,0) = V.at<float>(cols-1,c);
        plane.at<float>(cols,0) += plane.at<float>(c,0)*centroid.at<float>(0,c);
    }
}


/*
 *
 * 计算点到平面的距离
 *  Ax+By+Cz=D
 *  点（a,b,c） 到平面bai Ax+By+Cz=D的距离
 *  | A * a + B * b + C * c - D| /√(A ^ 2 + B ^ 2 + C ^ 2)
 *
 *  https://blog.csdn.net/u012719076/article/details/113124887
 */

void MyPointClound::CalculateDist(std::vector<float>dx, std::vector<float>dy, std::vector<float>dz, float* plane, std::vector<float> &dist)
{
    for (int i = 0; i < dx.size(); i++)
    {
        float ds = fabs(plane[0] * dx[i] + plane[1] * dy[i] + plane[2] * dz[i] - plane[3]);
        float dfen = sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
        if (!((double)dfen > -0.00001 && (double)dfen < -0.00001))
        {
            float ddist = ds / dfen;
            dist.push_back(ddist);
        }
    }
}
