#ifndef USBMONITOR_H
#define USBMONITOR_H

#include <QObject>
#include <QThread>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <dbt.h>

typedef struct HDCOMD
{
	int	ADDREM;//1:ADD; 0:REMOVE
	QString		TYPE;
	QString 	VID;
	QString 	PID;
	QString		DEVID;
	QString		DISP;
}	HDCOMD,		*PHDCOMD;

extern PDEV_BROADCAST_DEVICEINTERFACE onpDevInf;
extern WPARAM onwParam;
extern HDCOMD iHDCOMD;

class USBMonitor : public QObject
{
	Q_OBJECT

public:
	USBMonitor(QObject *parent);
	~USBMonitor();

private:
	
};

class MonitorThread : public QThread
{
	Q_OBJECT
public: 
	int Stop();
	void  DoemitSingal(bool arrival);
public:
signals:
	void requestInput();
	void sig_USBUpdate(bool arrival);
protected:
	void run();
};

#endif // USBMONITOR_H
