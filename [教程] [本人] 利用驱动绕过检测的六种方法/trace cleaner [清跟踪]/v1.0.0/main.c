#include "main.h"

PVOID64 get_kernel_mode_base(PULONG64 size)
{
	static PVOID64 kernel_addr = NULL;
	static ULONG64 kernel_size = 0;

	// 直接返回
	if (kernel_addr && kernel_size)
	{
		*size = kernel_size;
		return kernel_addr;
	}

	// 查找NtOpenFile函数的地址
	UNICODE_STRING routineName = RTL_CONSTANT_STRING(L"NtOpenFile");
	PVOID64 check_ptr = MmGetSystemRoutineAddress(&routineName);
	if (check_ptr == NULL)
	{
		printfs("[Info] : MmGetSystemRoutineAddress函数失败");
		return NULL;
	}

	// 获取保存数据所需大小
	ULONG bytes = 0;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);
	if (bytes == 0)
	{
		printfs("[Info] : ZwQuerySystemInformation函数失败");
		return NULL;
	}

	// 申请内存
	PRTL_PROCESS_MODULES modules_ptr = ExAllocatePoolWithTag(NonPagedPool, bytes, 'enoB');
	if (modules_ptr == NULL)
	{
		printfs("[Info] : ExAllocatePoolWithTag函数失败");
		return NULL;
	}

	// 清空结构体
	RtlZeroMemory(modules_ptr, bytes);

	// 获取模块信息
	status = ZwQuerySystemInformation(SystemModuleInformation, modules_ptr, bytes, &bytes);
	if (!NT_SUCCESS(status))
	{
		printfs("[Info] : ZwQuerySystemInformation函数失败");
		ExFreePoolWithTag(modules_ptr, 'enoB');
		return NULL;
	}

	// 开始遍历模块
	PRTL_PROCESS_MODULE_INFORMATION mod_ptr = modules_ptr->Modules;
	for (ULONG i = 0; i < modules_ptr->NumberOfModules; i++)
	{
		printfs("[Info] : %d - %s - %x \n", i, mod_ptr[i].FullPathName, mod_ptr[i].ImageBase);

		// 系统模块区间定位,这就是确定NtOpenFile函数所在的那一个模块
		if (check_ptr >= mod_ptr[i].ImageBase
			&& check_ptr < (PVOID)((PUCHAR)mod_ptr[i].ImageBase + mod_ptr[i].ImageSize))
		{
			// 保存到静态变量
			kernel_addr = mod_ptr[i].ImageBase;
			kernel_size = mod_ptr[i].ImageSize;

			// 保存大小
			*size = kernel_size;
		}
	}

	// 释放内存
	ExFreePoolWithTag(modules_ptr, 'enoB');

	// 返回地址
	return kernel_addr;
}

PVOID64 search_pattern(
	PCUCHAR pattern,
	UCHAR wildcard,
	ULONG_PTR len,
	PVOID64 base,
	ULONG_PTR size,
	INT index)
{
	INT cIndex = 0;
	for (ULONG_PTR i = 0; i < size - len; i++)
	{
		BOOLEAN found = TRUE;
		for (ULONG_PTR j = 0; j < len; j++)
		{
			if (pattern[j] != wildcard && pattern[j] != ((PCUCHAR)base)[i + j])
			{
				found = FALSE;
				break;
			}
		}

		if (found != FALSE && cIndex++ == index)
		{
			return (PUCHAR)base + i;
		}
	}

	return NULL;
}

PVOID64 scan_section(
	PCCHAR section,
	PCUCHAR pattern,
	UCHAR wildcard,
	ULONG_PTR len,
	PVOID64 base)
{
	// 根据基址就是PE文件头指针
	PIMAGE_NT_HEADERS64 nt_ptr = RtlImageNtHeader(base);
	if (!nt_ptr)
	{
		printfs("[Info] : RtlImageNtHeader");
		return NULL;
	}

	// 找到节区指针
	PIMAGE_SECTION_HEADER pFirstSection = (PIMAGE_SECTION_HEADER)((uintptr_t)&nt_ptr->FileHeader + nt_ptr->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER));

	// 初始化字符串
	ANSI_STRING s1;
	RtlInitAnsiString(&s1, section);

	// 遍历节区
	for (PIMAGE_SECTION_HEADER section_ptr = pFirstSection;
		section_ptr < pFirstSection + nt_ptr->FileHeader.NumberOfSections;
		section_ptr++)
	{
		// 初始化字符串
		ANSI_STRING s2;
		RtlInitAnsiString(&s2, (PCCHAR)section_ptr->Name);

		// 节区查找
		if (RtlCompareString(&s1, &s2, TRUE) == 0)
		{
			PVOID64 addr = search_pattern(pattern, wildcard, len, (PUCHAR)base + section_ptr->VirtualAddress, section_ptr->Misc.VirtualSize, 0);
			if (addr) return addr;
		}
	}

	return NULL;
}

VOID locate_piddb(PPIDDB_INFO info)
{
	// 模式签名
	static UCHAR PiDDBLockPtr_sig[] = "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\x8C";
	static UCHAR PiDDBCacheTablePtr_sig[] = "\x66\x03\xD2\x48\x8D\x0D";

	// 获取内核基址
	ULONG64 size = 0;
	PVOID64 base = get_kernel_mode_base(&size);
	if (base == NULL) return;

	// 地址查找
	PVOID64 lock_ptr = scan_section("PAGE", PiDDBLockPtr_sig, 0, sizeof(PiDDBLockPtr_sig) - 1, base);
	PVOID64 table_ptr = scan_section("PAGE", PiDDBCacheTablePtr_sig, 0, sizeof(PiDDBCacheTablePtr_sig) - 1, base);
	if (lock_ptr == NULL || table_ptr == NULL) return;

	// 加上偏移3
	table_ptr = (PVOID64)((uintptr_t)table_ptr + 3);

	// 然后计算相对的地址
	info->lock = (PERESOURCE)(resolve_relative_address(lock_ptr, 3, 7));
	info->table = (PRTL_AVL_TABLE)(resolve_relative_address(table_ptr, 3, 7));
}

PVOID64 resolve_relative_address(PVOID64 Instruction, ULONG offset, ULONG size)
{
	ULONG_PTR in_str = (ULONG_PTR)Instruction;
	LONG rip_offset = *(PLONG)(in_str + offset);
	PVOID resolved_addr = (PVOID)(in_str + size + rip_offset);

	return resolved_addr;
}

BOOLEAN clear_piddb_cache(UNICODE_STRING name, ULONG stamp)
{
	// 定位结构地址
	PIDDB_INFO info;
	RtlZeroMemory(&info, sizeof(info));
	locate_piddb(&info);
	if (info.lock == NULL || info.table == NULL) return FALSE;
	printfs("[Info] : lock address : %x \n", info.lock);
	printfs("[Info] : table address : %x \n", info.table);

	// 填充信息
	PiDDB_CACHE_ENTRY entry;
	RtlZeroMemory(&entry, sizeof(entry));
	entry.DriverName = name;
	entry.TimeDateStamp = stamp;

	if (ExAcquireResourceExclusiveLite(info.lock, TRUE))//尝试锁定资源
	{
		PPiDDB_CACHE_ENTRY data = RtlLookupElementGenericTableAvl(info.table, &entry);//从表中查找
		if (data)
		{
			if (RemoveEntryList(&data->List))//尝试断链
			{
				if (RtlDeleteElementGenericTableAvl(info.table, data))//删除元素
				{
					return TRUE;
				}
			}
		}
		ExReleaseResourceLite(info.lock);//尝试解锁
	}

	return FALSE;
}

BOOLEAN data_compare(BYTE* data, BYTE* mask, CHAR* str)
{
	for (; *str; ++str, ++data, ++mask)
		if (*str == 'x' && *data != *mask)
			return 0;

	return (*str) == 0;
}

DWORD64 find_pattern(DWORD64 addr, DWORD64 len, BYTE* mask, CHAR* str)
{
	for (DWORD64 i = 0; i < len; i++)
		if (data_compare((BYTE*)(addr + i), mask, str))
			return (UINT64)(addr + i);

	return 0;
}

VOID spoof_unload_driver_list()
{
	// 获取内核基址
	ULONG64 size = 0;
	PVOID64 base = get_kernel_mode_base(&size);
	if (base == NULL) return;

	// 定位地址
	PVOID64 unloaded_drivers_instr = (PVOID64)find_pattern(base, size, (BYTE*)"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9", "xxx????xxx");
	PVOID64 last_unloaded_driver_instr = (PVOID64)find_pattern(base, size, (BYTE*)"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32", "xx????xxx");
	if (unloaded_drivers_instr == NULL || last_unloaded_driver_instr == NULL) return;

	// 计算相对地址
	PMM_UNLOADED_DRIVER unloaded_drivers = *(PMM_UNLOADED_DRIVER*)resolve_relative_address(unloaded_drivers_instr, 3, 7);
	PULONG last_unloaded_driver = (PULONG)resolve_relative_address(last_unloaded_driver_instr, 2, 6);

	// 显示卸载驱动列表数量
	printfs("[Info] numbers is : %d \n", *last_unloaded_driver);

	// 遍历卸载驱动链表
	for (INT i = 0; i < 50; i++)
	{
		// 获取结构指针
		PMM_UNLOADED_DRIVER entry = &unloaded_drivers[i];

		// 如果字符串为空,就代表后面没有了
		if (entry->Name.Length == 0) break;

		// 显示字符串
		printfs("[Info] index : %d \t name : %ws \t time : %d \t module : %x \n",
			i, entry->Name.Buffer, entry->UnloadTime, entry->ModuleStart);
	}
}

/* 驱动卸载函数 */
VOID DriverUnload(PDRIVER_OBJECT object)
{
	printfs("[Info] : 驱动卸载成功");
}

/* 驱动入口函数 */
NTSTATUS DriverEntry(PDRIVER_OBJECT object, PUNICODE_STRING reg)
{
	// 设置驱动卸载函数
	object->DriverUnload = DriverUnload;

	// 清空piddb
	UNICODE_STRING name = RTL_CONSTANT_STRING(L"Kernel-Bridge.sys");
	if (clear_piddb_cache(name, 0x354585)) printfs("[Info] : piddb finish");
	else printfs("[Info] : piddb fail");

	// 扰乱卸载驱动列表
	spoof_unload_driver_list();

	printfs("[Info] : 驱动加载成功");
	return STATUS_SUCCESS;
}