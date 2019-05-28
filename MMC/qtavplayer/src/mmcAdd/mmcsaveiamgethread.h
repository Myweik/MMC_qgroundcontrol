#ifndef MMCSAVEIAMGETHREAD_H
#define MMCSAVEIAMGETHREAD_H

//#include <VLCQtCore/VideoFrame.h>
#include <QQueue>
#include <QThread>

//typedef struct _WorkerTask
//{
//    VlcVideoFrame*  frame;
//    QString         fileName;
//} WorkerTask;


class MMCSaveIamgeThread : public QThread
{
    Q_OBJECT
public:
    explicit MMCSaveIamgeThread(QObject *parent = nullptr);
    ~MMCSaveIamgeThread();

    void    addTask(VlcVideoFrame task);
signals:

public slots:

private:
    void run();
    void saveYV12Image(VlcVideoFrame& task);
    void saveBGRAImage(VlcVideoFrame& task);

private:
    QMutex _mutex;
    QQueue<VlcVideoFrame> _taskQueue;
};

#endif // MMCSAVEIAMGETHREAD_H
