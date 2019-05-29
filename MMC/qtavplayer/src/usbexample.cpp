#include "usbexample.h"

#ifdef interface
#undef interface
#endif
#include "fifo.h"

#include <QDateTime>

usbfifo *g_fifo = nullptr;
QMutex usb_byte_fifo_mutex;

void UsbExampleTask::run()
{
    switch(command){
    case UsbExampleTaskCommand_Read:
        mCodec->readUsb();
        break;
    case UsbExampleTaskCommand_ReadyRead:
        mCodec->onReadyRead();
        mCodec->onConfigReadyRead();
        break;
    }
}

//----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Singleton
static UsbExample* usbExample = NULL;
UsbExample*
getUsbExample()
{
    if(!usbExample)
        usbExample = new UsbExample();
    return usbExample;
}

UsbExample::UsbExample(QObject *parent)
    : QObject(parent), m_usb_dev(new QUsbDevice()), m_transfer_handler(Q_NULLPTR)
{
    this->setupDevice();

    _readConfigTimer = new QTimer(this);
    connect(_readConfigTimer, &QTimer::timeout, this, &UsbExample::getConfig);

    m_usb_dev->setTimeout(1);

//    m_send.append(static_cast<char>(0xF1u));
//    m_send.append(static_cast<char>(0x80u));

    if (this->openDevice()) {
        qInfo("Device open!");

        addReadTask();
        _readConfigTimer->start(150);
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
    qDebug() << "setupDevice";

    m_usb_dev->setLogLevel(QUsbDevice::logNone);

    // desc.idProduct == 0xAA97 && desc.idVendor == 0xAAAA
    m_filter.pid = 0xAA97;
    m_filter.vid = 0xAAAA;

    //
    m_config.alternate = 0;
    m_config.config = 1;
    m_config.interface = 0;

    //
//    m_read_ep = read_ep; ///*0x86*/ /*0x88*/ 0x84;
//    m_write_ep = write_ep; //0x01;

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

    m_transfer_handler = new QUsbTransfer(m_usb_dev, QUsbTransfer::bulkTransfer,  0x86, 0x02/* m_read_ep, m_write_ep*/);
    connect(m_transfer_handler, /*SIGNAL(readyRead())*/ &QUsbTransfer::readyRead, this, &UsbExample::addReadyReadTask);
    connect(m_transfer_handler, SIGNAL(bytesWritten(qint64)), this, SLOT(onWriteComplete(qint64)));
    b = m_transfer_handler->open(QIODevice::ReadWrite);

    m_config_handler = new QUsbTransfer(m_usb_dev, QUsbTransfer::bulkTransfer, 0x84, 0x01); //set
    connect(m_config_handler, /*SIGNAL(readyRead())*/ &QUsbTransfer::readyRead, this, &UsbExample::addReadyReadTask);
    b &= m_config_handler->open(QIODevice::ReadWrite);

    return b;
}

void UsbExample::addReadTask()
{
    mReadUsbThread.addTask(new UsbExampleTask(this,UsbExampleTask::UsbExampleTaskCommand_Read));
}

void UsbExample::addReadyReadTask()
{
    if(mReadyReadThread.size() < 3)
        mReadyReadThread.addTask(new UsbExampleTask(this,UsbExampleTask::UsbExampleTaskCommand_ReadyRead));
}

void UsbExample::readUsb()
{
    if (m_transfer_handler) {
        m_transfer_handler->poll();
    }
    if (m_transfer_handler) {
         m_config_handler->poll();
    }

    addReadTask();
}

void UsbExample::onConfigReadyRead()
{
    QByteArray recv;
    this->configRead(&recv);
    if(recv.size() > 0){
        sendConfigData(recv);
    }
}


void UsbExample::onReadyRead()
{
    m_recvMutex.lock();
    this->read(&m_recv);

    if(m_recv.size() > 0){
//        qDebug() << "onReadyRead" << m_recv.size();
        ProcessVideoRead((uchar*)m_recv.data(), m_recv.size());
         m_recv.clear();
    }
    m_recvMutex.unlock();
    QThread::usleep(1);
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

    if (m_config_handler != Q_NULLPTR) {
        m_config_handler->close();
        m_config_handler->disconnect();
        qInfo() << m_config_handler->errorString();
        delete m_config_handler;
        m_config_handler = Q_NULLPTR;
    }
}

void UsbExample::read(QByteArray *buf)
{
    qint64 len = m_transfer_handler->bytesAvailable();
    if(!len)
        return;
    QByteArray b = /*m_transfer_handler->read(len); */ m_transfer_handler->readAll();  //yong zhe ge hui gua
//    qDebug() << "Reading"  << b.toHex();
    buf->append(b);
}

void UsbExample::write(QByteArray *buf)
{
    qDebug() << "Writing" << *buf << buf->size();
    if (m_transfer_handler->write(buf->constData(), buf->size()) < 0) {
        qWarning("write failed");
    }
}

void UsbExample::configRead(QByteArray *buf)
{
    qint64 len = m_config_handler->bytesAvailable();
    if(!len)
        return;
    QByteArray b = /*m_transfer_handler->read(len); */ m_config_handler->readAll();  //yong zhe ge hui gua
    buf->append(b);
}

void UsbExample::configWrite(QByteArray *buf)
{
    qDebug() << "Writing" << *buf << buf->size();
    if (m_config_handler->write(buf->constData(), buf->size()) < 0) {
        qWarning("write failed");
    }
}

void UsbExample::setConfig()
{
    char buff[11] = {0xFF, 0x5A, 0x5B, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
    QByteArray buf(buff, 11);
    if(m_config_handler)
        this->configWrite(&buf);
}

void UsbExample::getConfig()
{
    char buff[10] = {0xFF, 0x5A, 0x19, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    QByteArray buf(buff, 10);
    if(m_config_handler)
        this->configWrite(&buf);
}

void UsbExample::ProcessVideoRead(unsigned char *buf, int len)
{
    if(g_fifo)
        g_fifo->write(buf,len);
}

void UsbExample::onWriteComplete(qint64 bytes)
{
    qDebug() << "onWriteComplete" << bytes << "bytes";
}




