#include "mmcgas.h"
#include "QGCApplication.h"
#include <QDebug>


MMCGas::MMCGas(QObject *parent) : QObject(parent)
{

}
MMCGas::MMCGas(float concenteration, int gasname, int gasstate, int gasuint, QObject *parent)
    : QObject(parent)
    , _gasName(gasname)
    , _gasState(gasstate)
    , _warning1Value(50)
    , _warning2Value(200)
    , _warning3Value(1000)
    , _warningState(2)
{
    _nameMap[1] = "EX";
    _nameMap[2] = "CO";
    _nameMap[3] = "O2";
    _nameMap[4] = "H2";
    _nameMap[5] = "CH4";
    _nameMap[6] = "C3H8";
    _nameMap[7] = "CO2";
    _nameMap[8] = "O3";
    _nameMap[9] = "H2S";
    _nameMap[10] = "SO2";
    _nameMap[11] = "NH3";
    _nameMap[12] = "CL2";
    _nameMap[13] = "ETO";
    _nameMap[14] = "HCL";
    _nameMap[15] = "PH3";
    _nameMap[16] = "HBr";
    _nameMap[17] = "HCN";
    _nameMap[18] = "AsH3";
    _nameMap[19] = "HF";
    _nameMap[20] = "Br2";
    _nameMap[21] = "NO";
    _nameMap[22] = "NO2";
    _nameMap[23] = "NOX";
    _nameMap[24] = "CLO2";
    _nameMap[25] = "SiH4";
    _nameMap[26] = "CS2";
    _nameMap[27] = "F2";
    _nameMap[28] = "B2H6";
    _nameMap[29] = "GeH4";
    _nameMap[30] = "N2";
    _nameMap[31] = "THT";
    _nameMap[32] = "C2H2";
    _nameMap[33] = "C2H4";
    _nameMap[34] = "CH2O";
    _nameMap[35] = "LPG";
    _nameMap[36] = "HC";
    _nameMap[37] = "VOC";
    _nameMap[38] = "H2O2";
    _nameMap[39] = "VOC";
    _nameMap[40] = "SF6";
    _nameMap[41] = "C7H8";
    _nameMap[42] = "C4H6";
    _nameMap[43] = "COS";
    _nameMap[44] = "N2H4";
    _nameMap[45] = "SeH2";
    _nameMap[46] = "C8H8";
    _nameMap[47] = "C4H8";
    _nameMap[48] = "CH2";

    _uintMap[0] = "%LEL";
    _uintMap[1] = "%VOL";
    _uintMap[2] = "PPM";
    _uintMap[3] = "PPb";
    _uintMap[4] = "mg/m3";


    calculatConcentration(gasname, concenteration);
    updateWarningState();

    emit warning1ValueChanged(_warning1Value);
    emit warning2ValueChanged(_warning2Value);
    emit warning3ValueChanged(_warning3Value);

    switch(gasname)
    {
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 20:
    case 21:
    case 23:
    case 37:
        _gasUint = 4;
        break;
    default:
        _gasUint = gasuint;
        break;
    }
}

const MMCGas& MMCGas::operator=(const MMCGas& other)
{
    _concentration = other._concentration;
    _gasName = other._gasName;
    _gasState = other._gasState;
    _gasUint = other._gasUint;
    return *this;
}

void MMCGas::setConcentration(float value)
{
    _concentration = value;
    updateWarningState();
    emit concentrationChange(value);
}

void MMCGas::setGasState(int state)
{
    _gasState = state;
    emit stateChanged(state);
}

void MMCGas::setGasUint(int uint)
{
    _gasUint = uint;
    emit uintChanged(_uintMap[uint]);
}

void MMCGas::setWarning1Value(float value)
{
//    qDebug() << "setWarning1Value" << value;
    if(value > _warning2Value)
        return;
    _warning1Value = value;
    updateWarningState();
    emit warning1ValueChanged(_warning1Value);
}
void MMCGas::setWarning2Value(float value)
{
//    qDebug() << "setWarning2Value" << value;
    if(value < _warning1Value)
        return;
    else if(value > _warning3Value)
        return;
    _warning2Value = value;
    updateWarningState();
    emit warning2ValueChanged(_warning2Value);
}
void MMCGas::setWarning3Value(float value)
{
//    qDebug() << "setWarning3Value" << value;
    if(value < _warning2Value)
        return;
    _warning3Value = value;
    updateWarningState();
    emit warning3ValueChanged(_warning3Value);
}

void MMCGas::reset(void)
{
    calculatConcentration(_gasName, _concentration);
    updateWarningState();

    emit warning1ValueChanged(_warning1Value);
    emit warning2ValueChanged(_warning2Value);
    emit warning3ValueChanged(_warning3Value);
}

void MMCGas::updateWarningState()
{
    if(_gasName == 3)
        _warningState = 0;
    else{
        if(_concentration >= _warning3Value)
            _warningState = 3;
        else if(_concentration >= _warning2Value)
            _warningState = 2;
        else if(_concentration >= _warning1Value)
            _warningState = 1;
        else
            _warningState = 0;
    }
    static int tmp=0;
    static bool firstWarning = true;
    if(_warningState != 0)
    {
        tmp++;
        if(tmp >= 10 || firstWarning)
        {
            firstWarning = false;
            tmp = 0;
            Q_UNUSED(50);
            qgcApp()->toolbox()->audioOutput()->say(QString("Warning:Detection of gas has exceeded").toLower());
        }
    }
    emit warningStateChanged(_warningState);
}

void MMCGas::calculatConcentration(int gasname, float concenteration)
{

    switch(gasname)
    {
    case 1:
        _warning1Value = 25;
        _warning2Value = 50;
        _concentration = concenteration;
        break;
    case 2:
        _warning1Value = 30;
        _warning2Value = 1700;
        _warning3Value = 2069;
        _concentration = concenteration * 28 /22.4;
        break;
    case 4:
        _concentration = concenteration * 2 /22.4;
        break;
    case 5:
        _concentration = concenteration * 16 /22.4;
        break;
    case 6:
        _concentration = concenteration * 44 /22.4;
        break;
    case 7:
        _warning1Value = (float)(1000 * 44 /22.4);
        _warning2Value = (float)(2000 * 44 /22.4);
        _warning3Value = (float)(5000 * 44 /22.4);
        _concentration = concenteration * 44 /22.4;
        break;
    case 8:
        _concentration = concenteration * 48 /22.4;
        break;
    case 9:
        _warning1Value = 10;
        _warning2Value = 430;
        _warning3Value = 618;
        _concentration = concenteration * 34 /22.4;
        break;
    case 10:
        _warning1Value = 142;
        _warning2Value = 1142;
        _warning3Value = 1428;
        _concentration = concenteration * 64 /22.4;
        break;
    case 11:
        _concentration = concenteration * 17 /22.4;
        break;
    case 12:
        _concentration = concenteration * 71 /22.4;
        break;
    case 21:
    case 22:
        _concentration = concenteration * 30 /22.4;
        break;
    case 23:
        _warning1Value = 102;
        _warning2Value = 205;
        _warning3Value = 410;
        _concentration = concenteration * 30 /22.4;
        break;
    case 37:
        _warning1Value = 40;
        _warning2Value = 9800;
        _warning3Value = 31900;
        _concentration = concenteration * 78 /22.4;
        break;
    default:
        _concentration = concenteration;
        break;
    }
    emit concentrationChange(_concentration);
}
