#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
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
    g_ThreadFlag = true;
    m_StartCamera = false;
    m_pAabout = NULL;

    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(100);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(startGUI()));

    QObject::connect(&m_Thread, SIGNAL(started()), &m_CameraRS, SLOT(Start()));
    //QObject::connect(this, SIGNAL(isStop()), &m_Thread, SLOT(quit()));
    //QObject::connect(&m_Thread, SIGNAL(finished()), &m_CameraRS, SLOT(Stop()));
    m_Thread.start();
    m_CameraRS.moveToThread(&m_Thread);
}

MainWindow::~MainWindow()
{
    if(m_pAabout) delete m_pAabout;

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
    m_pTimer->stop();
    g_ThreadMutex.lock();
    g_ThreadFlag = false;
    g_ThreadMutex.unlock();

    //ui->labelColor->clear();
    //ui->labelDepth->clear();
    //ui->labelResult->clear();
    ui->labelColor->setPixmap(QPixmap());
    ui->labelDepth->setPixmap(QPixmap());
    ui->labelResult->setPixmap(QPixmap());
    ui->labelColor->setText("Color");
    ui->labelDepth->setText("Depth");
    ui->labelResult->setText("Result");

    m_Thread.quit();
    m_Thread.wait();
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
