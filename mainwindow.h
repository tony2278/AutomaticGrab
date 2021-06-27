#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QImage>
#include "HalconCpp.h"
#include "HDevThread.h"

#include "camera_realsense.h"
#include "mars.h"

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

signals:
    void isStop();  //处理完成信号

private:
    Ui::MainWindow *ui;

    HalconCpp::HTuple hv_WindowID;
    Camera_RealSense m_CameraRS;
    QThread m_Thread;
    bool m_Start;
};

#endif // MAINWINDOW_H
