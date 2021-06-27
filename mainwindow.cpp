#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QImage>
#include <QPixmap>
#include "opencv2/opencv.hpp"
#include "common1.h"


using namespace HalconCpp;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_Start = false;


    QObject::connect(&m_Thread, SIGNAL(started()), &m_CameraRS, SLOT(Start()));
    QObject::connect(this, SIGNAL(isStop()), &m_CameraRS, SLOT(Stop()));
    m_Thread.start();
    m_CameraRS.moveToThread(&m_Thread);
}

MainWindow::~MainWindow()
{
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

    m_Start = true;
    //while(1)
    {
        if(!m_Start)
        {
            return;
            //break;
        }

        cv::Mat colorframe = m_CameraRS.frame(COLOR_IMAGE);
        if(colorframe.empty())
        {
            QThread::msleep(1000);
            return;
            //continue;
        }

        cv::Mat depthFrame = m_CameraRS.frame(DEPTH_IMAGE);
        if(depthFrame.empty())
        {
            QThread::msleep(1000);
            return;
            //continue;
        }

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
        QThread::msleep(50);
    }
}

void MainWindow::on_actionStop_triggered()
{
    m_Start = false;
    emit isStop();

    ui->labelColor->clear();
    ui->labelDepth->clear();
    ui->labelResult->clear();

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

    Threshold(hobjImg, &ho_Region, 0, 100);
    Connection(ho_Region, &ho_ConnectedRegions);
    ReduceDomain(hobjImg, ho_ConnectedRegions, &ho_ImageReduced);
    HImage hImg(ho_ImageReduced);
    Mars::HImage2QImage(hImg, result);
    return result;
}

void MainWindow::on_actionTake_Ply_triggered()
{

}

void MainWindow::on_actionTake_Photo_triggered()
{

}

void MainWindow::on_actionBinocular_Calibration_triggered()
{

}

void MainWindow::on_actionPlane_Calibration_triggered()
{

}
