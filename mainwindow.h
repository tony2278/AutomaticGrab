#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QImage>
#include<qtimer.h>
#include "HalconCpp.h"
#include "HDevThread.h"

#include "formabout.h"

#include "camera_realsense.h"
#include "mars.h"

#include "ahcplanefitter.h"
#include "fitplane.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void action(HalconCpp::HObject hobjImg);
    QImage actionX(HalconCpp::HObject hobjImg);

private slots:
    void on_actionStart_triggered();

    void on_actionStop_triggered();

    void on_actionExit_triggered();

    void on_actionTake_Ply_triggered();

    void on_actionTake_Photo_triggered();

    void on_actionBinocular_Calibration_triggered();

    void on_actionPlane_Calibration_triggered();

    void startGUI();

    void on_actionAbout_triggered();

signals:
    void isStop();  //处理完成信号

private:
    Ui::MainWindow *ui;

    HalconCpp::HTuple hv_WindowID;
    Camera_RealSense m_CameraRS;
    FitPlane m_FitPlane;
    QThread m_Thread;
    QTimer *m_pTimer;
    cv::Mat m_DepthFlame;
    cv::Mat m_ColorFlame;
    bool m_StartCamera;

    //Form UI
    FormAbout *m_pAabout;

    //AHC plane
    AHCPlaneFitter m_Ahcpf;
    PlaneFilter3 m_PlaneFilter;
    std::vector<std::vector<int> > m_PlaneContour;

    Image3D m_Cloud;
};

#endif // MAINWINDOW_H
