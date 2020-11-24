#include "HideDriver.h"

//获取Win10系统的MiProcessLoaderEntry函数地址
MiProcessLoaderEntry GetMiProcessLoaderEntryAddress_Win10()
{
	//查找方式
	//MmUnloadSystemImage -> MiUnloadSystemImage -> MiProcessLoaderEntry

	//特征码
	CHAR MmUnloadSystemImage_Code[] = "\x83\xCA\xFF"				//or        edx, 0FFFFFFFFh
		"\x48\x8B\xCF"																			//mov     rcx, rdi
		"\x48\x8B\xD8"																			//mov     rbx, rax
		"\xE8";																							//call       *******

	//获取MmUnloadSystemImage函数地址,做为搜索的开始地址
	ULONG_PTR MmUnloadSystemImageAddress = 0;
	{
		UNICODE_STRING FuncName;
		RtlInitUnicodeString(&FuncName, L"MmUnloadSystemImage");
		MmUnloadSystemImageAddress = (ULONG_PTR)MmGetSystemRoutineAddress(&FuncName);
		if (MmUnloadSystemImageAddress == 0)
		{
			printfs("[Hide] : 查找MmUnloadSystemImage函数地址失败");
			return NULL;
		}
	}

	//设置搜索的开始位置和结束位置
	ULONG_PTR StartAddress = MmUnloadSystemImageAddress;
	ULONG_PTR StopAddress = MmUnloadSystemImageAddress + 0x500;

	ULONG_PTR MiUnloadSystemImageAddress = 0;

	//开始搜索MiUnloadSystemImage的地址
	while (StartAddress < StopAddress)
	{
		if (memcmp((VOID*)StartAddress, MmUnloadSystemImage_Code, strlen(MmUnloadSystemImage_Code)) == 0)
		{
			StartAddress += strlen(MmUnloadSystemImage_Code);
			MiUnloadSystemImageAddress = *(LONG*)StartAddress + StartAddress + 4;
			break;
		}
		++StartAddress;
	}

	if (MiUnloadSystemImageAddress == 0)
	{
		printfs("[Hide] : 查找MiUnloadSystemImage函数地址失败");
		return NULL;
	}

	//再次设置搜索的开始位置和结束位置
	StartAddress = MiUnloadSystemImageAddress;
	StopAddress = MiUnloadSystemImageAddress + 0x600;

	MiProcessLoaderEntry f_MiProcessLoaderEntry = 0;

	//开始搜索MiProcessLoaderEntry函数地址
	while (StartAddress < StopAddress)
	{
		if (*(UCHAR*)StartAddress == 0xE8 &&												//call
			*(UCHAR *)(StartAddress + 5) == 0x8B && *(UCHAR *)(StartAddress + 6) == 0x05)	//mov eax,
		{
			StartAddress++;	//跳过call的0xE8
			f_MiProcessLoaderEntry = (MiProcessLoaderEntry)(*(LONG*)StartAddress + StartAddress + 4);
			break;
		}
		++StartAddress;
	}

	return f_MiProcessLoaderEntry;
}

//获取MiProcessLoaderEntry函数地址
MiProcessLoaderEntry GetMiProcessLoaderEntryAddress()
{
	//函数地址
	MiProcessLoaderEntry f_MiProcessLoaderEntry = NULL;

	//操作状态
	NTSTATUS Status = STATUS_SUCCESS;

	//获取当前系统版本
	RTL_OSVERSIONINFOEXW Version;
	RtlZeroMemory(&Version, sizeof(Version));
	Version.dwOSVersionInfoSize = sizeof(Version);
	Status = RtlGetVersion(&Version);
	if (!NT_SUCCESS(Status))
	{
		printfs("[Hide] : 获取系统版本失败");
		return f_MiProcessLoaderEntry;
	}

	if (Version.dwMajorVersion == 10)
	{
		printfs("[Hide] : Win10系统");
		f_MiProcessLoaderEntry = GetMiProcessLoaderEntryAddress_Win10();
	}
	else if (Version.dwMajorVersion == 6 && Version.dwMinorVersion == 3)
	{
		printfs("[Hide] : Win8.1系统");
	}
	else if (Version.dwMajorVersion == 6 && Version.dwMinorVersion == 2 && Version.wProductType == VER_NT_WORKSTATION)
	{
		printfs("[Hide] : Win8系统");
	}
	else if (Version.dwMajorVersion == 6 && Version.dwMinorVersion == 1 && Version.wProductType == VER_NT_WORKSTATION)
	{
		printfs("[Hide] : Win7系统");
	}
	else printfs("[Hide] : 未知系统");

	return f_MiProcessLoaderEntry;
}

//重新初始化函数
VOID Reinitialize(PDRIVER_OBJECT DriverObject, PVOID Context, ULONG Count)
{
	//为引用
	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Count);

	//先获取MiProcessLoaderEntry函数的地址
	MiProcessLoaderEntry f_MiProcessLoaderEntry = GetMiProcessLoaderEntryAddress();
	if (f_MiProcessLoaderEntry == 0)
	{
		printfs("[Hide] : MiProcessLoaderEntry函数地址获取失败");
		return;
	}
	printfs("[Hide] : MiProcessLoaderEntry ->  %8x", f_MiProcessLoaderEntry);

	//因为驱动从链表上摘除之后就不再支持SEH了
	//驱动的SEH分发是根据从链表上获取驱动地址，判断异常的地址是否在该驱动中
	//因为链表上没了，就会出问题
	//学习（抄袭）到的方法是用别人的驱动对象改他链表上的地址
	{
		//获取Beep设备
		PDRIVER_OBJECT BeepObject = NULL;
		UNICODE_STRING FuncName;
		RtlInitUnicodeString(&FuncName, L"\\Driver\\beep");
		NTSTATUS Status = ObReferenceObjectByName(&FuncName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &BeepObject);
		if (!NT_SUCCESS(Status))
		{
			printfs("[Hide] : 获取Beep替死对象失败");
			return;
		}

		//替死替换
		PLDR_DATA_TABLE_ENTRY LdrEntry = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
		LdrEntry->DllBase = BeepObject->DriverStart;

		ObDereferenceObject(BeepObject);
	}

	//隐藏自己
	f_MiProcessLoaderEntry(DriverObject->DriverSection, 0);

	//链表操作
	{
		PLDR_DATA_TABLE_ENTRY ldr = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
		InitializeListHead(&ldr->InLoadOrderLinks);
		InitializeListHead(&ldr->InMemoryOrderLinks);
	}

	//破坏特征
	DriverObject->DriverSection = NULL;
	DriverObject->DriverStart = NULL;
	DriverObject->DriverSize = 0;
	DriverObject->DriverUnload = NULL;
	DriverObject->DriverInit = NULL;
	DriverObject->DeviceObject = NULL;

	printfs("[Hide] : 驱动隐藏成功");
}

//驱动入口
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegString)
{
	//未引用
	UNREFERENCED_PARAMETER(RegString);

	//重新初始化
	IoRegisterDriverReinitialization(DriverObject, Reinitialize, NULL);

	printfs("[Hide] : 驱动加载成功");
	return STATUS_SUCCESS;
}