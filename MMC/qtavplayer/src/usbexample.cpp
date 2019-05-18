#include "usbexample.h"

#ifdef interface
#undef interface
#endif
#include "fifo.h"

#include <QDateTime>

usbfifo *g_fifo;
QMutex usb_byte_fifo_mutex;

void UsbExampleTask::run()
{
    switch(command){
    case UsbExampleTaskCommand_Read:
        mCodec->readUsb();
        break;
    }
}

//----------------------------------------------------------------------------------

UsbExample::UsbExample(QObject *parent)
    : QObject(parent), m_usb_dev(new QUsbDevice()), m_transfer_handler(Q_NULLPTR)
{
    this->setupDevice();
    m_usb_dev->setTimeout(1);

    m_send.append(static_cast<char>(0xF1u));
    m_send.append(static_cast<char>(0x80u));

    if (this->openDevice()) {
        qInfo("Device open!");
        this->write(&m_send);

        addReadTask();
    } else {
        qWarning("Could not open device!");
    }
}

UsbExample::~UsbExample()
{
    this->closeDevice();
    m_usb_dev->deleteLater();
}

void UsbExample::setupDevice()
{
    /* There are 2 ways of identifying devices depending on the platform.
   * You can use both methods, only one will be taken into account.
   */
    qDebug("setupDevice");

    m_usb_dev->setLogLevel(QUsbDevice::logDebug);

    // desc.idProduct == 0xAA97 && desc.idVendor == 0xAAAA
    m_filter.pid = 0xAA97;
    m_filter.vid = 0xAAAA;

    //
    m_config.alternate = 0;
    m_config.config = 1;
    m_config.interface = 0;

    //
    m_read_ep = 0x86;
    m_write_ep = 0x01;

    //
    m_usb_dev->setId(m_filter);
    m_usb_dev->setConfig(m_config);
}

bool UsbExample::openDevice()
{
    if (m_usb_dev->open() == QUsbDevice::statusOK) {
        // Device is open
        return this->openHandle();
    }
    return false;
}

void UsbExample::closeDevice()
{
    qDebug("Closing");

    if (m_usb_dev->isConnected()) {
        this->closeHandle();
        m_usb_dev->close();
    }
}

bool UsbExample::openHandle()
{
    qDebug("Opening Handle");
    bool b = false;
    m_transfer_handler = new QUsbTransfer(m_usb_dev, QUsbTransfer::bulkTransfer, m_read_ep, m_write_ep);

    connect(m_transfer_handler, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_transfer_handler, SIGNAL(bytesWritten(qint64)), this, SLOT(onWriteComplete(qint64)));

    b = m_transfer_handler->open(QIODevice::ReadWrite);

//    if (b) {
//        m_transfer_handler->setPolling(true);
//    }

    return b;
}

void UsbExample::addReadTask()
{
    mReadUsbThread.addTask(new UsbExampleTask(this,UsbExampleTask::UsbExampleTaskCommand_Read));
}

void UsbExample::readUsb()
{
    if (m_transfer_handler) {
        m_transfer_handler->poll();
    }
    QThread::msleep(3);
    addReadTask();
}

void UsbExample::closeHandle()
{
    qDebug("Closing Handle");
    if (m_transfer_handler != Q_NULLPTR) {
        m_transfer_handler->close();
        m_transfer_handler->disconnect();
        qInfo() << m_transfer_handler->errorString();
        delete m_transfer_handler;
        m_transfer_handler = Q_NULLPTR;
    }
}

void UsbExample::read(QByteArray *buf)
{
    qint64 len = m_transfer_handler->bytesAvailable();
    if(!len)
        return;
    QByteArray b = /*m_transfer_handler->read(len); */ m_transfer_handler->readAll();  //yong zhe ge hui gua
//    qDebug() << "Reading"  << b.size();
    buf->append(b);
}

void UsbExample::write(QByteArray *buf)
{
    qDebug() << "Writing" << *buf << buf->size();
    if (m_transfer_handler->write(buf->constData(), buf->size()) < 0) {
        qWarning("write failed");
    }
}

void UsbExample::ProcessVideoRead(unsigned char *buf, int len)
{
//    int fifoactsize=0;
//    usb_byte_fifo_mutex.lock();

    g_fifo->write(buf,len);

//    usb_byte_fifo_mutex.unlock();
}

void UsbExample::onReadyRead()
{
    this->read(&m_recv);
    qDebug() << "onReadyRead" << m_recv.size();
    if(m_recv.size() > 0){
        ProcessVideoRead((uchar*)m_recv.data(), m_recv.size());
         m_recv.clear();
    }
//    this->write(&m_send);
}

void UsbExample::onWriteComplete(qint64 bytes)
{
    qDebug() << "onWriteComplete" << bytes << "bytes";
}


