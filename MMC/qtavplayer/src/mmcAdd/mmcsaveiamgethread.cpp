#include "mmcsaveiamgethread.h"
#include <QImage>
#include <qDebug>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

MMCSaveIamgeThread::MMCSaveIamgeThread(QObject *parent)
    : QThread(parent)
{

}

MMCSaveIamgeThread::~MMCSaveIamgeThread()
{

}

void MMCSaveIamgeThread::addTask(VlcVideoFrame task)
{
    _mutex.lock();
    _taskQueue.enqueue(task);
    _mutex.unlock();

    if(!isRunning()) start();
}

void MMCSaveIamgeThread::run()
{
    VlcVideoFrame task;
    while(1){
        if(_taskQueue.count()){
            _mutex.lock();
            task = _taskQueue.dequeue();
            _mutex.unlock();

            if(task.planeCount == 3){
                saveYV12Image(task);
            }else{
                saveBGRAImage(task);
            }
            msleep(1);
        }
        msleep(10);
    }
}

void MMCSaveIamgeThread::saveYV12Image(VlcVideoFrame& task)
{
    //Save
    if(task.planeCount == 3){
        QImage image(task.visiblePitch[0], task.visibleLines[0], QImage::Format_RGB888);
        unsigned char* buff_Y =  (unsigned char*)task.plane[0].data();
        unsigned char* buff_V =  (unsigned char*)task.plane[1].data();
        unsigned char* buff_U =  (unsigned char*)task.plane[2].data();
        unsigned char* buff_RGB   = image.bits();
        int iWidth = task.visiblePitch[0];
        int iHeight = task.visibleLines[0];
        int i       = 0;
        int len_Y   = 0;
        int len_VU  = 0;
        for(int y=0; y < iHeight; y++)
        {
            len_Y  = y * iWidth;
            len_VU = (y >> 2) * iWidth;
            for(int x=0; x < iWidth; x++)
            {
                unsigned char* Y = &buff_Y[len_Y + x];
                unsigned char* V = &buff_V[len_VU + (x>>1)];
                unsigned char* U = &buff_U[len_VU + (x>>1)];

                short rgb[3];
                rgb[0] = short(*Y + 1.370705 * (*V - 128));// r分量值
                rgb[1] = short(*Y - 0.698001 * (*U - 128)  - 0.703125 * (*V - 128)); // g分量值
                rgb[2] = short(*Y + 1.732446 * (*U - 128)); // b分量值
                i = y * 3 * iWidth + x * 3;
                for(int j =0; j < 3; j++){
                    if(rgb[j] <= 255 && rgb[j] >= 0){
                        buff_RGB[i+ j] = rgb[j];
                    }else{
                        buff_RGB[i+ j] = (rgb[j] < 0) ? 0 : 255;
                    }
                }
            }
        }

        QString dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "/MMC Station/Photo/" + QDateTime::currentDateTime().toString("yyyyMMdd") + "/";
        QDir tmpDir1;
        bool ok = false;
        if(tmpDir1.exists(dir)){
            ok = true;
        }else{
            ok = tmpDir1.mkpath(dir);
        }
        if(ok){
            QString fileName = QDateTime::currentDateTime().toString("hhmmsszzz");
            image.save(QString("%1%2%3").arg(dir).arg(fileName).arg(".jpg"));
        }
    }
}

void MMCSaveIamgeThread::saveBGRAImage(VlcVideoFrame& task)
{
    QImage image((uchar*)task.plane[0].data(), task.width, task.height, QImage::Format_ARGB32);

    QString dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "/MMC Station/Photo/" + QDateTime::currentDateTime().toString("yyyyMMdd") + "/";
    QDir tmpDir1;
    bool ok = false;
    if(tmpDir1.exists(dir)){
        ok = true;
    }else{
        ok = tmpDir1.mkpath(dir);
    }
    if(ok){
        QString fileName = QDateTime::currentDateTime().toString("hhmmsszzz");
        image.save(QString("%1%2%3").arg(dir).arg(fileName).arg(".jpg"));
    }
}
