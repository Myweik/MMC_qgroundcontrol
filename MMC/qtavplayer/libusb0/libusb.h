#ifndef LIBUSB_H
#define LIBUSB_H

#include <QObject>

#include "usbmonitor.h"
#include "lusb0_usb.h"
#include "usbinterface.h"

#include <QStringList>
#include <QStringListModel>


#define MY_VID 0xaaaa
#define MY_PID 0xaa97
#define MY_GOLDEN_VID 0xaaaa
#define MY_GOLDEN_PID 0xaa98
#define EP_IN4 0x82
#define EP_IN3 0x84
#define EP_IN2 0x86
#define EP_IN1 0x88
#define EP_IN5 0x85
#define EP_OUT1 0x02
#define EP_OUT2 0x01
#define EP_OUT3 0x06
#define BUF_SIZE_HID 512
#define MY_INTF 0
#define MY_CONFIG 1

extern MonitorThread *monitorThread;

extern usb_dev_handle *COMMONDEV;
extern usb_dev_handle *VIDEODEV;
extern usb_dev_handle *AUDIODEV;
extern usb_dev_handle *CUSTOMERDEV;

class LibUsb : public QObject
{
    Q_OBJECT
public:
    explicit LibUsb(QObject *parent = nullptr);
    bool setConfig(); //対频

signals:
    void sendConfigData(QByteArray);

public slots:
    void GetHid(QString vid,QString pid);
    void UpdateDeviceList();

private:
    QStringList devList;
    QStringListModel *devListModel;
};

extern LibUsb*    getUsbExample();
#endif // LIBUSB_H
