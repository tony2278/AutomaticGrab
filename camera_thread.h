#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include <QObject>
#include <QMutex>
#include "camera_realsense.h"
#include "opencv2/opencv.hpp"
#include "common1.h"

class Camera_Thread : public QThread
{
    Q_OBJECT
public:
    Camera_Thread(QObject *parent = 0);
    ~Camera_Thread();

    void stop();
    cv::Mat frame(int type);

protected:
    //QThread的虚函数, 线程处理函数, 不能直接调用, 通过start()间接调用
    void run();

signals:
    void isDone();  //处理完成信号

public slots:
    void onJob();

private:
    QMutex m_Mutex; //定义互斥锁类
    cv::Mat m_DepthFrame, m_ColorFrame;

    Camera_RealSense m_CameraRS;

};


#endif // CAMERA_THREAD_H
