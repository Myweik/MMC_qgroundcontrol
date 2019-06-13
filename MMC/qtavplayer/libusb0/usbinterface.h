#ifndef USBINTERFACE_H
#define USBINTERFACE_H
#include <QObject>
#include <QMutex>
//#include "artosynplayer.h"

#include "libusb.h"
//#include "fifo.h"
#include "../src/fifo.h"
#include "../src/AVThread.h"

#include <QTimer>

class HidThread;

class UsbHidTask : public Task{
public :
    ~UsbHidTask(){}
    enum UsbHidTaskCommand{
        UsbHidTaskCommand_Read,
        UsbHidTaskCommand_ReadyRead,

    };
    UsbHidTask(HidThread *codec,UsbHidTaskCommand command,double param = 0,QString param2 = ""):
        mCodec(codec),command(command),param(param),param2(param2){}
protected :
    /** 现程实现 */
    virtual void run();
private :
    HidThread *mCodec;
    UsbHidTaskCommand command;
    double param;
    QString param2;
};


class HidThread : public QObject
{
	Q_OBJECT
    friend class UsbHidTask;
public: 
	HidThread();

public:
	void ProcessVideoRead(BYTE* buf,int len);
    int write(char* data, int);
	int transfer_bulk_async(usb_dev_handle *dev,int ep,char *bytes,int size,int timeout);

    void addReadTask();
    void addReadyReadTask();
signals:
    void sendReadData(QByteArray);
	

protected:
	void run();

    void readVideo(int timeout = 1000);
    void readData(int timeout = 10);
private:
	QString messageStr; 
	volatile  bool stopped;
    QTimer* _readConfigTimer;

    AVThread mReadUsbThread;
    AVThread mReadyReadThread;
public slots:
    bool getConfig();
    bool setConfig();
    bool setOSD(bool state = true);

};
extern HidThread *hidThread;
#endif // USBINTERFACE_H
