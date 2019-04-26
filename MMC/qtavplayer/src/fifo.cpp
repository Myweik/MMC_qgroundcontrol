//#include "stdafx.h"
#include "fifo.h"
#include <QDebug>
usbfifo::usbfifo(void)
{
    _buffQurue  = new BuffQueue;
    g_fifo = this;
    initialfifo();
}
usbfifo::~usbfifo(void)
{
     _usbByteMutex.lock();
    _buffQurue->release();
    delete _buffQurue;
    _buffQurue = nullptr;
     _usbByteMutex.unlock();
}

void usbfifo::write(unsigned char *buf, int length)
{
    qDebug() << "-------------------------usbfifo::write" << length;
    QByteArray* buff = new QByteArray;
    buff->resize(length);
    memcpy(buff->data(), buf, static_cast<ulong>(length));

    _usbByteMutex.lock();
    _buffQurue->put(buff);
    _usbByteMutex.unlock();
}

int usbfifo::read(unsigned char *buf)
{
    if(_buffQurue->size() < 1)
        return -1;
    _usbByteMutex.lock();
    QByteArray* buff = _buffQurue->get();
    _usbByteMutex.unlock();
    int len = buff->size();
    memcpy(buf, buff->data(), len);
    buff->clear();
    delete buff;
    return len;
}

int usbfifo::size()
{
    return  _buffQurue->size();
}

void usbfifo::initialfifo()
{
    _usbByteMutex.lock();
    _buffQurue->release();
    _usbByteMutex.unlock();
}
