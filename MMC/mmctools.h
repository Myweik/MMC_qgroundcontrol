#ifndef MMCTOOLS_H
#define MMCTOOLS_H
#include <QObject>

namespace MMC
{

    unsigned char _crc8(unsigned char *ptr, unsigned char len);
    quint32 _crc32(const quint8 *src, unsigned len, unsigned state);
    unsigned char _xor8(unsigned char *ptr, unsigned char len);

}

#endif // MMCTOOLS_H
