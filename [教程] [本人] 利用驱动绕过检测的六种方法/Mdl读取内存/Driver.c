#include "Driver.h"

// {F90B1129-715C-4F84-A069-FEE12E2AFB48}
UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\{F90B1129-715C-4F84-A069-FEE12E2AFB48}");
UNICODE_STRING DeviceLink = RTL_CONSTANT_STRING(L"\\??\\{F90B1129-715C-4F84-A069-FEE12E2AFB48}");

//读取内存
VOID MdlReadProcessMemory(PUserData Buffer)
{
	//打开目标进程
	PEPROCESS Process = NULL;
	NTSTATUS Status = PsLookupProcessByProcessId((HANDLE)Buffer->Pid, &Process);
	if (!NT_SUCCESS(Status))
	{
		printfs("[Mdl] : read PsLookupProcessByProcessId函数失败");
		return;
	}

	//申请内存空间
	PBYTE Temp = ExAllocatePool(PagedPool, Buffer->Size);
	if (Temp == NULL)
	{
		printfs("[Mdl] : read ExAllocatePool函数失败");
		ObDereferenceObject(Process);
		return;
	}

	//附加进程
	KAPC_STATE Stack = { 0 };
	KeStackAttachProcess(Process, &Stack);

	//对齐内存
	ProbeForRead((PVOID)Buffer->Address, Buffer->Size, 1);

	//复制内存
	RtlCopyMemory(Temp, (PVOID)Buffer->Address, Buffer->Size);

	//解除引用
	ObDereferenceObject(Process);

	//结束附加
	KeUnstackDetachProcess(&Stack);

	//复制到我们的缓冲区
	RtlCopyMemory(Buffer->Data, Temp, Buffer->Size);

	//释放内存
	ExFreePool(Temp);
}

//写入内存
VOID MdlWriteProcessMemory(PUserData Buffer)
{
	//打开目标进程
	PEPROCESS Process = NULL;
	NTSTATUS Status = PsLookupProcessByProcessId((HANDLE)Buffer->Pid, &Process);
	if (!NT_SUCCESS(Status))
	{
		printfs("[Mdl] : write PsLookupProcessByProcessId函数失败");
		return;
	}

	//申请内存空间
	PBYTE Temp = ExAllocatePool(PagedPool, Buffer->Size);
	if (Temp == NULL)
	{
		printfs("[Mdl] : write ExAllocatePool函数失败");
		ObDereferenceObject(Process);
		return;
	}

	//复制内存数据
	for (DWORD i = 0; i < Buffer->Size; i++) Temp[i] = Buffer->Data[i];

	//附加进程
	KAPC_STATE Stack = { 0 };
	KeStackAttachProcess(Process, &Stack);

	//申请MDL
	PMDL Mdl = IoAllocateMdl((PVOID)Buffer->Address, Buffer->Size, FALSE, FALSE, NULL);
	if (Mdl == NULL)
	{
		printfs("[Mdl] : IoAllocateMdl函数失败");
		KeUnstackDetachProcess(&Stack);
		ExFreePool(Temp);
		ObDereferenceObject(Process);
		return;
	}

	//建设物理页面
	MmBuildMdlForNonPagedPool(Mdl);

	//锁定页面
	PBYTE ChangeData = MmMapLockedPages(Mdl, KernelMode);

	//复制内存
	if (ChangeData) RtlCopyMemory(ChangeData, Temp, Buffer->Size);

	//释放数据
	IoFreeMdl(Mdl);
	ExFreePool(Temp);
	KeUnstackDetachProcess(&Stack);
	ObDereferenceObject(Process);
}

//驱动派遣函数
NTSTATUS DriverIoctl(PDEVICE_OBJECT Device, PIRP pirp)
{
	//未引用
	UNREFERENCED_PARAMETER(Device);

	//获取堆栈
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(pirp);

	//获取控制码
	ULONG Code = Stack->Parameters.DeviceIoControl.IoControlCode;

	if (Stack->MajorFunction == IRP_MJ_DEVICE_CONTROL)
	{
		//获取数据指针
		PUserData Buffer = pirp->AssociatedIrp.SystemBuffer;
		printfs("[Mdl] : PID:%d  地址:%x  大小:%d", Buffer->Pid, Buffer->Address, Buffer->Size);

		if (Code == Mdl_Read) MdlReadProcessMemory(Buffer); //读取内存
		if (Code == Mdl_Write) MdlWriteProcessMemory(Buffer);//写入内存

		pirp->IoStatus.Information = sizeof(UserData);
	}
	else pirp->IoStatus.Information = 0;

	//完成IO
	pirp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

//驱动卸载函数
VOID DriverUnload(PDRIVER_OBJECT object)
{
	if (object->DeviceObject)
	{
		IoDeleteSymbolicLink(&DeviceLink);
		IoDeleteDevice(object->DeviceObject);
	}
	printfs("[Mdl] : 驱动卸载成功");
}

//驱动入口函数
NTSTATUS DriverEntry(PDRIVER_OBJECT object, PUNICODE_STRING reg)
{
	printfs("[Mdl] : 驱动注册表 -> %wZ", reg);

	//设置卸载函数
	object->DriverUnload = DriverUnload;

	//创建设备
	PDEVICE_OBJECT Device = NULL;
	NTSTATUS Status = IoCreateDevice(object, sizeof(object->DriverExtension), &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &Device);
	if (!NT_SUCCESS(Status))
	{
		printfs("[Mdl] : IoCreateDevice函数失败");
		return Status;
	}

	//创建链接
	Status = IoCreateSymbolicLink(&DeviceLink, &DeviceName);
	if (!NT_SUCCESS(Status))
	{
		printfs("[Mdl] : IoCreateSymbolicLink函数失败");
		IoDeleteDevice(Device);
		return Status;
	}

	//设置派遣函数
	object->MajorFunction[IRP_MJ_CREATE] = DriverIoctl;
	object->MajorFunction[IRP_MJ_CLOSE] = DriverIoctl;
	object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverIoctl;

	printfs("[Mdl] : 驱动加载成功");
	return STATUS_SUCCESS;
}