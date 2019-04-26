#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H

#include <QAbstractVideoBuffer>

class VideoBuffer : public QAbstractVideoBuffer
{
public:
    VideoBuffer(uchar *data, int numBytes, int bytesPerLine);

    virtual uchar *map(QAbstractVideoBuffer::MapMode mode, int *numBytes,
                       int *bytesPerLine) override;

    virtual QAbstractVideoBuffer::MapMode mapMode() const override;

    virtual void unmap() override;

    void setMap(uchar *data, int numBytes, int bytesPerLine) {
        m_data = data;
        m_numBytes = numBytes;
        m_bytesPerLine = bytesPerLine;
    }

private:
    uchar *m_data;
    int m_numBytes;
    int m_bytesPerLine;
};

#endif // VIDEOBUFFER_H
