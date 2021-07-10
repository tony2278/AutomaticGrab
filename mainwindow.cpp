#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QDir>
#include "opencv2/opencv.hpp"
#include "common1.h"


using namespace HalconCpp;

bool g_ThreadFlag = false;
std::mutex g_ThreadMutex;


//to do : TCP, log, caibration...

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->append("by Tony2278");
    g_ThreadFlag = false;
    m_StartCamera = false;
    m_pAabout = NULL;

    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(100);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(startGUI()));

    QObject::connect(&m_Thread, SIGNAL(started()), &m_CameraRS, SLOT(Start()));
    //QObject::connect(this, SIGNAL(isStop()), &m_Thread, SLOT(quit()));
    //QObject::connect(&m_Thread, SIGNAL(finished()), &m_CameraRS, SLOT(Stop()));
    //m_Thread.start();
    m_CameraRS.moveToThread(&m_Thread);
}

MainWindow::~MainWindow()
{
    if(m_pAabout) delete m_pAabout;
    if(m_pTimer) delete m_pTimer;

    if(m_Thread.isRunning())
    {
        m_Thread.quit();
        m_Thread.wait();
        m_pTimer->stop();
    }

    delete ui;
}

void MainWindow::on_actionStart_triggered()
{
    // Local control variables
    //int vid = ui->widget_Depth->winId();
    //HTuple winID = vid;
    //HTuple hv_Width, hv_Height;
    //GetImageSize(hobjImg, &hv_Width, &hv_Height);
    //SetWindowAttr("background_color","black");
    //OpenWindow(0,0,ui->labelColor->width(), ui->labelColor->height(),winID,"visible","",&hv_WindowID);
    //HDevWindowStack::Push((hv_WindowID));

    on_actionStop_triggered();

    if(!m_Thread.isRunning())
    {
        m_Thread.start();
        m_pTimer->start();
        m_StartCamera = true;
    }
    else
    {
        m_StartCamera = false;
    }
}

void MainWindow::on_actionStop_triggered()
{
    if(m_Thread.isRunning())
    {
        m_pTimer->stop();
        g_ThreadMutex.lock();
        g_ThreadFlag = false;
        g_ThreadMutex.unlock();
        m_Thread.quit();
        m_Thread.wait();
    }

    ui->labelColor->setPixmap(QPixmap());
    ui->labelDepth->setPixmap(QPixmap());
    ui->labelResult->setPixmap(QPixmap());
    ui->labelColor->setText("Color");
    ui->labelDepth->setText("Depth");
    ui->labelResult->setText("Result");
}

void MainWindow::on_actionExit_triggered()
{
    on_actionStop_triggered();

    this->close();
}

// Main procedure
void MainWindow::action(HObject hobjImg)
{
  // Local iconic variables
  HObject  ho_Region, ho_ConnectedRegions;
  HObject  ho_ImageReduced;

  Threshold(hobjImg, &ho_Region, 0, 100);
  Connection(ho_Region, &ho_ConnectedRegions);
  ReduceDomain(hobjImg, ho_ConnectedRegions, &ho_ImageReduced);
  if (HDevWindowStack::IsOpen())
      DispObj(ho_ImageReduced, HDevWindowStack::GetActive());
}

QImage MainWindow::actionX(HObject hobjImg)
{
    // Local iconic variables
    HObject  ho_Region, ho_ConnectedRegions;
    HObject  ho_ImageReduced;
    QImage result;

    Threshold(hobjImg, &ho_Region, 100, 255);
    Connection(ho_Region, &ho_ConnectedRegions);
    ReduceDomain(hobjImg, ho_ConnectedRegions, &ho_ImageReduced);
    HImage hImg(ho_ImageReduced);
    Mars::HImage2QImage(hImg, result);
    return result;
}

void MainWindow::on_actionTake_Ply_triggered()
{
    QDir *folder = new QDir();
    bool exist = folder->exists("Image");
    if(exist)
    {
    }
    else
    {
        //创建文件夹
        bool ok = folder->mkdir("Image");
    }

    folder->cd("./Image");
    exist = folder->exists("Ply");
    if(exist)
    {
    }
    else
    {
        //创建文件夹
        folder->mkdir("Ply");
    }

    if(!m_DepthFlame.empty() && !m_ColorFlame.empty())
    {
        MyPointClound::GenPly(m_DepthFlame, m_ColorFlame);
        ui->textEdit->append("./Image/Ply/pointcloud1.ply done");
    }
}

void MainWindow::on_actionTake_Photo_triggered()
{
    QDir *folder = new QDir();
    bool exist = folder->exists("Image");
    if(exist)
    {
    }
    else
    {
        //创建文件夹
        bool ok = folder->mkdir("Image");
    }

    folder->cd("./Image");
    exist = folder->exists("Photo");
    if(exist)
    {
    }
    else
    {
        //创建文件夹
        folder->mkdir("Photo");
    }

    if(!m_DepthFlame.empty() && !m_ColorFlame.empty())
    {
        cv::imwrite("./Image/Photo/depth1.png", m_DepthFlame);
        cv::imwrite("./Image/Photo/color1.png", m_ColorFlame);
        ui->textEdit->append("depth1.png done");
        ui->textEdit->append("color1.png done");
    }
}

void MainWindow::on_actionBinocular_Calibration_triggered()
{

}

void MainWindow::on_actionPlane_Calibration_triggered()
{
    //Method 1 Use AHC
    //m_Ahcpf.SetPF();
    //m_Ahcpf.ReadPly();
    //m_Ahcpf.onNewCloud();   //Some error occur

    //Method 2 Use RANSAC
    //FitPlane                //need to be verified later
    /*pcl::PointCloud<pcl::PointXYZ>::Ptr input(new pcl::PointCloud<pcl::PointXYZ>);
    if (pcl::io::loadPLYFile<pcl::PointXYZ>("./Image/Ply/pointcloud1.ply", *input) == -1)
    {
         PCL_ERROR("Couldnot read file.\n");
         system("pause");
         return;
    }

    m_FitPlane.FitPlanebyRansac(input);*/

    //Method 3 Use AHC
    if(m_DepthFlame.empty())
    {
        m_DepthFlame = cv::imread("D:/image/depth1.png", -1);
        if(m_DepthFlame.empty()) return;
    }

    m_PlaneFilter.clear();
    m_PlaneContour.clear();


    m_PlaneFilter.minSupport = 200;
    m_PlaneFilter.params.z_near = 0;
    m_PlaneFilter.params.z_far = 1000;
    m_PlaneFilter.params.depthAlpha = 0.09;
    m_PlaneFilter.windowWidth = m_PlaneFilter.windowHeight = 2;
    m_PlaneFilter.params.stdTol_merge = 5;
    m_PlaneFilter.params.stdTol_init = 5;
    m_PlaneFilter.params.similarityTh_merge = std::cos(MACRO_DEG2RAD(20.0));
    m_PlaneFilter.params.similarityTh_refine = std::cos(MACRO_DEG2RAD(10.0));

    MyPointClound::GenPly(m_DepthFlame, cv::Mat());
    m_Cloud = MyPointClound::GenPly(m_DepthFlame);
    cv::Mat roiimg = cv::Mat(m_Cloud.hh, m_Cloud.ww, CV_8UC3);
    m_PlaneFilter.run(&m_Cloud, &m_PlaneContour, &roiimg);
    cv::imwrite("Plane_Calibration.png", roiimg);
    int cnt = (int)m_PlaneContour.size();
    if(cnt <= 0)
    {
        ui->textEdit->append("pSeg Plane number = 0");
        return;
    }
    else
    {
        ui->textEdit->append("pSeg Plane number = " + QString::number(cnt));
    }

    double *centerptr = m_PlaneFilter.extractedPlanes[0]->center;
    double *normalptr = m_PlaneFilter.extractedPlanes[0]->normal;
    cv::Vec3d planeNormal(normalptr[0], normalptr[1], normalptr[2]);
    //planeNormal = cv::normalize(planeNormal);
    if(planeNormal[2] < 0)
    {
        planeNormal = -planeNormal;
    }

    std::vector<cv::Mat> vecPlane;
    cv::Mat planepoints;
    cv::Mat plane;

    MyPointClound::GenPlane(m_PlaneFilter, m_PlaneContour, m_DepthFlame, vecPlane);
    MyPointClound::FitPlane(vecPlane[0], plane);

    //平面参数方程：Ax+By+Cz=D   plane分别对应ABCD
    cv::Vec4d tablePlane = cv::Vec4d(plane.at<double>(0, 0), plane.at<double>(1, 0), plane.at<double>(2, 0), plane.at<double>(3, 0));
    if(tablePlane[2] < 0)
    {
        tablePlane = -tablePlane;
    }

    cv::Vec3d planeNormalNew(tablePlane[0], tablePlane[1], tablePlane[2]);
    cv::Vec3d planeCenter(0, 0, -tablePlane[3]/tablePlane[2]);
}

void MainWindow::startGUI()
{
    cv::Mat colorframe = m_CameraRS.frame(COLOR_IMAGE);
    if(colorframe.empty())
    {
        return;
        //continue;
    }

    cv::Mat depthFrame = m_CameraRS.frame(DEPTH_IMAGE);
    if(depthFrame.empty())
    {
        return;
        //continue;
    }

    m_DepthFlame = depthFrame;
    m_ColorFlame = colorframe;
    QImage image = Mars::cvMat2QImage(colorframe);
    QPixmap pixmap = QPixmap::fromImage(image);
    pixmap = pixmap.scaled(ui->labelColor->size(), Qt::KeepAspectRatio);
    ui->labelColor->setPixmap(pixmap);

    QImage imageDepth = Mars::cvMat2QImage(depthFrame);
    QPixmap pixmapDepth = QPixmap::fromImage(imageDepth);
    pixmapDepth = pixmapDepth.scaled(ui->labelDepth->size(), Qt::KeepAspectRatio);
    ui->labelDepth->setPixmap(pixmapDepth);

    HObject hobjImage = Mars::MatToHImage(colorframe);
    QImage resultImage = actionX(hobjImage);
    QPixmap pixmapResult = QPixmap::fromImage(resultImage);
    pixmapResult = pixmapResult.scaled(ui->labelResult->size(), Qt::KeepAspectRatio);
    ui->labelResult->setPixmap(pixmapResult);
    //QThread::msleep(50);
}

void MainWindow::on_actionAbout_triggered()
{
    if(!m_pAabout)
    {
        m_pAabout = new FormAbout();
    }

    m_pAabout->show();
}
