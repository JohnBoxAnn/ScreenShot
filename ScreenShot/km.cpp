// km.cpp : Defines the exported functions for the DLL application.
//
#pragma once
#include "stdafx.h"
#include "km.h"
#include "key.h"
#include "popmsg.h"
#include <winsvc.h>
#define _T(qoute) TEXT(qoute)

BOOL MouseMove(POINT ptTarget, USHORT Flags = MOUSE_MOVE_RELATIVE);

BOOL MouseMove(LONG dx, LONG dy, USHORT Flags = MOUSE_MOVE_RELATIVE);

BOOL KeyboardButton(USHORT VirtualKey, USHORT Flags);

BOOL MouseButton(USHORT ButtonFlags);

BOOL InstallDriver(SC_HANDLE  SchSCManager, LPCTSTR DriverName, LPCTSTR ServiceExe);

BOOL RemoveDriver(SC_HANDLE  SchSCManager, LPCTSTR DriverName);

BOOL StartDriver(SC_HANDLE SchSCManager, LPCTSTR DriverName);

BOOL StopDriver(SC_HANDLE SchSCManager, LPCTSTR DriverName);


BOOL InstallDriver(SC_HANDLE SchSCManager, LPCTSTR DriverName, LPCTSTR ServiceExe)
{
    SC_HANDLE   schService;
    DWORD       err;

    schService = CreateService(SchSCManager,           // handle of service control manager database
        DriverName,             // address of name of service to start
        DriverName,             // address of display name
        SERVICE_ALL_ACCESS,     // type of access to service
        SERVICE_KERNEL_DRIVER,  // type of service
        SERVICE_DEMAND_START,   // when to start service
        SERVICE_ERROR_NORMAL,   // severity if service fails to start
        ServiceExe,             // address of name of binary file
        NULL,                   // service does not belong to a group
        NULL,                   // no tag requested
        NULL,                   // no dependency names
        NULL,                   // use LocalSystem account
        NULL                    // no password for service account
        );

    if (schService == NULL) {

        err = GetLastError();
        if (err == ERROR_SERVICE_EXISTS)
            return TRUE;
        else {
			Poperr(_T("CreateService failed!  Error = "), err);
            return FALSE;
        }
    }


    if (schService)
        CloseServiceHandle(schService);

    return TRUE;

}   // InstallDriver

BOOL ManageDriver(LPCTSTR DriverName, USHORT Function )
{

    TCHAR       ServiceName[MAX_PATH] = {'\0'};
    SC_HANDLE   schSCManager;

    BOOL rCode = TRUE;

    if (!DriverName) {
		Poperr(_T("Invalid Driver or Service provided to ManageDriver()"));
        return FALSE;
    }

    schSCManager = OpenSCManager(NULL, NULL,  SC_MANAGER_ALL_ACCESS);

    if (!schSCManager) {
        Poperr(_T("Open SC Manager failed! Error = "), GetLastError());
        return FALSE;
    }

    switch( Function ) {

        case DRIVER_FUNC_INSTALL:
		{
			TCHAR *path = new TCHAR[MAX_PATH];
			TCHAR *driver = new TCHAR[10];
			TCHAR *dir = new TCHAR[MAX_PATH];
			TCHAR *filename = new TCHAR[50];
			TCHAR *ext = new TCHAR[10];
			GetModuleFileName(NULL, path, MAX_PATH);
			_wsplitpath_s(path, driver, 10, dir, MAX_PATH, filename, 50, ext, 10);
			_wmakepath_s(ServiceName, MAX_PATH, driver, dir, KEYMOUSE_DRIVER_NAME, L"sys");
			delete[] path, driver, dir, filename, ext;

			if (InstallDriver(schSCManager, DriverName, ServiceName))
				rCode = StartDriver(schSCManager, DriverName);
			else
				rCode = FALSE;

			break;
		}
        case DRIVER_FUNC_REMOVE:

            StopDriver(schSCManager, DriverName );
            RemoveDriver(schSCManager, DriverName );
            rCode = TRUE;
            break;

        default:
            Poperr(_T("Unknown ManageDriver() function."));
            rCode = FALSE;
            break;
    }

    if (schSCManager)
        CloseServiceHandle(schSCManager);

    return rCode;

}   // ManageDriver


BOOL RemoveDriver(SC_HANDLE SchSCManager, LPCTSTR DriverName )
{
    SC_HANDLE   schService;
    BOOL        rCode;

    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);

    if (schService == NULL) {
        Poperr(_T("OpenService failed!  Error = "), GetLastError());
        return FALSE;
    }

    if (DeleteService(schService))
        rCode = TRUE;
    else {

		Poperr(_T("DeleteService failed!  Error = "), GetLastError());
        rCode = FALSE;
    }

    if (schService)
        CloseServiceHandle(schService);

    return rCode;

}   // RemoveDriver



BOOL StartDriver(SC_HANDLE SchSCManager, LPCTSTR DriverName )
{
    SC_HANDLE   schService;
    DWORD       err;

    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);

    if (schService == NULL) {

		Poperr(_T("OpenService failed!  Error = "), GetLastError());
        return FALSE;
    }

    if (!StartService(schService, 0, NULL)) {

        err = GetLastError();
        if (err == ERROR_SERVICE_ALREADY_RUNNING)
            return TRUE;
        else {

			Poperr(_T("StartService failure! Error = "), err );
            return FALSE;
        }
    }

    if (schService)
        CloseServiceHandle(schService);

    return TRUE;

}   // StartDriver



BOOL StopDriver(SC_HANDLE SchSCManager, LPCTSTR DriverName)
{
    BOOL            rCode = TRUE;
    SC_HANDLE       schService;
    SERVICE_STATUS  serviceStatus;

    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);

    if (schService == NULL) {
		Poperr(_T("OpenService failed!  Error = "), GetLastError());
        return FALSE;
    }

    if (ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus))
        rCode = TRUE;
    else {

		Poperr(_T("ControlService failed!  Error = "), GetLastError());
        rCode = FALSE;
    }

    if (schService)
        CloseServiceHandle(schService);

    return rCode;

}   //  StopDriver


/******************************************************************
*
*   以下函数模拟键盘和鼠标操作，依赖于驱动文件kmclass.sys
*
******************************************************************/

//参数1 nScanCode，扫描码
//参数2 Flags，按键的状态（KEY_MAKE，KEY_BREAK，KEY_E0，KEY_E1）
BOOL KeyboardButton(USHORT nScanCode, USHORT Flags)
{
    KEYBOARD_INPUT_DATA  kid ;
    DWORD dwOutput;

    HANDLE hDevice = CreateFile(KEYMOUSE_WIN32_DEVICE_NAME, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
        return FALSE;

    memset(&kid, 0, sizeof(KEYBOARD_INPUT_DATA));

    kid.Flags    = Flags;
    kid.MakeCode = nScanCode;

    BOOL bRet = DeviceIoControl(hDevice, IOCTL_KEYBOARD, &kid, sizeof(KEYBOARD_INPUT_DATA), NULL, 0, &dwOutput, NULL);

    if (!bRet)
        Poperr(_T("Error! please open the VKBDservice driver!"));
    CloseHandle(hDevice);

    return bRet;
}

// 模拟按键
BOOL InputKey(USHORT nScanCode, USHORT Flags, int nSleepTime) {
	if (KeyboardButton(nScanCode, Flags | KEY_DOWN)) {
		Sleep(nSleepTime);
		if (KeyboardButton(nScanCode, Flags | KEY_UP))
			return TRUE;
	}
	return FALSE;
}

// 输入字符串
BOOL InputString(tstring str)
{
    if (str.empty())
        return TRUE;

	for (int i = 0; i < str.length(); i++) {
		switch (str[i])
		{
		case L'0':
			if (!InputKey(__Pad0, 0, 50))
				return FALSE;
			break;
		case L'1':
			if (!InputKey(__Pad1, 0, 50))
				return FALSE;
			break;
		case L'2':
			if (!InputKey(__Pad2, 0, 50))
				return FALSE;
			break;
		case L'3':
			if (!InputKey(__Pad3, 0, 50))
				return FALSE;
			break;
		case L'4':
			if (!InputKey(__Pad4, 0, 50))
				return FALSE;
			break;
		case L'5':
			if (!InputKey(__Pad5, 0, 50))
				return FALSE;
			break;
		case L'6':
			if (!InputKey(__Pad6, 0, 50))
				return FALSE;
			break;
		case L'7':
			if (!InputKey(__Pad7, 0, 50))
				return FALSE;
			break;
		case L'8':
			if (!InputKey(__Pad8, 0, 50))
				return FALSE;
			break;
		case L'9':
			if (!InputKey(__Pad9, 0, 50))
				return FALSE;
			break;
		default:
			Poperr(_T("InputString(tstring&)::不是有效字符"));
			return FALSE;
		}
	}
    return TRUE;
}



/*参数
*x,x坐标;
*y,y坐标;
*Flags,使用相对坐标(MOUSE_MOVE_RELATIVE)还是绝对坐标(MOUSE_MOVE_ABSOLUTE)
*/
BOOL MouseMove(LONG dx, LONG dy, USHORT Flags)
{
    MOUSE_INPUT_DATA  mid ;
    DWORD dwOutput;

    HANDLE hDevice = CreateFile(KEYMOUSE_WIN32_DEVICE_NAME, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
        return FALSE;

    memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));

    mid.Flags = Flags;
    switch (mid.Flags)
    {
    case MOUSE_MOVE_RELATIVE:
        mid.LastX = dx;
        mid.LastY = dy;
        break;
    case MOUSE_MOVE_ABSOLUTE:
        mid.LastX = dx * 0xffff / GetSystemMetrics(SM_CXSCREEN);
        mid.LastY = dy * 0xffff / GetSystemMetrics(SM_CYSCREEN);
        break;
    default:
        Poperr(_T("Flags: Parameter error!"));
        return FALSE;
    }

    BOOL bRet = DeviceIoControl(hDevice, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL);

    if (!bRet)
        Poperr(_T("Error! please open the VKBDservice driver!"));
    CloseHandle(hDevice);

    return bRet;
}


BOOL MouseMove(POINT ptTarget, USHORT Flags)
{
    return MouseMove(ptTarget.x, ptTarget.y, Flags);
}


//ButtonFlags,鼠标按钮状态标志
BOOL MouseButton(USHORT ButtonFlags)
{
    MOUSE_INPUT_DATA  mid;
    DWORD dwOutput;

    HANDLE hDevice = CreateFile(KEYMOUSE_WIN32_DEVICE_NAME, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
        return FALSE;

    memset(&mid, 0, sizeof(MOUSE_INPUT_DATA));

    mid.ButtonFlags = ButtonFlags;

    BOOL bRet = DeviceIoControl(hDevice, IOCTL_MOUSE, &mid, sizeof(MOUSE_INPUT_DATA), NULL, 0, &dwOutput, NULL);

    if (!bRet)
        Poperr(_T("Error! please open the VKBDservice driver!"));
    CloseHandle(hDevice);

    return bRet;
}