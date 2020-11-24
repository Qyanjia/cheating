#include "entry.hpp"

VOID re_initialize(PDRIVER_OBJECT DriverObject, PVOID Context, ULONG Count)
{
	// 未引用
	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Count);

	// 隐藏自身
	win10_kernel_driver_hide::driver_hide(DriverObject);
}

/* 驱动入口 */
EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT object, PUNICODE_STRING reg)
{
	// 未引用
	UNREFERENCED_PARAMETER(reg);

	// 重新初始化
	IoRegisterDriverReinitialization(object, re_initialize, NULL);

	printfs("驱动加载成功");
	return STATUS_SUCCESS;
}