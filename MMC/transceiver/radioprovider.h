#ifndef RADIOPROVIDER_H
#define RADIOPROVIDER_H

#pragma once

#include <QString>
#include <QThread>
#include <QByteArray>
#include <QSerialPort>
#include <QObject>

#include <atomic>
#include <QTimer>


class RadioProvider : public QThread
{
    Q_OBJECT
public:
    RadioProvider(const QString& device);
    ~RadioProvider();
    void _start();

public slots:
    int writeData(char type, QByteArray buff);
    void closeSerialSlot();

signals:
    void RTCMDataUpdate(int type, QByteArray message);
    void isconnectChanged(bool isconnect);
    void closeSerialSignals();

protected:
    void run();

private slots:
    void readBytes(); //读取数据
    void openSerialSlot();    //遍历串口

private:
    int  openSerial(QString name); //打开串口
    void analysisPack(QByteArray buff);
    //解析并组包
    int splicePacket(const uint8_t data, uint8_t *dst);
    //发送数据
    int _writeData(char type, QByteArray buff);

private:
    QString _device;        //串口名
    QSerialPort *_serial = nullptr;

//    QByteArray _readData;
//    int _read_index = 0;
//    int _read_count = 0;

    //---------------------
//    int connectNumber = 0;
    int _isconnect = 0;
//    QTimer*    _serialLostTimer = nullptr;

    QTimer*    _openSerialTimer = nullptr;

    bool _isRuning = false;
};

#endif // RADIOPROVIDER_H
