#ifndef MMCLIDAR_H
#define MMCLIDAR_H

#include <QObject>
#include <QMap>

class MMCLidar : public QObject
{
    Q_OBJECT
public:
    explicit MMCLidar(QObject *parent = 0);
    explicit MMCLidar(float distance, int amp, int flag, QObject *parent = 0);
    const MMCLidar& operator=(const MMCLidar& other);

    Q_PROPERTY(float    distance    READ distance   WRITE   setDistance     NOTIFY distanceChanged)
    Q_PROPERTY(int      amp         READ amp        WRITE   setAmp          NOTIFY ampChanged)
    Q_PROPERTY(int      flag        READ flag       WRITE   setFlag         NOTIFY flagChanged)


    float   distance()      {   return _distance; }
    int     amp()           {   return _amp; }
    int     flag()          {   return _flag; }

    void setDistance(float distance);
    void setAmp(int amp);
    void setFlag(int flag);


signals:
    void distanceChanged(float value);
    void ampChanged(int amp);
    void flagChanged(int flag);
public:
    float _distance;
    int   _amp;
    int   _flag;
};

#endif // MMCLIDAR_H
