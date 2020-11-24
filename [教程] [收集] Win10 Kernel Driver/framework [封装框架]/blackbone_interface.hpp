#pragma once
#pragma warning(disable : 4005)

#include <windows.h>
#include <ntstatus.h>
#include <winternl.h>

#include <string>
#include <map>
#include <vector>

#define FILE_DEVICE_BLACKBONE           0x8005

#define IOCTL_BLACKBONE_DISABLE_DEP  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_SET_PROTECTION  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_GRANT_ACCESS   (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_ALLOCATE_FREE_MEMORY  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x804, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_COPY_MEMORY  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x803, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_PROTECT_MEMORY  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x805, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_MAP_MEMORY  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x806, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_MAP_REGION  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x807, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_UNMAP_MEMORY  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_UNMAP_REGION  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x809, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_INJECT_DLL  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x80B, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_HIDE_VAD  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x80A, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_UNLINK_HTABLE  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x80D, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_BLACKBONE_ENUM_REGIONS  (ULONG)CTL_CODE(FILE_DEVICE_BLACKBONE, 0x80E, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

typedef struct _DISABLE_DEP
{
	ULONG   pid;            // Process ID
} DISABLE_DEP, *PDISABLE_DEP;

typedef enum _PolicyOpt
{
	Policy_Disable,
	Policy_Enable,
	Policy_Keep,        // Don't change current value
} PolicyOpt;

typedef struct _SET_PROC_PROTECTION
{
	ULONG pid;              // Process ID
	PolicyOpt protection;   // Process protection
	PolicyOpt dynamicCode;  // DynamiCode policy
	PolicyOpt signature;    // BinarySignature policy
} SET_PROC_PROTECTION, *PSET_PROC_PROTECTION;

typedef struct _HANDLE_GRANT_ACCESS
{
	ULONGLONG  handle;      // Handle to modify
	ULONG      pid;         // Process ID
	ULONG      access;      // Access flags to grant
} HANDLE_GRANT_ACCESS, *PHANDLE_GRANT_ACCESS;

typedef struct _ALLOCATE_FREE_MEMORY
{
	ULONGLONG base;             // Region base address
	ULONGLONG size;             // Region size
	ULONG     pid;              // Target process id
	ULONG     protection;       // Memory protection for allocation
	ULONG     type;             // MEM_RESERVE/MEM_COMMIT/MEM_DECOMMIT/MEM_RELEASE
	BOOLEAN   allocate;         // TRUE if allocation, FALSE is freeing
	BOOLEAN   physical;         // If set to TRUE, physical pages will be directly mapped into UM space
} ALLOCATE_FREE_MEMORY, *PALLOCATE_FREE_MEMORY;

typedef struct _ALLOCATE_FREE_MEMORY_RESULT
{
	ULONGLONG address;          // Address of allocation
	ULONGLONG size;             // Allocated size
} ALLOCATE_FREE_MEMORY_RESULT, *PALLOCATE_FREE_MEMORY_RESULT;

typedef struct _COPY_MEMORY
{
	ULONGLONG localbuf;         // Buffer address
	ULONGLONG targetPtr;        // Target address
	ULONGLONG size;             // Buffer size
	ULONG     pid;              // Target process id
	BOOLEAN   write;            // TRUE if write operation, FALSE if read
} COPY_MEMORY, *PCOPY_MEMORY;

typedef struct _PROTECT_MEMORY
{
	ULONGLONG base;             // Region base address
	ULONGLONG size;             // Region size
	ULONG     pid;              // Target process id
	ULONG     newProtection;    // New protection value
} PROTECT_MEMORY, *PPROTECT_MEMORY;

struct MapMemoryResult
{
	DWORD64 hostSharedPage;       // Shared page address in current process
	DWORD64 targetSharedPage;     // Shared page address in target process
	HANDLE targetPipe;          // Hook pipe handle in the target process

	using mapMemoryMap = std::map<std::pair<DWORD64, uint32_t>, DWORD64>;
	mapMemoryMap regions;       // Mapped regions info
};

typedef struct _MAP_MEMORY
{
	ULONG   pid;                // Target process id
	wchar_t pipeName[32];       // Hook pipe name
	BOOLEAN mapSections;        // Set to TRUE to map sections
} MAP_MEMORY, *PMAP_MEMORY;

typedef struct _MAP_MEMORY_RESULT_ENTRY
{
	ULONGLONG originalPtr;      // Address in target process
	ULONGLONG newPtr;           // Mapped address in host process
	ULONG size;                 // Region size
} MAP_MEMORY_RESULT_ENTRY, *PMAP_MEMORY_RESULT_ENTRY;

typedef struct _MAP_MEMORY_RESULT
{
	ULONGLONG pipeHandle;       // Pipe handle in target process
	ULONGLONG targetPage;       // Address of shared page in target process
	ULONGLONG hostPage;         // Address of shared page in host process

	ULONG count;                // Number of REMAP_MEMORY_RESULT_ENTRY entries

	// List of remapped regions (variable-sized array)
	MAP_MEMORY_RESULT_ENTRY entries[1];
} MAP_MEMORY_RESULT, *PMAP_MEMORY_RESULT;

struct MapMemoryRegionResult
{
	DWORD64 originalPtr;          // Address of region in the target process
	DWORD64 newPtr;               // Address of mapped region in the current process
	DWORD64 removedPtr;           // Address of region unmapped because of address conflict
	uint32_t size;              // Size of mapped region
	uint32_t removedSize;       // Size of unmapped region
};

typedef struct _MAP_MEMORY_REGION
{
	ULONGLONG base;             // Region base address
	ULONG     pid;              // Target process id
	ULONG     size;             // Region size
} MAP_MEMORY_REGION, *PMAP_MEMORY_REGION;

typedef struct _MAP_MEMORY_REGION_RESULT
{
	ULONGLONG originalPtr;      // Address in target process
	ULONGLONG newPtr;           // Mapped address in host process
	ULONGLONG removedPtr;       // Unmapped region base, in case of conflicting region
	ULONG     size;             // Mapped region size
	ULONG     removedSize;      // Unmapped region size
} MAP_MEMORY_REGION_RESULT, *PMAP_MEMORY_REGION_RESULT;

typedef struct _UNMAP_MEMORY
{
	ULONG     pid;              // Target process ID
} UNMAP_MEMORY, *PUNMAP_MEMORY;

typedef struct _UNMAP_MEMORY_REGION
{
	ULONGLONG base;             // Region base address
	ULONG     pid;              // Target process ID
	ULONG     size;             // Region size
} UNMAP_MEMORY_REGION, *PUNMAP_MEMORY_REGION;

typedef enum _InjectType
{
	IT_Thread,      // CreateThread into LdrLoadDll
	IT_Apc,         // Force user APC into LdrLoadDll
	IT_MMap,        // Manual map
} InjectType;

typedef enum _MmapFlags
{
	KNoFlags = 0x00,    // No flags
	KManualImports = 0x01,    // Manually map import libraries
	KWipeHeader = 0x04,    // Wipe image PE headers
	KHideVAD = 0x10,    // Make image appear as PAGE_NOACESS region
	KRebaseProcess = 0x40,    // If target image is an .exe file, process base address will be replaced with mapped module value
	KNoThreads = 0x80,    // Don't create new threads, use hijacking

	KNoExceptions = 0x01000, // Do not create custom exception handler
	KNoSxS = 0x08000, // Do not apply SxS activation context
	KNoTLS = 0x10000, // Skip TLS initialization and don't execute TLS callbacks
} KMmapFlags;

typedef struct _INJECT_DLL
{
	InjectType type;                // Type of injection
	wchar_t    FullDllPath[512];    // Fully-qualified path to the target dll
	wchar_t    initArg[512];        // Init routine argument
	ULONG      initRVA;             // Init routine RVA, if 0 - no init routine
	ULONG      pid;                 // Target process ID
	BOOLEAN    wait;                // Wait on injection thread
	BOOLEAN    unlink;              // Unlink module after injection
	BOOLEAN    erasePE;             // Erase PE headers after injection
	KMmapFlags flags;               // Manual map flags
	ULONGLONG  imageBase;           // Image address in memory to manually map
	ULONG      imageSize;           // Size of memory image
	BOOLEAN    asImage;             // Memory chunk has image layout
} INJECT_DLL, *PINJECT_DLL;

typedef struct _MMAP_DRIVER
{
	wchar_t    FullPath[512];    // Fully-qualified path to the driver
} MMAP_DRIVER, *PMMAP_DRIVER;

typedef struct _HIDE_VAD
{
	ULONGLONG base;             // Region base address
	ULONGLONG size;             // Region size
	ULONG pid;                  // Target process ID
} HIDE_VAD, *PHIDE_VAD;

typedef struct _UNLINK_HTABLE
{
	ULONG      pid;         // Process ID
} UNLINK_HTABLE, *PUNLINK_HTABLE;

typedef struct _ENUM_REGIONS
{
	ULONG      pid;         // Process ID
} ENUM_REGIONS, *PENUM_REGIONS;

typedef struct _MEM_REGION
{
	ULONGLONG BaseAddress;
	ULONGLONG AllocationBase;
	ULONG AllocationProtect;
	ULONGLONG RegionSize;
	ULONG State;
	ULONG Protect;
	ULONG Type;
} MEM_REGION, *PMEM_REGION;

typedef struct _ENUM_REGIONS_RESULT
{
	ULONGLONG  count;                   // Number of records
	MEM_REGION regions[1];              // Found regions, variable-sized
} ENUM_REGIONS_RESULT, *PENUM_REGIONS_RESULT;

namespace Interfaces
{
	// 驱动句柄
	static HANDLE _hDriver = INVALID_HANDLE_VALUE;

	// 链接符号
	// {985C59E7-0DC1-41C3-8F38-49DAD5B89967}
	static const wchar_t* _Link = L"\\\\.\\{985C59E7-0DC1-41C3-8F38-49DAD5B89967}";

	// 初始化
	HANDLE Initialize()
	{
		_hDriver = CreateFileW(_Link,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		return _hDriver;
	}

	// 禁用DEP
	NTSTATUS DisableDEP(DWORD pid)
	{
		DWORD bytes = 0;
		DISABLE_DEP disableDep = { pid };

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_DISABLE_DEP, &disableDep, sizeof(disableDep), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 设置进程保护标识
	NTSTATUS ProtectProcess(
		DWORD pid,
		PolicyOpt protection,
		PolicyOpt dynamicCode = Policy_Keep,
		PolicyOpt binarySignature = Policy_Keep)
	{
		DWORD bytes = 0;
		SET_PROC_PROTECTION setProt = { pid, protection, dynamicCode, binarySignature };

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_SET_PROTECTION, &setProt, sizeof(setProt), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 修改句柄访问权限
	NTSTATUS PromoteHandle(
		DWORD pid,
		HANDLE handle,
		DWORD access)
	{
		DWORD bytes = 0;
		HANDLE_GRANT_ACCESS grantAccess = { 0 };

		grantAccess.pid = pid;
		grantAccess.handle = (ULONGLONG)handle;
		grantAccess.access = access;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_GRANT_ACCESS, &grantAccess, sizeof(grantAccess), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 申请内存
	NTSTATUS AllocateMem(DWORD pid,
		DWORD64& base,
		DWORD64& size,
		DWORD type,
		DWORD protection,
		bool physical = false)
	{
		DWORD bytes = 0;
		ALLOCATE_FREE_MEMORY allocMem = { 0 };
		ALLOCATE_FREE_MEMORY_RESULT result = { 0 };

		allocMem.pid = pid;
		allocMem.base = base;
		allocMem.size = size;
		allocMem.type = type;
		allocMem.protection = protection;
		allocMem.allocate = TRUE;
		allocMem.physical = physical;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(
			_hDriver, IOCTL_BLACKBONE_ALLOCATE_FREE_MEMORY,
			&allocMem, sizeof(allocMem),
			&result, sizeof(result), &bytes, NULL))
		{
			size = base = 0;
			return STATUS_UNSUCCESSFUL;
		}

		base = result.address;
		size = result.size;

		return STATUS_SUCCESS;
	}

	// 释放内存
	NTSTATUS FreeMem(
		DWORD pid,
		DWORD64 base,
		DWORD64 size,
		DWORD type)
	{
		DWORD bytes = 0;
		ALLOCATE_FREE_MEMORY freeMem = { 0 };
		ALLOCATE_FREE_MEMORY_RESULT result = { 0 };

		freeMem.pid = pid;
		freeMem.base = base;
		freeMem.size = size;
		freeMem.type = type;
		freeMem.allocate = FALSE;
		freeMem.physical = FALSE;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(
			_hDriver, IOCTL_BLACKBONE_ALLOCATE_FREE_MEMORY,
			&freeMem, sizeof(freeMem),
			&result, sizeof(result), &bytes, NULL))
		{
			return STATUS_UNSUCCESSFUL;
		}

		return STATUS_SUCCESS;
	}

	// 读取内存
	NTSTATUS ReadMem(
		DWORD pid,
		DWORD64 base,
		DWORD64 size,
		PVOID buffer)
	{
		DWORD bytes = 0;
		COPY_MEMORY copyMem = { 0 };

		copyMem.pid = pid;
		copyMem.targetPtr = base;
		copyMem.localbuf = (ULONGLONG)buffer;
		copyMem.size = size;
		copyMem.write = FALSE;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_COPY_MEMORY, &copyMem, sizeof(copyMem), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 写入内存
	NTSTATUS WriteMem(
		DWORD pid,
		DWORD64 base,
		DWORD64 size,
		PVOID buffer)
	{
		DWORD bytes = 0;
		COPY_MEMORY copyMem = { 0 };

		copyMem.pid = pid;
		copyMem.targetPtr = base;
		copyMem.localbuf = (ULONGLONG)buffer;
		copyMem.size = size;
		copyMem.write = TRUE;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_COPY_MEMORY, &copyMem, sizeof(copyMem), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 保护内存
	NTSTATUS ProtectMem(
		DWORD pid,
		DWORD64 base,
		DWORD64 size,
		DWORD protection)
	{
		DWORD bytes = 0;
		PROTECT_MEMORY protectMem = { 0 };

		protectMem.pid = pid;
		protectMem.base = base;
		protectMem.size = size;
		protectMem.newProtection = protection;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_PROTECT_MEMORY, &protectMem, sizeof(protectMem), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 映射内存
	NTSTATUS MapMemory(
		DWORD pid,
		const std::wstring& pipeName,
		bool mapSections,
		MapMemoryResult& result)
	{
		MAP_MEMORY data = { 0 };
		DWORD bytes = 0;
		ULONG sizeRequired = 0;
		data.pid = pid;
		data.mapSections = mapSections;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		wcscpy_s(data.pipeName, pipeName.c_str());

		BOOL res = DeviceIoControl(_hDriver, IOCTL_BLACKBONE_MAP_MEMORY, &data, sizeof(data), &sizeRequired, sizeof(sizeRequired), &bytes, NULL);
		if (res != FALSE && bytes == 4)
		{
			MAP_MEMORY_RESULT* pResult = (MAP_MEMORY_RESULT*)malloc(sizeRequired);

			if (DeviceIoControl(_hDriver, IOCTL_BLACKBONE_MAP_MEMORY, &data, sizeof(data), pResult, sizeRequired, &bytes, NULL))
			{
				for (ULONG i = 0; i < pResult->count; i++)
					result.regions.emplace(std::make_pair(std::make_pair(pResult->entries[i].originalPtr, pResult->entries[i].size),
						pResult->entries[i].newPtr));

				result.hostSharedPage = pResult->hostPage;
				result.targetSharedPage = pResult->targetPage;
				result.targetPipe = (HANDLE)pResult->pipeHandle;

				free(pResult);
				return STATUS_SUCCESS;
			}
		}

		return STATUS_UNSUCCESSFUL;
	}

	// 映射内存区域
	NTSTATUS MapMemoryRegion(
		DWORD pid,
		DWORD64 base,
		uint32_t size,
		MapMemoryRegionResult& result)
	{
		MAP_MEMORY_REGION data = { 0 };
		MAP_MEMORY_REGION_RESULT mapResult = { 0 };
		DWORD bytes = 0;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		data.pid = pid;
		data.base = base;
		data.size = size;

		if (DeviceIoControl(_hDriver, IOCTL_BLACKBONE_MAP_REGION, &data, sizeof(data), &mapResult, sizeof(mapResult), &bytes, NULL))
		{
			result.newPtr = mapResult.newPtr;
			result.originalPtr = mapResult.originalPtr;
			result.removedPtr = mapResult.removedPtr;
			result.removedSize = mapResult.removedSize;
			result.size = mapResult.size;

			return STATUS_SUCCESS;
		}

		return STATUS_UNSUCCESSFUL;
	}

	// 取消内存映射
	NTSTATUS UnmapMemory(DWORD pid)
	{
		UNMAP_MEMORY data = { pid };
		DWORD bytes = 0;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (DeviceIoControl(_hDriver, IOCTL_BLACKBONE_UNMAP_MEMORY, &data, sizeof(data), NULL, 0, &bytes, NULL))
			return STATUS_SUCCESS;

		return STATUS_UNSUCCESSFUL;
	}

	// 取消内存区域映射
	NTSTATUS UnmapMemoryRegion(
		DWORD pid,
		DWORD64 base,
		uint32_t size)
	{
		UNMAP_MEMORY_REGION data = { 0 };
		DWORD bytes = 0;

		data.pid = pid;
		data.base = base;
		data.size = size;

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_UNMAP_REGION, &data, sizeof(data), NULL, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 注入DLL
	NTSTATUS InjectDll(
		DWORD pid,
		const std::wstring& path,
		InjectType itype,
		uint32_t initRVA = 0,
		const std::wstring& initArg = L"",
		bool unlink = false,
		bool erasePE = false,
		bool wait = true)
	{
		DWORD bytes = 0;
		INJECT_DLL data = { IT_Thread };

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		wcscpy_s(data.FullDllPath, path.c_str());
		wcscpy_s(data.initArg, initArg.c_str());
		data.type = itype;
		data.pid = pid;
		data.initRVA = initRVA;
		data.wait = wait;
		data.unlink = unlink;
		data.erasePE = erasePE;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_INJECT_DLL, &data, sizeof(data), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 映射DLL
	NTSTATUS MmapDll(
		DWORD pid,
		void* address,
		uint32_t size,
		bool asImage,
		KMmapFlags flags,
		uint32_t initRVA = 0,
		const std::wstring& initArg = L"")
	{
		DWORD bytes = 0;
		INJECT_DLL data = { IT_MMap };

		memset(data.FullDllPath, 0, sizeof(data.FullDllPath));
		wcscpy_s(data.initArg, initArg.c_str());

		data.pid = pid;
		data.initRVA = initRVA;
		data.wait = true;
		data.unlink = false;
		data.erasePE = false;
		data.flags = flags;
		data.imageBase = (ULONGLONG)address;
		data.imageSize = size;
		data.asImage = asImage;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_INJECT_DLL, &data, sizeof(data), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 内存区域禁止访问
	NTSTATUS ConcealVAD(
		DWORD pid,
		DWORD64 base,
		uint32_t size)
	{
		DWORD bytes = 0;
		HIDE_VAD hideVAD = { 0 };

		hideVAD.base = base;
		hideVAD.size = size;
		hideVAD.pid = pid;

		// Not loaded
		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_HIDE_VAD, &hideVAD, sizeof(hideVAD), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 隐藏句柄
	NTSTATUS UnlinkHandleTable(DWORD pid)
	{
		DWORD bytes = 0;
		UNLINK_HTABLE unlink = { pid };

		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_UNLINK_HTABLE, &unlink, sizeof(unlink), nullptr, 0, &bytes, NULL))
			return STATUS_UNSUCCESSFUL;

		return STATUS_SUCCESS;
	}

	// 枚举内存区域
	NTSTATUS EnumMemoryRegions(
		DWORD pid,
		std::vector<MEMORY_BASIC_INFORMATION64>& regions)
	{
		if (_hDriver == INVALID_HANDLE_VALUE)
			return STATUS_UNSUCCESSFUL;

		DWORD bytes = 0;
		ENUM_REGIONS data = { 0 };
		DWORD size = sizeof(ENUM_REGIONS_RESULT);
		auto result = reinterpret_cast<PENUM_REGIONS_RESULT>(malloc(size));

		data.pid = pid;
		result->count = 0;

		DeviceIoControl(_hDriver, IOCTL_BLACKBONE_ENUM_REGIONS, &data, sizeof(data), result, size, &bytes, NULL);

		result->count += 100;
		size = static_cast<DWORD>(result->count * sizeof(result->regions[0]) + sizeof(result->count));
		result = reinterpret_cast<PENUM_REGIONS_RESULT>(realloc(result, size));

		if (!DeviceIoControl(_hDriver, IOCTL_BLACKBONE_ENUM_REGIONS, &data, sizeof(data), result, size, &bytes, NULL))
		{
			free(result);
			return STATUS_UNSUCCESSFUL;
		}

		regions.resize(static_cast<size_t>(result->count));
		for (uint32_t i = 0; i < result->count; i++)
		{
			regions[i].AllocationBase = result->regions[i].AllocationBase;
			regions[i].AllocationProtect = result->regions[i].AllocationProtect;
			regions[i].BaseAddress = result->regions[i].BaseAddress;
			regions[i].Protect = result->regions[i].Protect;
			regions[i].RegionSize = result->regions[i].RegionSize;
			regions[i].State = result->regions[i].State;
			regions[i].Type = result->regions[i].Type;
		}

		free(result);
		return STATUS_SUCCESS;
	}
};