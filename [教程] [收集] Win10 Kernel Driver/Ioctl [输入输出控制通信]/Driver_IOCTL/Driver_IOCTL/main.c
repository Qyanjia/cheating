/*
IOCTL例子
*/
#pragma warning (disable : 4100 4995 4002)

/* 这个顺序包含,不然会报错 */
#include <ntifs.h>
#include <ntddk.h>

/* 函数声明,不然会报错 */
NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

/* 数据结构 */
struct DataStruct
{
	HANDLE PID;			//进程ID
	PVOID64 Addr;		//地址
	PVOID64 Result;		//结果
	SIZE_T Size;				//大小
};

/* 读取 */
#define IOCTL_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

/* 写入 */
#define IOCTL_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x998, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

/* 调试辅助函数 */
#define DebugMessage(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

/* 设备名称 */
UNICODE_STRING g_DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Driver_IOCTL");

/* 设备链接符号 */
UNICODE_STRING g_SymbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\Driver_IOCTL");

/* 设备对象指针 */
PDEVICE_OBJECT g_DeviceObject;

/* 读取内存 */
NTSTATUS MyReadProcessMemory(HANDLE PID, PVOID SourceAddr, PVOID TargetAddr, SIZE_T Size)
{
	PEPROCESS SourceProcess;
	NTSTATUS Status = PsLookupProcessByProcessId(PID, &SourceProcess);
	if (Status != STATUS_SUCCESS) return Status;

	SIZE_T Result;
	PEPROCESS TargetProcess = PsGetCurrentProcess();
	Status = MmCopyVirtualMemory(SourceProcess, SourceAddr, TargetProcess, TargetAddr, Size, KernelMode, &Result);
	return Status;
}

/* 写入内存 */
NTSTATUS MyWriteProcessMemory(HANDLE PID, PVOID SourceAddr, PVOID TargetAddr, SIZE_T Size)
{
	PEPROCESS SourceProcess;
	NTSTATUS Status = PsLookupProcessByProcessId(PID, &SourceProcess);
	if (Status != STATUS_SUCCESS) return Status;

	SIZE_T Result;
	PEPROCESS TargetProcess = PsGetCurrentProcess();
	Status = MmCopyVirtualMemory(TargetProcess, TargetAddr, SourceProcess, SourceAddr, Size, KernelMode, &Result);
	return Status;
}

/* 创建 */
NTSTATUS CreateFunction(PDEVICE_OBJECT device, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

/* 关闭 */
NTSTATUS CloseFunction(PDEVICE_OBJECT device, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

/* 控制 */
NTSTATUS ControlFunction(PDEVICE_OBJECT device, PIRP irp)
{
	/* 获取IRP栈位置 */
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(irp);

	/* 获取对应控制码 */
	ULONG Code = Stack->Parameters.DeviceIoControl.IoControlCode;

	/* 数据结构 */
	struct DataStruct Data;

	/* 返回状态 */
	NTSTATUS Status = STATUS_SUCCESS;

	switch (Code) //读取内存呢还是写入内存呢>
	{
	case IOCTL_READ://读取内存
	{
		/* 复制数据 */
		memcpy(&Data, irp->AssociatedIrp.SystemBuffer, sizeof(Data));

		/* 获取输出缓冲区 */
		PUCHAR Buffer = MmGetSystemAddressForMdl(irp->MdlAddress, NormalPagePriority);

		/* 读取内存 */
		if (Buffer && Data.Addr)
		{
			Status = MyReadProcessMemory(Data.PID, Data.Addr, Buffer, Data.Size);
			if (Status == STATUS_SUCCESS) KeFlushIoBuffers(irp->MdlAddress, TRUE, FALSE);
			else DebugMessage("读取发生错误 : %d ", Status);
		}
		break;
	}
	case IOCTL_WRITE://写入内存
	{
		/* 复制数据 */
		memcpy(&Data, irp->AssociatedIrp.SystemBuffer, sizeof(Data));

		/* 获取输出缓冲区 */
		PUCHAR Buffer = MmGetSystemAddressForMdl(irp->MdlAddress, NormalPagePriority);

		/* 写入内存 */
		if (Buffer && Data.Addr)
		{
			Status = MyWriteProcessMemory(Data.PID, Data.Addr, Buffer, Data.Size);
			if (Status == STATUS_SUCCESS) KeFlushIoBuffers(irp->MdlAddress, TRUE, FALSE);
			else DebugMessage("写入发生错误 : %d", Status);
		}
		break;
	}
	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

/* 驱动卸载函数 */
VOID DriverUnload(PDRIVER_OBJECT driver)
{
	/* 删除设备链接符号 */
	IoDeleteSymbolicLink(&g_SymbolicLink);

	/* 删除设备 */
	IoDeleteDevice(driver->DeviceObject);

	DebugMessage("[+] 设备卸载成功");
}

/* 这里就是驱动的入口函数 */
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg)
{
	/* 创建设备 */
	IoCreateDevice(driver, 0, &g_DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &g_DeviceObject);

	/* 创建设备链接符号 */
	IoCreateSymbolicLink(&g_SymbolicLink, &g_DeviceName);

	/* 设置相应处理函数 */
	driver->MajorFunction[IRP_MJ_CREATE] = CreateFunction;
	driver->MajorFunction[IRP_MJ_CLOSE] = CloseFunction;
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ControlFunction;

	/* 设置驱动卸载函数 */
	driver->DriverUnload = DriverUnload;

	/* 设置为直接方式读写 */
	g_DeviceObject->Flags |= DO_DIRECT_IO;

	/* 当设置DO_DEVICE_INITIALIZING标志时，I/O管理器将拒绝所有打开该设备句柄的请求以及向该设备对象上附着其他设备对象 */
	g_DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	DebugMessage("[+] 设备装载成功");
	return STATUS_SUCCESS;
}