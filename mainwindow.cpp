#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>

#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionStart_triggered()
{
    QImage *img = new QImage;
    QString path = "D:/image/123456.bmp";

    if(!(img->load(path)))
    {
        ui->textEdit->append("Load img failed.");
        return;
    }

    ui->labelColor->setPixmap(QPixmap::fromImage(*img));
    ui->labelColor->setFixedSize(300,300);

    action();
}

void MainWindow::on_actionStop_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

// Main procedure
void MainWindow::action()
{

  // Local iconic variables
  HObject  ho_Image, ho_Region, ho_ConnectedRegions;
  HObject  ho_ImageReduced;

  // Local control variables
  int vid = ui->widget_Depth->winId();
  HTuple hv_Width, hv_Height;
  HTuple winID = vid;
  HTuple hv_WindowID;

  ReadImage(&ho_Image, "D:/image/123456.bmp");
  GetImageSize(ho_Image, &hv_Width, &hv_Height);
  SetWindowAttr("background_color","black");
  OpenWindow(0,0,hv_Width,hv_Height,winID,"visible","",&hv_WindowID);
  HDevWindowStack::Push((hv_WindowID));

  Threshold(ho_Image, &ho_Region, 0, 100);
  Connection(ho_Region, &ho_ConnectedRegions);
  ReduceDomain(ho_Image, ho_ConnectedRegions, &ho_ImageReduced);
  if (HDevWindowStack::IsOpen())
      DispObj(ho_ImageReduced, HDevWindowStack::GetActive());

}


