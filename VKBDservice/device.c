#include "stdafx.h"
#include "device.h"


typedef struct _DEVICE_EXTENSION {

	PDEVICE_OBJECT       kbdDeviceObject;        //键盘类设备对象
	PDEVICE_OBJECT       mouDeviceObject;        //鼠标类设备对象
	MY_KEYBOARDCALLBACK  My_KbdCallback;         //KeyboardClassServiceCallback函数 
	MY_MOUSECALLBACK     My_MouCallback;         //MouseClassServiceCallback函数

}DEVICE_EXTENSION, *PDEVICE_EXTENSION;


void VKBDserviceUnload(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS VKBDserviceCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS VKBDserviceDefaultHandler(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS VKBDserviceDispatchDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS GetVKBDserviceInfo(PDEVICE_OBJECT DeviceObject, USHORT Index);
DRIVER_INITIALIZE DriverEntry;


//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"
#endif
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING  RegistryPath)
{
	UNICODE_STRING     DeviceName, Win32Device;
	PDEVICE_OBJECT     DeviceObject = NULL;
	NTSTATUS           status;


	RtlInitUnicodeString(&DeviceName, KEYMOUSE_DEVICE_NAME);
	RtlInitUnicodeString(&Win32Device, KEYMOUSE_DOS_DEVICE_NAME);

	for (ULONGLONG i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = VKBDserviceDefaultHandler;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = VKBDserviceCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = VKBDserviceCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = VKBDserviceDispatchDeviceControl;

	DriverObject->DriverUnload = VKBDserviceUnload;
	status = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION), &DeviceName, FILE_DEVICE_KEYMOUSE, 0, TRUE, &DeviceObject);

	if (!NT_SUCCESS(status))
		return status;
	if (!DeviceObject)
		return STATUS_UNEXPECTED_IO_ERROR;

	DeviceObject->Flags |= DO_DIRECT_IO;
	DeviceObject->AlignmentRequirement = FILE_WORD_ALIGNMENT;
	status = IoCreateSymbolicLink(&Win32Device, &DeviceName);

	status = GetVKBDserviceInfo(DeviceObject, KEYBOARD_DEVICE);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("KEYBOARD_DEVICE ERROR, error = 0x%08lx\n", status));
		return status;
	}

	status = GetVKBDserviceInfo(DeviceObject, MOUSE_DEVICE);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("MOUSE_DEVICE ERROR, error = 0x%08lx\n", status));
		return status;
	}

	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

void VKBDserviceUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING Win32Device;
	RtlInitUnicodeString(&Win32Device, KEYMOUSE_DOS_DEVICE_NAME);
	IoDeleteSymbolicLink(&Win32Device);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS VKBDserviceCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS VKBDserviceDefaultHandler(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}

NTSTATUS VKBDserviceDispatchDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	NTSTATUS				status = STATUS_SUCCESS;
	PIO_STACK_LOCATION		irpStack;
	PDEVICE_EXTENSION		deviceExtension;
	ULONG					ioControlCode;
	PKEYBOARD_INPUT_DATA	KbdInputDataStart, KbdInputDataEnd;
	PMOUSE_INPUT_DATA		MouseInputDataStart, MouseInputDataEnd;
	ULONG					InputDataConsumed;
	PVOID					ioBuffer;

	irpStack = IoGetCurrentIrpStackLocation(Irp);
	deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

	ioBuffer = Irp->AssociatedIrp.SystemBuffer;
	ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
	Irp->IoStatus.Information = 0;

	switch (ioControlCode)
	{
	case IOCTL_KEYBOARD:
		if (ioBuffer)
		{
			KEYBOARD_INPUT_DATA kid = *(PKEYBOARD_INPUT_DATA)ioBuffer;
			KbdInputDataStart = &kid;
			KbdInputDataEnd = KbdInputDataStart + 1;
			deviceExtension->My_KbdCallback(deviceExtension->kbdDeviceObject,
				KbdInputDataStart,
				KbdInputDataEnd,
				&InputDataConsumed);

			status = STATUS_SUCCESS;
		}
		break;

	case IOCTL_MOUSE:
		if (ioBuffer)
		{
			MOUSE_INPUT_DATA mid = *(PMOUSE_INPUT_DATA)ioBuffer;
			MouseInputDataStart = &mid;
			MouseInputDataEnd = MouseInputDataStart + 1;

			deviceExtension->My_MouCallback(deviceExtension->mouDeviceObject,
				MouseInputDataStart,
				MouseInputDataEnd,
				&InputDataConsumed);

			status = STATUS_SUCCESS;
		}
		break;

	default:
		status = STATUS_INVALID_PARAMETER;
		KdPrint(("\nunknown IRP_MJ_DEVICE_CONTROL\n"));
		break;
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}


NTSTATUS GetVKBDserviceInfo(PDEVICE_OBJECT DeviceObject, USHORT Index)
{
	NTSTATUS           status;
	UNICODE_STRING     ObjectName;
	PCWSTR             kmhidName, kmclassName, kmName;
	PVOID              kmDriverStart;
	ULONG              kmDriverSize;
	PVOID*             TargetDeviceObject;
	PVOID*             TargetclassCallback;
	PDEVICE_EXTENSION  deviceExtension;
	PDRIVER_OBJECT     kmDriverObject = NULL;
	PDRIVER_OBJECT     kmclassDriverObject = NULL;


	deviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

	switch (Index)
	{
	case KEYBOARD_DEVICE:
		kmName = L"kbd";
		kmhidName = L"\\Driver\\kbdhid";
		kmclassName = L"\\Driver\\kbdclass";
		TargetDeviceObject = (PVOID*)&(deviceExtension->kbdDeviceObject);
		TargetclassCallback = (PVOID*)&(deviceExtension->My_KbdCallback);
		break;
	case MOUSE_DEVICE:
		kmName = L"mou";
		kmhidName = L"\\Driver\\mouhid";
		kmclassName = L"\\Driver\\mouclass";
		TargetDeviceObject = (PVOID*)&(deviceExtension->mouDeviceObject);
		TargetclassCallback = (PVOID*)&(deviceExtension->My_MouCallback);
		break;
	default:
		return STATUS_INVALID_PARAMETER;
	}


	// 通过USB类设备获取驱动对象
	RtlInitUnicodeString(&ObjectName, kmhidName);
	status = ObReferenceObjectByName(
		&ObjectName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		FILE_READ_ACCESS,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&kmDriverObject);


	if (!NT_SUCCESS(status))
	{
		// 通过i8042prt获取驱动对象
		RtlInitUnicodeString(&ObjectName, L"\\Driver\\i8042prt");
		status = ObReferenceObjectByName(&ObjectName,
			OBJ_CASE_INSENSITIVE,
			NULL,
			FILE_READ_ACCESS,
			*IoDriverObjectType,
			KernelMode,
			NULL,
			(PVOID*)&kmDriverObject);
		if (!NT_SUCCESS(status))
		{
			KdPrint(("Couldn't Get the i8042prt Driver Object\n"));
			return status;
		}
		else
		{
			ObDereferenceObject(kmDriverObject);
		}
	}
	else
	{
		ObDereferenceObject(kmDriverObject);
	}

	// 通过kmclass获取键盘鼠标类驱动对象
	RtlInitUnicodeString(&ObjectName, kmclassName);
	status = ObReferenceObjectByName(&ObjectName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		FILE_READ_ACCESS,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&kmclassDriverObject);

	if (!NT_SUCCESS(status))
	{
		KdPrint(("Couldn't Get the kmclass Driver Object\n"));
		return status;
	}
	else
	{
		kmDriverStart = kmclassDriverObject->DriverStart;
		kmDriverSize = kmclassDriverObject->DriverSize;
		ObDereferenceObject(kmclassDriverObject);
	}

	ULONG_PTR		DeviceExtensionSize;
	PULONG_PTR		kmDeviceExtension;
	PDEVICE_OBJECT	kmTempDeviceObject;
	PDEVICE_OBJECT	kmclassDeviceObject;
	PDEVICE_OBJECT	kmDeviceObject = kmDriverObject->DeviceObject;
	while (kmDeviceObject)
	{
		kmTempDeviceObject = kmDeviceObject;
		while (kmTempDeviceObject)
		{
			kmDeviceExtension = (PULONG_PTR)kmTempDeviceObject->DeviceExtension;
			kmclassDeviceObject = kmclassDriverObject->DeviceObject;
			DeviceExtensionSize = ((ULONG_PTR)kmTempDeviceObject->DeviceObjectExtension - (ULONG_PTR)kmTempDeviceObject->DeviceExtension) / 4;
			while (kmclassDeviceObject)
			{
				for (ULONG_PTR i = 0; i < DeviceExtensionSize; i++)
				{
					if (kmDeviceExtension[i] == (ULONG_PTR)kmclassDeviceObject &&
						kmDeviceExtension[i + 1] >(ULONG_PTR)kmDriverStart    &&
						kmDeviceExtension[i + 1] < (ULONG_PTR)kmDriverStart + kmDriverSize)
					{
						// 将获取到的设备对象保存到自定义扩展设备结构
						*TargetDeviceObject = (PVOID)kmDeviceExtension[i];
						*TargetclassCallback = (PVOID)kmDeviceExtension[i + 1];
						KdPrint(("%SDeviceObject == 0x%x\n", kmName, kmDeviceExtension[i]));
						KdPrint(("%SClassServiceCallback == 0x%x\n", kmName, kmDeviceExtension[i + 1]));
						return STATUS_SUCCESS;
					}
				}
				kmclassDeviceObject = kmclassDeviceObject->NextDevice;
			}
			kmTempDeviceObject = kmTempDeviceObject->AttachedDevice;
		}
		kmDeviceObject = kmDeviceObject->NextDevice;
	}
	return STATUS_UNSUCCESSFUL;
}
