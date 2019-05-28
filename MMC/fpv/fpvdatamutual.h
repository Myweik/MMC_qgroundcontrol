#ifndef FPVDATAMUTUAL_H
#define FPVDATAMUTUAL_H

#include <QObject>
#include <QSerialPort>
#include <QThread>

class fpvDataMutual  : public QThread
{
    Q_OBJECT
public:
    explicit fpvDataMutual(const QString& device);

signals:
    void sendWriteData(char type, QByteArray buff);

protected:
    void run();

private slots:
    void readBytes(); //读取数据
    int  writeData(char type, QByteArray buff);
    void openSerial();
private:
     int  _openSerial(QString name); //打开串口
    //发送数据
    int _writeData(char type, QByteArray buff);
private:
    QString _device;        //串口名
    QSerialPort *_serial = nullptr;
};

#endif // FPVDATAMUTUAL_H
