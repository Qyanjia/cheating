#pragma warning(disable : 4133)

#include <ntifs.h>
#include <ntddk.h>

/* 调试辅助函数 */
#define printfs(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

/* 获取系统模块信息宏 */
#define SystemModuleInformation 11

/* 函数定义 */
typedef NTSTATUS(__fastcall *MiProcessLoaderEntry)(PVOID pDriverSection, BOOLEAN bLoad);
typedef NTSTATUS(*NtQuerySystemInformation)(
	IN ULONG SystemInformationClass,
	OUT PVOID   SystemInformation,
	IN ULONG_PTR    SystemInformationLength,
	OUT PULONG_PTR  ReturnLength OPTIONAL);

/* 系统模块信息结构体 */
typedef struct _SYSTEM_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

/* LDR数据结构体 */
typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID      DllBase;
	PVOID      EntryPoint;
}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

/* 这些函数声明才能用 */
NTSYSAPI NTSTATUS NTAPI ObReferenceObjectByName(
	__in PUNICODE_STRING ObjectName,
	__in ULONG Attributes,
	__in_opt PACCESS_STATE AccessState,
	__in_opt ACCESS_MASK DesiredAccess,
	__in POBJECT_TYPE ObjectType,
	__in KPROCESSOR_MODE AccessMode,
	__inout_opt PVOID ParseContext,
	__out PVOID* Object
);

extern POBJECT_TYPE *IoDriverObjectType;