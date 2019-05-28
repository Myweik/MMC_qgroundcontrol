#ifndef USBEXAMPLE_H
#define USBEXAMPLE_H

#include <QObject>
#include <QUsbDevice>
#include <QUsbTransfer>

#include <QMutex>
#include "AVThread.h"

const QUsbDevice::Endpoint USB_PIPE_IN = 0x86; /* Bulk output endpoint for responses */
const QUsbDevice::Endpoint USB_PIPE_OUT = 0x01; /* Bulk input endpoint for commands */
const quint16 USB_TIMEOUT_MSEC = 300;

class UsbExample;

class UsbExampleTask : public Task{
public :
    ~UsbExampleTask(){}
    enum UsbExampleTaskCommand{
        UsbExampleTaskCommand_Read,
        UsbExampleTaskCommand_ReadyRead,

    };
    UsbExampleTask(UsbExample *codec,UsbExampleTaskCommand command,double param = 0,QString param2 = ""):
        mCodec(codec),command(command),param(param),param2(param2){}
protected :
    /** 现程实现 */
    virtual void run();
private :
    UsbExample *mCodec;
    UsbExampleTaskCommand command;
    double param;
    QString param2;
};

class UsbExample : public QObject
{
    Q_OBJECT
    friend class UsbExampleTask;
public:
    explicit UsbExample(QObject *parent = Q_NULLPTR);
    ~UsbExample(void);
    void setupDevice(void);
    bool openDevice(void);
    void closeDevice(void);
    bool openHandle(void);
    void readUsb();
    void closeHandle(void);
    void read(QByteArray *buf);
    void write(QByteArray *buf);

    void addReadTask();
    void addReadyReadTask();

    void ProcessVideoRead(unsigned char* buf,int len);
public slots:
    void onReadyRead(void);
    void onWriteComplete(qint64 bytes);

signals:

private:
    QUsbDevice *m_usb_dev;
    QUsbTransfer *m_transfer_handler;
    QByteArray m_send, m_recv;
    QMutex m_recvMutex;
    QUsbDevice::Id m_filter;
    QUsbDevice::Config m_config;
    QUsbDevice::Endpoint m_read_ep, m_write_ep;

    AVThread mReadUsbThread;
    AVThread mReadyReadThread;
};
//extern HidThread *hidThread;
extern QMutex usb_byte_fifo_mutex;

#endif // USBEXAMPLE_H
