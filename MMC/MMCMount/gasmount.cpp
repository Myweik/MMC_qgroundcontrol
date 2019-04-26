#include "gasmount.h"
#include "QGCApplication.h"
#include "Vehicle.h"

GasMount::GasMount(Vehicle* vehicle)
        : MountInfo(vehicle)
{
    QString dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "/MMC Station/DetectionReport/";
    QDir tmpDir;
    if(!tmpDir.mkpath(dir))
        return;
    QString fileName = dir + QDateTime::currentDateTime().toString("yyyy_MM_dd_hhmmss") + ".mmcgas";
    _gas_file = new QFile(fileName);
    if(!_gas_file->open(QIODevice::ReadWrite))
        qDebug() << "open file error";
}

void GasMount::setState(int value)
{
    if(_state == value)
        return;
    _state = value;
    emit stateChanged();
}

void GasMount::setGasList(QList<gas_info> gasList)
{
    for(int i=0; i < gasList.count(); i++)
    {
        int j=0;
        for(j; j < _gasList.count(); j++)
        {
            MMCGas *tmpGas = qobject_cast<MMCGas*> (_gasList.get(j));
            if(tmpGas->gasName() == (int)gasList.at(i).name)
            {
                tmpGas->setConcentration(gasList.at(i).concentration);
                tmpGas->setGasState((int)gasList.at(i).state);
                tmpGas->setGasUint((int)gasList.at(i).unit);
                break;
            }
        }
        if(j == _gasList.count())
        {
            MMCGas *gas = new MMCGas(gasList.at(i).concentration, (int)gasList.at(i).name, (int)gasList.at(i).state, (int)gasList.at(i).unit);
            _gasList.append(gas);
        }
    }
    emit gasListChanged();

    if(_gas_file && _gas_file->isOpen()){
        QTextStream in(_gas_file);
        in  << QDateTime::currentDateTime().toString("yyyy_MM_dd_hh:mm:ss")
            << "," << QString::number(_vehicle->latitude(), 'f', 8)
            << "," << QString::number(_vehicle->longitude(), 'f', 8);
        for(int i=0; i < _gasList.count(); i++)
        {
            MMCGas *gas = qobject_cast<MMCGas*>(_gasList.get(i));
            in << "," << gas->name()
               << "," << gas->concentration()
               << "," << gas->uintName();
        }
        in << "\r\n";
    }
}

void GasMount::controlPitch(int offset, bool plus)
{
    pitchControl(offset, plus);
}

void GasMount::pitchControl(int offset, bool plus)
{
    uint8_t buff[5] = {0xA5,4,3,0,0};
    if(plus)
        buff[3] = offset;
    else
        buff[3] = -offset;
    qDebug() << "--------------------------GasMount::pitchControl" << offset << plus;
    sendData(buff, buff[2] + 2);
}

void GasMount::handleInfo(const can_data &data)
{
    uint8_t tmpData[255]={0};
    for(int i=0; i < data.pack_len; i++)
    {
        if(0 < mountPacket(data.date[i], &tmpData[0]))
        {
            switch(tmpData[0])
            {
            case 1: //版本信息
                parseGasInfo(&tmpData[0], tmpData[1]);
                break;
            case 2: //云台角度反馈
                parseConcentration(&tmpData[0], tmpData[1]);
                break;
            case MOUNT_UPDATA_TYPE::MOUNT_UP_REQUESTTIME:
                sendTime();
                break;
            case MOUNT_UPDATA_TYPE::MOUNT_UP_REQUESTID:
                parseMountIDAck(&tmpData[0], tmpData[1]);
                break;
            }
        }
    }
}

void GasMount::saveJson(QJsonObject &dataJson)
{
    MountInfo::saveJson(dataJson);
    dataJson.insert("cmdState", state());          //挂载上行ID
    QJsonArray arrayJson;
    for(int i = 0; i < _gasList.count(); i++)
    {
        QJsonObject itemJson;
        MMCGas *tmpGas = qobject_cast<MMCGas*> (_gasList.get(i));
        itemJson.insert("cmdItemName", tmpGas->name());
        itemJson.insert("cmdItemConcentration", tmpGas->concentration());
        itemJson.insert("cmdItemState", tmpGas->state());
        itemJson.insert("cmdItemUnit", tmpGas->uint());
        arrayJson.append(itemJson);
    }
    dataJson.insert("cmdGasArray", arrayJson);
}

void GasMount::parseGasInfo(const uint8_t *ptr, int length)
{
    if(length != 8)
        return;
    parseVersion(ptr+2); //版本信息
    this->setState(*(ptr+6));
}

void GasMount::parseConcentration(const uint8_t *ptr, int length)
{
    QList<gas_info> tmpList;
    for(int i=2; i < length;)
    {
        gas_info tmpGas;
        memcpy(&tmpGas, ptr+i, sizeof(tmpGas));
        tmpList.append(tmpGas);
        i += 8;
    }
    this->setGasList(tmpList);
//    if(_webServerConnected)
//        wGasState(gasMount);//更新web数据状态
}
