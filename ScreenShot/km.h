#ifndef _KM_H_
#define _KM_H_

#pragma once
#include "..\VKBDservice\defination.h"


#define DRIVER_FUNC_INSTALL     0x01
#define DRIVER_FUNC_REMOVE      0x02

BOOL InputKey(USHORT nScanCode, USHORT Flags, int nSleepTime = 0);

BOOL InputString(tstring str);

BOOL ManageDriver(LPCTSTR DriverName, USHORT Function);

#endif // _KM_H_