#ifndef FIFO_H
#define FIFO_H
//#include <sdkddkver.h>
//#include <windows.h>
//#include <QtCore>
//#include <atlstr.h> // CString
//#include"qlist.h"
//#include <QTextCodec>
//#include <tchar.h>
//#include <dbt.h>
#include <iostream>
//#include <setupapi.h>
#include <QString>
//#include "Singoogle.h"

#include"qthread.h"
#include <QMutex>
#include <QReadWriteLock>
#include <QQueue>
using namespace std;
//#define FIFLODEEPLENGTH 500*1024
#define FIFLODEEPLENGTH 10*1024*1024


//memcpy(data, d->m_read_buf.data(), static_cast<ulong>(read_size));
//d->m_read_buf = d->m_read_buf.mid(static_cast<int>(read_size));

//handler->m_read_buf_mutex.lock();
//const int previous_size = handler->m_read_buf.size();
//handler->m_read_buf.resize(previous_size + received);
//memcpy(handler->m_read_buf.data() + previous_size, transfer->buffer, static_cast<ulong>(received));
//handler->m_read_buf_mutex.unlock();

class BuffQueue{
public :
    BuffQueue(){init();}

    QQueue<QByteArray *> buffs;
private :
    QReadWriteLock mutex;
public :
    void init(){
        release();
    }

    void put(QByteArray *buff){
        if(buff == NULL)
            return;
        mutex.lockForWrite();
        buffs.push_back(buff);
        mutex.unlock();
    }

    QByteArray *get(){
        QByteArray *buff = NULL;
        mutex.lockForWrite();
        if(buffs.size() > 0){
            buff = buffs.front();
            buffs.pop_front();
        }
        mutex.unlock();
        return buff;
    }

    int size(){
        mutex.lockForRead();
        int len = buffs.size();
        mutex.unlock();
        return len;
    }

    void release(){
        mutex.lockForWrite();
        QQueue<QByteArray *>::iterator begin = buffs.begin();
        QQueue<QByteArray *>::iterator end = buffs.end();
        while(begin != end){
            QByteArray *buff = *begin;
            if(buff != NULL){
                buff->clear();
                delete buff;
            }
            buffs.pop_front();
            begin = buffs.begin();
        }
        mutex.unlock();
    }
};


class usbfifo
{
public:
		usbfifo(void);
	virtual ~usbfifo(void);
public:
//    QByteArray _readBuf;

    BuffQueue* _buffQurue = nullptr;
    QMutex _usbByteMutex;

    void write(unsigned char*,int);
    int  read(unsigned char*);
    void clear();
    int  size();
	void initialfifo();
public:	signals:

protected:
	void run();
};
extern usbfifo *g_fifo;

#endif //FIFO_H
