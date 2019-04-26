#include "mmclidar.h"
#include <QDebug>


MMCLidar::MMCLidar(QObject *parent) : QObject(parent)
{

}

MMCLidar::MMCLidar(float distance, int amp, int flag, QObject *parent)
    : QObject(parent)
    , _distance(distance)
    , _amp(amp)
    , _flag(flag)
{

}

const MMCLidar& MMCLidar::operator=(const MMCLidar& other)
{
    _distance = other._distance;
    _amp = other._amp;
    _flag = other._flag;
    return *this;
}

void MMCLidar::setDistance(float distance)
{
    _distance = distance;
    emit distanceChanged(_distance);
}

void MMCLidar::setAmp(int amp)
{
    _amp = amp;
    emit ampChanged(_amp);
}

void MMCLidar::setFlag(int flag)
{
    _flag = flag;
    emit flagChanged(_flag);
}
