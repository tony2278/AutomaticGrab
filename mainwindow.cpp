#include "mainwindow.h"
#include "ui_mainwindow.h"
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionStart_triggered()
{
    m_CameraThread.start();

    QThread::msleep(3000);
    cv::Mat frame = m_CameraThread.frame(COLOR_IMAGE);
    if(frame.empty())
    {
        ui->textEdit->append("frame is empty!");
        m_CameraThread.stop();
        return;
    }

    // Local control variables
    int vid = ui->widget_Depth->winId();
    HTuple winID = vid;
    //HTuple hv_Width, hv_Height;
    //GetImageSize(hobjImg, &hv_Width, &hv_Height);
    SetWindowAttr("background_color","black");
    OpenWindow(0,0,ui->labelColor->width(), ui->labelColor->height(),winID,"visible","",&hv_WindowID);
    HDevWindowStack::Push((hv_WindowID));

    m_Start = true;
    while(1)
    {
        if(!m_Start) break;

        cv::Mat frame = m_CameraThread.frame(COLOR_IMAGE);
        if(frame.empty())
        {
            m_CameraThread.stop();
            return;
        }

        cv::Mat resizedFrame;
        //cv::resize(frame, resizedFrame, cv::Size(ui->labelColor->width(), ui->labelColor->height()));

        HObject hobjImage;
        QImage image = Mars::cvMat2QImage(frame);
        if(image.width() < 1)
        {
            ui->textEdit->append("Capture failed, use default image.");
            m_CameraThread.stop();
            return;

            /*
            QString path = "D:/image/123456.bmp";
            ReadImage(&hobjImage, "D:/image/123456.bmp");
            if(!(img->load(path)))
            {
                m_CameraThread.stop();
                return;
            }*/
        }
        else
        {
            hobjImage = Mars::MatToHImage(frame);
        }

        QPixmap pixmap = QPixmap::fromImage(image);
        pixmap = pixmap.scaled(ui->labelColor->size(), Qt::KeepAspectRatio);
        ui->labelColor->setPixmap(pixmap);

        action(hobjImage);
    }

    m_CameraThread.stop();
}

void MainWindow::on_actionStop_triggered()
{
    m_Start = false;
    m_CameraThread.stop();

    ui->labelColor->clear();
    if (HDevWindowStack::IsOpen())
    {
        //ClearWindow(HDevWindowStack::GetActive());
        CloseWindow(HDevWindowStack::GetActive());
    }
}

void MainWindow::on_actionExit_triggered()
{
    m_CameraThread.stop();

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

void MainWindow::on_actionTake_Ply_triggered()
{

}

void MainWindow::on_actionTake_Photo_triggered()
{

}
