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
    //QThread���麯��, �̴߳�����, ����ֱ�ӵ���, ͨ��start()��ӵ���
    void run();

signals:
    void isDone();  //��������ź�

public slots:
    void onJob();

private:
    QMutex m_Mutex; //���廥������
    cv::Mat m_DepthFrame, m_ColorFrame;

    Camera_RealSense m_CameraRS;

};


#endif // CAMERA_THREAD_H
