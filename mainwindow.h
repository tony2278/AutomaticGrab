#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "HalconCpp.h"
#include "HDevThread.h"

#include "camera_thread.h"
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

private slots:
    void on_actionStart_triggered();

    void on_actionStop_triggered();

    void on_actionExit_triggered();

    void on_actionTake_Ply_triggered();

    void on_actionTake_Photo_triggered();

private:
    Ui::MainWindow *ui;

    HalconCpp::HTuple hv_WindowID;
    Camera_Thread m_CameraThread;
    bool m_Start;
};

#endif // MAINWINDOW_H
