#pragma once
#ifndef _KEY_H_
#define _KEY_H_

// ScanCode

#define __Backspace		0x0E
#define __Capslock		0x3A
#define __Enter			0x1C
#define __Esc			0x01
#define __Alt			0x38
#define __Ctrl			0x1D
#define __LShift		0x2A
#define __Numlock		0x45
#define __RShift		0x36
#define __Scrolllock	0x46
#define __Space			0x39
#define __Sysreq		0x54
#define __Tab			0x0F
#define __F1			0x3B
#define __F2			0x3C
#define __F3			0x3D
#define __F4			0x3E
#define __F5			0x3F
#define __F6			0x40
#define __F7			0x41
#define __F8			0x42
#define __F9			0x43
#define __F10			0x44
#define __F11			0x57
#define __F12			0x58
#define __A				0x1E
#define __B				0x30
#define __C				0x2E
#define __D				0x20
#define __E				0x12
#define __F				0x21
#define __G				0x22
#define __H				0x23
#define __I				0x17
#define __J				0x24
#define __K				0x25
#define __L				0x26
#define __M				0x32
#define __N				0x31
#define __O				0x18
#define __P				0x19
#define __Q				0x10
#define __R				0x13
#define __S				0x1F
#define __T				0x14	// 可能与MFC或ATL宏定义冲突
#define __U				0x16
#define __V				0x2F
#define __W				0x11
#define __X				0x2D
#define __Y				0x15
#define __Z				0x2C
#define __1				0x02
#define __2				0x03
#define __3				0x04
#define __4				0x05
#define __5				0x06
#define __6				0x07
#define __7				0x08
#define __8				0x09
#define __9				0x0A
#define __0				0x0B
#define __Pad0			0x52
#define __Pad1			0x4F
#define __Pad2			0x50
#define __Pad3			0x51
#define __Pad4			0x4B
#define __Pad5			0x4C
#define __Pad6			0x4D
#define __Pad7			0x47
#define __Pad8			0x48
#define __Pad9			0x49

// Extended(plus 0xE0 in the front)

#define __Ins			0x52
#define __End			0x4F
#define __Down			0x50
#define __PgDn			0x51
#define __Left			0x4B
#define __Right			0x4D
#define __Home			0x47
#define __Up			0x48
#define __PgUp			0x49
#define __Del			0x53
#define __PrtSc			0x37
#endif
