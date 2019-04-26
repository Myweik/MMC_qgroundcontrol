#include "mmcpmitem.h"

MMCPMItem::MMCPMItem(QObject *parent) : QObject(parent)
{

}

MMCPMItem::MMCPMItem(QString name, QString value, QString unit,  QObject *parent)
    : QObject(parent)
    , _name(name)
    , _unit(unit)
    , _value(value)
{
    emit nameChanged(name);
    emit valueChanged(value);
    emit unitChanged(unit);
}

void MMCPMItem::setName(QString name)
{
    _name = name;
    emit nameChanged(name);
}

void MMCPMItem::setValue(QString value)
{
    _value = value;
    emit valueChanged(value);
}

void MMCPMItem::setUnit(QString unit)
{
    _unit = unit;
    emit unitChanged(unit);
}

