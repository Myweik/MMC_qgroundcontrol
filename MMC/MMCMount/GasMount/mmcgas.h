#ifndef MMCGAS_H
#define MMCGAS_H

#include <QObject>
#include <QMap>

typedef struct gas_data{
    float concentration ;
    short state;
    char name;
    char unit;
} gas_info;

class MMCGas : public QObject
{
    Q_OBJECT
public:
    explicit MMCGas(QObject *parent = 0);
    explicit MMCGas(float concenteration, int gasname, int gasstate, int gasuint, QObject *parent = 0);
    const MMCGas& operator=(const MMCGas& other);

    Q_PROPERTY(QString  name            READ name           CONSTANT)
    Q_PROPERTY(float    concentration   READ concentration  NOTIFY concentrationChange)
    Q_PROPERTY(int      state           READ state          NOTIFY stateChanged)
    // 0:正常  1:一级警报 2：二级警报  3.三级警报
    Q_PROPERTY(int      warningState    READ warningState   NOTIFY warningStateChanged)
    Q_PROPERTY(QString  uintName        READ uintName       NOTIFY uintChanged)
    //四档报警值
    Q_PROPERTY(float    warning1Value   READ warning1Value       WRITE setWarning1Value       NOTIFY warning1ValueChanged)
    Q_PROPERTY(float    warning2Value   READ warning2Value       WRITE setWarning2Value       NOTIFY warning2ValueChanged)
    Q_PROPERTY(float    warning3Value   READ warning3Value       WRITE setWarning3Value       NOTIFY warning3ValueChanged)

    Q_INVOKABLE void reset     (void);
    float   concentration      (void){ return _concentration; }
    int     state              (void){ return _gasState; }
    int     warningState       (void){ return _warningState; }
    int     uint               (void){ return _gasUint;}
    QString name               (void){ return _nameMap[_gasName]; }
    QString uintName           (void){ return _uintMap[_gasUint];}
    int     gasName            (void){ return _gasName; }
    float   warning1Value      (void){ return _warning1Value; }
    float   warning2Value      (void){ return _warning2Value; }
    float   warning3Value      (void){ return _warning3Value; }

    void    setConcentration(float value);
    void    setGasState(int state);
    void    setGasUint(int uint);
    void    setWarning1Value(float value);
    void    setWarning2Value(float value);
    void    setWarning3Value(float value);

private:
    void updateWarningState();
    void calculatConcentration(int gasname, float concenteration);

signals:
    void concentrationChange(float value);
    void stateChanged(int state);
    void uintChanged(QString uint);
    void warningStateChanged(int state);
    void warning1ValueChanged(float value);
    void warning2ValueChanged(float value);
    void warning3ValueChanged(float value);
public:
    float   _concentration;
    float   _warning1Value;
    float   _warning2Value;
    float   _warning3Value;
    int     _gasName;
    int     _gasState;
    int     _warningState;
    int     _gasUint;
    QMap<int, QString> _nameMap;
    QMap<int, QString> _uintMap;
    // key smallwarning largewarning
    QMap<int, QPair<int, int> > _defaultWarningMap;
};

#endif // MMCGAS_H
