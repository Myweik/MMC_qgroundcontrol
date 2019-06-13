#include "usbmonitor.h"
#include <stdio.h>
#include <tchar.h>
#include "lusb0_usb.h"
//#include "artosynplayer.h"

#include "libusb.h"
#include <Guiddef.h>
#include <list>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//usb detect
//////////////////////////////////////////////////////////////////////////
const _TCHAR CLASS_NAME[]  = _T("Monitor Class");

PDEV_BROADCAST_DEVICEINTERFACE onpDevInf;
WPARAM onwParam;
HWND hWnd; 
HDCOMD iHDCOMD;
#define THRD_MESSAGE_EXIT WM_USER + 1

list<GUID> monitorlist; 

GUID WceusbshGUID = { 0x36FC9E60, 0xC465, 0x11CF, { 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };

GUID GUID_DEVINTERFACE_LIST[] = 
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } },
	// GUID_DEVINTERFACE_COMPORT
	{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },
	// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
	{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },
	// GUID_DEVINTERFACE_PARALLEL
	{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },
	// GUID_DEVINTERFACE_PARCLASS
	{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};

// For informational messages and window titles
//PWSTR g_pszAppName=_T("MonitorClass");

void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam)
{
	onpDevInf = pDevInf;
	onwParam = wParam;
	//monitorthread->DoemitSingal();
	bool bat;
	if(DBT_DEVICEARRIVAL == wParam){
		bat = true;
		monitorThread->DoemitSingal(bat);
	}
	else if (DBT_DEVICEREMOVECOMPLETE == wParam){
		bat = false;
		monitorThread->DoemitSingal(bat);
	}
}
LRESULT DeviceChange(UINT message, WPARAM wParam, LPARAM lParam)
{
    if ( (DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam /*|| DBT_DEVTYP_DEVINST==wParam*/)&&(lParam!=NULL))
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;

		switch( pHdr->dbch_devicetype ) 
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			UpdateDevice(pDevInf, wParam);
			break;

		case DBT_DEVTYP_HANDLE:
			pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
			break;

		case DBT_DEVTYP_OEM:
			pDevOem = (PDEV_BROADCAST_OEM)pHdr;
			break;

		case DBT_DEVTYP_PORT:
			pDevPort = (PDEV_BROADCAST_PORT)pHdr;
			break;

		case DBT_DEVTYP_VOLUME:
			pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			break;
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		break;
	case WM_SIZE:
		break;
	case WM_DEVICECHANGE:
		return DeviceChange(message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
ATOM MyRegisterClass()
{
	WNDCLASS wc = {0};
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
	return RegisterClass(&wc);
}
bool CreateMessageOnlyWindow()
{
    hWnd = CreateWindowEx(0, CLASS_NAME, _T(""), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL,       // Parent window    
		NULL,       // Menu
		GetModuleHandle(NULL),  // Instance handle
		NULL        // Additional application data
		);

	return hWnd != NULL;
}
void RegisterDeviceNotify()
{

	HDEVNOTIFY hDevNotify;
	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	}
	for(int i =0;i<monitorlist.size();i++)
	{
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = monitorlist.back();
		monitorlist.pop_back();
		hDevNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	}
}
void Singoogle_Monitorguidlist()
{
	LPGUID guids = new GUID();
	monitorlist.insert (monitorlist.begin(), *guids);
}
DWORD  ThrdFunc( LPVOID lpParam)
{

	if (0 == MyRegisterClass())
		return -1;

	if (!CreateMessageOnlyWindow())
		return -1;
	RegisterDeviceNotify();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == THRD_MESSAGE_EXIT) return 0;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

USBMonitor::USBMonitor(QObject *parent)
	: QObject(parent)
{

}

USBMonitor::~USBMonitor()
{

}

void MonitorThread::run()
{
	ThrdFunc(NULL);
}

void MonitorThread::DoemitSingal(bool arrival)
{
	emit sig_USBUpdate(arrival);
}
