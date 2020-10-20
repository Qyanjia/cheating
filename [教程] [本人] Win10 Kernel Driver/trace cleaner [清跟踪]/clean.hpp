#pragma once

#include "struct.h"

// 调试信息输出
#define log(format, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, format, __VA_ARGS__)

// 内存池Tag
#define BB_POOL_TAG 'enoB'

// PIDDB内存签名
UCHAR PiDDBLockPtr_sig[] = "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\x8C";
UCHAR PiDDBCacheTablePtr_sig[] = "\x66\x03\xD2\x48\x8D\x0D";

//you can also put the sig within the function, but some of the sig ends up on the stack and in the .text section, and causes issues when zeroing the sig memory.

// 相对地址转化函数
EXTERN_C PVOID ResolveRelativeAddress(
	_In_ PVOID Instruction,
	_In_ ULONG OffsetOffset,
	_In_ ULONG InstructionSize)
{
	ULONG_PTR Instr = (ULONG_PTR)Instruction;
	LONG RipOffset = *(PLONG)(Instr + OffsetOffset);
	PVOID ResolvedAddr = (PVOID)(Instr + InstructionSize + RipOffset);

	return ResolvedAddr;
}

// 内存模式匹配
NTSTATUS BBSearchPattern(
	IN PCUCHAR pattern,
	IN UCHAR wildcard,
	IN ULONG_PTR len,
	IN const VOID* base,
	IN ULONG_PTR size,
	OUT PVOID* ppFound,
	int index = 0)
{
	if (ppFound == NULL || pattern == NULL || base == NULL)
		return STATUS_ACCESS_DENIED; //STATUS_INVALID_PARAMETER;

	int cIndex = 0;
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
			*ppFound = (PUCHAR)base + i;
			return STATUS_SUCCESS;
		}
	}

	return STATUS_NOT_FOUND;
}

PVOID g_KernelBase = NULL;
ULONG g_KernelSize = 0;

// 获取基址
PVOID GetKernelBase(OUT PULONG pSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytes = 0;
	PRTL_PROCESS_MODULES pMods = NULL;
	PVOID checkPtr = NULL;
	UNICODE_STRING routineName;

	// Already found
	// 如果上一次查找过了,直接返回结果就行了
	if (g_KernelBase != NULL)
	{
		if (pSize)
			*pSize = g_KernelSize;
		return g_KernelBase;
	}

	// 查找NtOpenFile函数的地址
	RtlUnicodeStringInit(&routineName, L"NtOpenFile");
	checkPtr = MmGetSystemRoutineAddress(&routineName);
	if (checkPtr == NULL)
	{
		log("[clear] : 查找NtOpenFile函数地址失败");
		return NULL;
	}

	// Protect from UserMode AV
	// 获取所需大小
	status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);
	if (bytes == 0)
	{
		log("[clear] : Invalid SystemModuleInformation size");
		return NULL;
	}

	// 申请内存空间
	pMods = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, BB_POOL_TAG);
	RtlZeroMemory(pMods, bytes);

	// 获取模块信息
	status = ZwQuerySystemInformation(SystemModuleInformation, pMods, bytes, &bytes);
	if (NT_SUCCESS(status))
	{
		PRTL_PROCESS_MODULE_INFORMATION pMod = pMods->Modules;

		// 遍历模块
		for (ULONG i = 0; i < pMods->NumberOfModules; i++)
		{
			// System routine is inside module
			// 系统模块区间定位,这就是确定NtOpenFile函数所在的那一个模块
			if (checkPtr >= pMod[i].ImageBase
				&& checkPtr < (PVOID)((PUCHAR)pMod[i].ImageBase + pMod[i].ImageSize))
			{
				g_KernelBase = pMod[i].ImageBase;
				g_KernelSize = pMod[i].ImageSize;
				if (pSize)
					*pSize = g_KernelSize;
				break;
			}
		}
	}

	// 释放内存
	if (pMods)
		ExFreePoolWithTag(pMods, BB_POOL_TAG);

	log("g_KernelBase: %x", g_KernelBase);
	log("g_KernelSize: %x", g_KernelSize);
	return g_KernelBase;
}

// 节区扫描
NTSTATUS BBScanSection(
	IN PCCHAR section,
	IN PCUCHAR pattern,
	IN UCHAR wildcard,
	IN ULONG_PTR len,
	OUT PVOID* ppFound,
	PVOID base = nullptr)
{
	// 先得到指定的模块基址
	if (ppFound == NULL) return STATUS_ACCESS_DENIED; //STATUS_INVALID_PARAMETER
	if (nullptr == base) base = GetKernelBase(&g_KernelSize);
	if (base == nullptr) return STATUS_ACCESS_DENIED; //STATUS_NOT_FOUND;

	// 根据基址就是PE文件头指针
	PIMAGE_NT_HEADERS64 pHdr = RtlImageNtHeader(base);
	if (!pHdr)
	{
		log("[clear] : 查找到基址无效");
		return STATUS_ACCESS_DENIED; // STATUS_INVALID_IMAGE_FORMAT;
	}

	// 找到节区指针
	PIMAGE_SECTION_HEADER pFirstSection = (PIMAGE_SECTION_HEADER)((uintptr_t)&pHdr->FileHeader + pHdr->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER));

	// 遍历节区
	for (PIMAGE_SECTION_HEADER pSection = pFirstSection;
		pSection < pFirstSection + pHdr->FileHeader.NumberOfSections;
		pSection++)
	{
		ANSI_STRING s1, s2;
		RtlInitAnsiString(&s1, section);
		RtlInitAnsiString(&s2, (PCCHAR)pSection->Name);

		// 节区查找
		if (RtlCompareString(&s1, &s2, TRUE) == 0)
		{
			// 内存模式匹配
			PVOID ptr = NULL;
			NTSTATUS status = BBSearchPattern(pattern, wildcard, len, (PUCHAR)base + pSection->VirtualAddress, pSection->Misc.VirtualSize, &ptr);
			if (NT_SUCCESS(status))
			{
				*(PULONG64)ppFound = (ULONG_PTR)(ptr); //- (PUCHAR)base
				return status;
			}
			//we continue scanning because there can be multiple sections with the same name.
		}
	}

	return STATUS_ACCESS_DENIED; //STATUS_NOT_FOUND;
}

// 定位PIDDB
extern "C" bool LocatePiDDB(PERESOURCE* lock, PRTL_AVL_TABLE* table)
{
	PVOID PiDDBLockPtr = nullptr, PiDDBCacheTablePtr = nullptr;
	if (!NT_SUCCESS(BBScanSection("PAGE", PiDDBLockPtr_sig, 0, sizeof(PiDDBLockPtr_sig) - 1, reinterpret_cast<PVOID*>(&PiDDBLockPtr))))
	{
		log("[clear] : Unable to find PiDDBLockPtr sig.");
		return false;
	}

	if (!NT_SUCCESS(BBScanSection("PAGE", PiDDBCacheTablePtr_sig, 0, sizeof(PiDDBCacheTablePtr_sig) - 1, reinterpret_cast<PVOID*>(&PiDDBCacheTablePtr))))
	{
		log("[clear] : Unable to find PiDDBCacheTablePtr sig");
		return false;
	}

	// 加上偏移3
	PiDDBCacheTablePtr = PVOID((uintptr_t)PiDDBCacheTablePtr + 3);

	// 然后计算相对的地址
	*lock = (PERESOURCE)(ResolveRelativeAddress(PiDDBLockPtr, 3, 7));
	*table = (PRTL_AVL_TABLE)(ResolveRelativeAddress(PiDDBCacheTablePtr, 3, 7));

	return true;
}

PMM_UNLOADED_DRIVER MmUnloadedDrivers;
PULONG				MmLastUnloadedDriver;

// 数据匹配
BOOLEAN bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return 0;

	return (*szMask) == 0;
}

// 查找模式
UINT64 FindPattern(UINT64 dwAddress, UINT64 dwLen, BYTE *bMask, char * szMask)
{
	for (UINT64 i = 0; i < dwLen; i++)
		if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
			return (UINT64)(dwAddress + i);

	return 0;
}

// 查找卸载驱动表相关数据
NTSTATUS FindMmDriverData(VOID)
{
	/*
	 *	nt!MmLocateUnloadedDriver:
	 *	fffff801`51c70394 4c8b15a57e1500  mov     r10,qword ptr [nt!MmUnloadedDrivers (fffff801`51dc8240)]
	 *	fffff801`51c7039b 4c8bc9          mov     r9 ,rcx
	 */
	 // 查找卸载驱动表指针
	PVOID MmUnloadedDriversInstr = (PVOID)FindPattern((UINT64)g_KernelBase, g_KernelSize,
		(BYTE*)"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
		"xxx????xxx"
	);

	/*
	 *	nt!MiRememberUnloadedDriver+0x59:
	 *	fffff801`5201a4c5 8b057ddddaff    mov     eax,dword ptr [nt!MmLastUnloadedDriver (fffff801`51dc8248)]
	 *	fffff801`5201a4cb 83f832          cmp     eax,32h
	*/
	// 查找上一个卸载驱动指针
	PVOID MmLastUnloadedDriverInstr = (PVOID)FindPattern((UINT64)g_KernelBase, g_KernelSize,
		(BYTE*)"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
		"xx????xxx"
	);

	// 没有查找到地址
	if (MmUnloadedDriversInstr == NULL || MmLastUnloadedDriverInstr == NULL)
		return STATUS_NOT_FOUND;

	// 相对转化
	MmUnloadedDrivers = *(PMM_UNLOADED_DRIVER*)ResolveRelativeAddress(MmUnloadedDriversInstr, 3, 7);
	MmLastUnloadedDriver = (PULONG)ResolveRelativeAddress(MmLastUnloadedDriverInstr, 2, 6);

	log("[clear] : MmUnloadedDrivers Addr: %x", MmUnloadedDrivers);
	log("[clear] : MmLastUnloadedDriver Addr: %x", MmLastUnloadedDriver);
	return STATUS_SUCCESS;
}

// 判断卸载表的实例是否为空
BOOLEAN IsUnloadedDriverEntryEmpty(_In_ PMM_UNLOADED_DRIVER Entry)
{
	if (Entry->Name.MaximumLength == 0 ||
		Entry->Name.Length == 0 ||
		Entry->Name.Buffer == NULL)
		return TRUE;

	return FALSE;
}

// 判断卸载表是否修改过
BOOLEAN IsMmUnloadedDriversFilled(VOID)
{
	// 遍历卸载驱动列表
	for (ULONG Index = 0; Index < MM_UNLOADED_DRIVERS_SIZE; ++Index)
	{
		PMM_UNLOADED_DRIVER Entry = &MmUnloadedDrivers[Index];

		// 有空的,被修改过????
		if (IsUnloadedDriverEntryEmpty(Entry))
			return FALSE;
	}

	return TRUE;
}

ERESOURCE PsLoadedModuleResource;

namespace clear
{
	// 清理PIDDB
	NTSTATUS clearCache(UNICODE_STRING DriverName, ULONG timeDateStamp)
	{
		// first locate required variables
		// 先要找到需要的变量
		PERESOURCE PiDDBLock = nullptr;
		PRTL_AVL_TABLE PiDDBCacheTable = nullptr;
		if (!LocatePiDDB(&PiDDBLock, &PiDDBCacheTable))
		{
			log("[clear] : ClearCache Failed");
			return STATUS_UNSUCCESSFUL;
		}

		log("[clear] : Found PiDDBLock and PiDDBCacheTable");
		log("[clear] : Found PiDDBLock %x", PiDDBLock);
		log("[clear] : Found PiDDBCacheTable %x", PiDDBCacheTable);

		// build a lookup entry
		// 构建一个实例对象
		PiDDBCacheEntry lookupEntry = { };
		lookupEntry.DriverName = DriverName;					// 目标驱动名称
		lookupEntry.TimeDateStamp = timeDateStamp;		// 目标驱动时间戳

		// acquire the ddb resource lock
		// 尝试锁定
		BOOLEAN Res = ExAcquireResourceExclusiveLite(PiDDBLock, TRUE);
		if (Res == FALSE)
		{
			log("[clear] : ExAcquireResourceExclusiveLite失败");
			return STATUS_UNSUCCESSFUL;
		}

		// search our entry in the table
		// 从表中查找
		PiDDBCacheEntry* pFoundEntry = (PiDDBCacheEntry*)RtlLookupElementGenericTableAvl(PiDDBCacheTable, &lookupEntry);
		if (pFoundEntry == nullptr)
		{
			// release the ddb resource lock
			ExReleaseResourceLite(PiDDBLock);
			log("[clear] : ClearCache Failed (Not found)");
			return STATUS_UNSUCCESSFUL;
		}

		// first, unlink from the list
		// 第一步就是断链
		Res = RemoveEntryList(&pFoundEntry->List);
		if (Res == FALSE)
		{
			ExReleaseResourceLite(PiDDBLock);
			log("[clear] : RemoveEntryList失败");
			return STATUS_UNSUCCESSFUL;
		}

		// then delete the element from the avl table
		// 后面就从这个AVL表中删除元素
		Res = RtlDeleteElementGenericTableAvl(PiDDBCacheTable, pFoundEntry);
		if (Res == FALSE)
		{
			ExReleaseResourceLite(PiDDBLock);
			log("[clear] : RtlDeleteElementGenericTableAvl失败");
			return STATUS_UNSUCCESSFUL;
		}

		// release the ddb resource lock
		// 释放锁定
		ExReleaseResourceLite(PiDDBLock);
		log("[clear] : ClearCache Sucessful");

		return STATUS_SUCCESS;
	}

	// 清空驱动卸载表
	NTSTATUS ClearUnloadedDriver(_In_ PUNICODE_STRING	DriverName, _In_ BOOLEAN AccquireResource)
	{
		// 尝试锁定
		if (AccquireResource)
			ExAcquireResourceExclusiveLite(&PsLoadedModuleResource, TRUE);

		BOOLEAN Modified = FALSE;
		BOOLEAN Filled = IsMmUnloadedDriversFilled();// 完整的卸载驱动表,没有NULL的

		// 遍历驱动卸载表
		for (ULONG Index = 0; Index < MM_UNLOADED_DRIVERS_SIZE; ++Index)
		{
			PMM_UNLOADED_DRIVER Entry = &MmUnloadedDrivers[Index];

			// 修改过,就需要修复链表
			if (Modified)
			{
				// Shift back all entries after modified one.
				// 修改后链表后移
				PMM_UNLOADED_DRIVER PrevEntry = &MmUnloadedDrivers[Index - 1];
				RtlCopyMemory(PrevEntry, Entry, sizeof(MM_UNLOADED_DRIVER));

				// Zero last entry.
				// 清空上一个
				if (Index == MM_UNLOADED_DRIVERS_SIZE - 1)
					RtlFillMemory(Entry, sizeof(MM_UNLOADED_DRIVER), 0);
			}
			else if (RtlEqualUnicodeString(DriverName, &Entry->Name, TRUE))		// 查找到需要清空的驱动信息了
			{
				// Erase driver entry.
				// 移除设备入口?就释放了一下内存
				PVOID BufferPool = Entry->Name.Buffer;
				RtlFillMemory(Entry, sizeof(MM_UNLOADED_DRIVER), 0);
				ExFreePoolWithTag(BufferPool, 'TDmM');

				// Because we are erasing last entry we want to set MmLastUnloadedDriver to 49
				// if list have been already filled.
				// 移除了一个,相应计数也要进行减少
				*MmLastUnloadedDriver = (Filled ? MM_UNLOADED_DRIVERS_SIZE : *MmLastUnloadedDriver) - 1;
				Modified = TRUE;
			}
		}

		// 修改过
		if (Modified)
		{
			ULONG64 PreviousTime = 0;

			// Make UnloadTime look right.
			// 确保时间的准确性
			for (LONG Index = MM_UNLOADED_DRIVERS_SIZE - 2; Index >= 0; --Index)
			{
				PMM_UNLOADED_DRIVER Entry = &MmUnloadedDrivers[Index];

				// 如果卸载驱动信息是空
				if (IsUnloadedDriverEntryEmpty(Entry))
					continue;

				// 设置一个相对合理的时间
				if (PreviousTime != 0 && Entry->UnloadTime > PreviousTime)
					Entry->UnloadTime = PreviousTime - 100;

				PreviousTime = Entry->UnloadTime;
			}

			// Clear remaining entries.
			//	清空剩下的
			ClearUnloadedDriver(DriverName, FALSE);
		}

		// 解锁
		if (AccquireResource)
			ExReleaseResourceLite(&PsLoadedModuleResource);

		return Modified ? STATUS_SUCCESS : STATUS_NOT_FOUND;
	}
}