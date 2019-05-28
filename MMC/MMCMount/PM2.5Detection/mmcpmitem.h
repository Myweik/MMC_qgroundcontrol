#ifndef MMCPMITEM_H
#define MMCPMITEM_H
#include <QObject>
#include<QDebug>


class MMCPMItem : public QObject
{
    Q_OBJECT
public:
    explicit MMCPMItem(QObject *parent = 0);
    explicit MMCPMItem(QString name, QString value, QString unit,  QObject *parent = 0);
    Q_PROPERTY(QString name      READ name     WRITE setName    NOTIFY nameChanged)
    Q_PROPERTY(QString value     READ value    WRITE setValue   NOTIFY valueChanged)
    Q_PROPERTY(QString unit      READ unit     WRITE setUnit    NOTIFY unitChanged)

    QString name       (void) { return _name;}
    QString value      (void) { return _value;}
    QString unit       (void) { return _unit;}

    void setName(QString name);
    void setValue(QString value);
    void setUnit(QString unit);

signals:
    void nameChanged(QString name);
    void valueChanged(QString value);
    void unitChanged(QString unit);

private:
   QString _name; //名字
   QString _value; //浓度
   QString _unit; //单位

};

#endif // MMCPMITEM_H
