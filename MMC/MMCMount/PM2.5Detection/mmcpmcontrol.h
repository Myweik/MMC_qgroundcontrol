#ifndef MMCPMCONTROL_H
#define MMCPMCONTROL_H
#include <QObject>
#include <QSerialPort>
#include <QGeoCoordinate>
#include <QStringList>
#include <QTimer>
#include <QObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "SerialLink.h"

#include "QmlObjectListModel.h"
#include "MMCMount/PM2.5Detection/mmcpmitem.h"
class MMCPMControl : public QObject
{
    Q_OBJECT
public:
    explicit MMCPMControl(QObject *parent = 0);
    ~MMCPMControl();

    Q_PROPERTY(bool                     isOpen              READ isOpen             NOTIFY isOpenChanged)
    Q_PROPERTY(QStringList              portList            READ portList           NOTIFY portListChanged)
    Q_PROPERTY(QStringList              serialBaudRates     READ serialBaudRates    CONSTANT)
    Q_PROPERTY(QmlObjectListModel*      pmList              READ pmList             NOTIFY pmListChanged)
    Q_INVOKABLE bool openPort(QString name, QString baudRate);
    Q_INVOKABLE void closePort();

    Q_INVOKABLE void setCoordinate(const QGeoCoordinate &coordinate);

    bool                isOpen          (void)  { return _isOpen; }
    QStringList         portList        (void)  { return _portList; }

    QmlObjectListModel* pmList          (void)  { return &_pmList;}
    QStringList         serialBaudRates (void);
    void serialPorts();

private slots:
    // 读取数据
    void readBytes(QByteArray buff);
    void updateSerialPorts();

signals:
    //列表更改
    void portListChanged(QStringList ports);
    void isOpenChanged(bool open);
    void linkLost(bool lost);
    void pmListChanged(QmlObjectListModel* list);

private:
    void rtcm_packet(const uint8_t data);

private:
    // 串口指针
    SerialLink*    _serial;
    SerialConfiguration*   _serialConfig;
    // 可用串口列表
    QStringList     _portList;
    QTimer*         _timer;
    bool            _isOpen;
    QByteArray      laser_info;
    QFile *         _PM_file;
    bool            _connection;
    QmlObjectListModel _pmList;
    QString          _latitude;
    QString          _longitude;
};

#endif // MMCPMCONTROL_H
