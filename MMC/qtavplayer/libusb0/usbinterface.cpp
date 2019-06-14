#include "usbinterface.h"
#include <QDateTime>
#include <QDebug>
//#include "artosynplayer.h"
#include "hidapi.h"
//#include "newplayergui.h"
#include "libusb.h"

HDCOMD iHDCOMD;

HidThread *hidThread;
extern usbfifo *g_fifo;
extern QMutex usb_byte_fifo_mutex;

void UsbHidTask::run()
{
    switch(command){
    case UsbHidTaskCommand_Read:
        mCodec->readVideo();
        break;
    case UsbHidTaskCommand_ReadyRead:
        mCodec->readData();
        break;
    }
}

int HidThread::write(char* data,int len)
{
    if(COMMONDEV)
        return transfer_bulk_async(COMMONDEV, EP_OUT2, data, len, 100);
    else
        return -1;
}

void HidThread::addReadTask()
{
    if(mReadUsbThread.size() < 3)
        mReadUsbThread.addTask(new UsbHidTask(this,UsbHidTask::UsbHidTaskCommand_Read));
}

void HidThread::addReadyReadTask()
{
    if(mReadyReadThread.size() < 3)
        mReadyReadThread.addTask(new UsbHidTask(this,UsbHidTask::UsbHidTaskCommand_ReadyRead));
}

//void HidThread::run()
//{
//	while(1)
//	{
//        if (VIDEODEV ==NULL || COMMONDEV == NULL)
//		{
//			QThread::msleep(100);
//			continue;
//		}
//        static bool flag = false;
//        if(!flag){
//            flag = setOSD(true);
//        }

//        readVideo(100);

////        readData(1);
//        QThread::usleep(1);
//    }
//}

void HidThread::readVideo(int timeout)
{
    addReadTask();
    if(VIDEODEV == NULL)
        return;
    static unsigned char buf[BUF_SIZE_HID*12];
    int res = transfer_bulk_async(VIDEODEV, EP_IN2, (char*)buf, sizeof(buf), timeout);

//    qDebug() <<" ----------HidThread::run()" << res;

    if (res > 0 && g_fifo!=NULL)
    {
        ProcessVideoRead(buf,res);
    }
}

void HidThread::readData(int timeout)
{
    addReadyReadTask();
    if(COMMONDEV == NULL)
        return;

    static unsigned char buff[BUF_SIZE_HID*8];
    int res  = transfer_bulk_async(COMMONDEV, EP_IN3, (char*)buff, sizeof(buff), timeout);
    if (res > 0)
    {
        sendReadData(QByteArray((char*)buff, res));
//        qDebug() <<" ----------HidThread::run() 2" << res << QByteArray((char*)buff, res).toHex();
    }
}

bool HidThread::getConfig()
{
    static bool flag = false;
    if(!flag){
        flag = setOSD(true);
    }

    char buff[10] = {0xFF, 0x5A, 0x19, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    int res = write((char*)buff, 10);
    return res > 0;
}

bool HidThread::setConfig()
{
    char buff[11] = {0xFF, 0x5A, 0x5B, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
    int res = write((char*)buff, 11);
    return res > 0;
}

bool HidThread::setOSD(bool state)
{
    char buff[11] = {0xFF, 0x5A, 0x11, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
    if(!state){
        buff[10] = 0x00;
        buff[8]  = 0x00;
    }
    int res = write((char*)buff, 11);
    return res > 0;
}

void HidThread::ProcessVideoRead(BYTE* buf,int len)
{
    if(g_fifo)
        g_fifo->write(buf,len);
}

HidThread::HidThread()
{
    _readConfigTimer = new QTimer(this);
    connect(_readConfigTimer, &QTimer::timeout, this, &HidThread::getConfig);
    _readConfigTimer->start(150);

    addReadTask();
    addReadyReadTask();
}

int HidThread::transfer_bulk_async(usb_dev_handle *dev,int ep,char *bytes,int size,int timeout)
{

	if (dev==NULL)
	{
		return -1;
	}

	void* async_context = NULL;
	int ret;
	// Setup the async transfer.  This only needs to be done once
	// for multiple submit/reaps. (more below)
	//
	if (ep==EP_OUT2)
	{
		ret=usb_interrupt_setup_async(dev, &async_context, ep);
	}
	/*else if(ep==EP_IN2)
	{
		ret=usb_isochronous_setup_async(dev, &async_context, ep,size);
	}*/
	else
	{
		ret = usb_bulk_setup_async(dev, &async_context, ep);
	}
	
	if (ret < 0)
	{
		//qDebug()<<"1111111111"<<usb_strerror();

		//printf("error usb_bulk_setup_async:\n%s\n", usb_strerror());
		goto Done;
	}

	// Submit this transfer.  This function returns immediately and the
	// transfer is on it's way to the device.
	//
	ret = usb_submit_async(async_context, bytes, size);
	if (ret < 0)
	{
		//qDebug()<<"222222222222222"<<usb_strerror();
		//printf("error usb_submit_async:\n%s\n", usb_strerror());
		usb_free_async(&async_context);
		goto Done;
	}

	// Wait for the transfer to complete.  If it doesn't complete in the
	// specified time it is cancelled.  see also usb_reap_async_nocancel().
	//
	
	ret = usb_reap_async(async_context, timeout);
	//ret = usb_reap_async_nocancel(async_context, timeout);

	// Free the context.
	usb_free_async(&async_context);

Done:
	return ret;
}

