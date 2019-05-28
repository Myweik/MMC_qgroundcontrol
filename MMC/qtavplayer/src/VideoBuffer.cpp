#include "VideoBuffer.h"
#include <QDebug>

VideoBuffer::VideoBuffer(uchar *data, int numBytes, int bytesPerLine)
    : QAbstractVideoBuffer(QAbstractVideoBuffer::NoHandle)
    , m_data(data)
    , m_numBytes(numBytes)
    , m_bytesPerLine(bytesPerLine)
{

}

uchar *VideoBuffer::map(QAbstractVideoBuffer::MapMode mode, int *numBytes,
                        int *bytesPerLine)
{
    Q_UNUSED(mode)

    *numBytes = m_numBytes;
    *bytesPerLine = m_bytesPerLine;

    return m_data;
}

QAbstractVideoBuffer::MapMode VideoBuffer::mapMode() const
{
    return QAbstractVideoBuffer::ReadOnly;
}

void VideoBuffer::unmap()
{
    //delete m_data;
}
