#include "hooks.hpp"

/* 驱动出口 */
EXTERN_C VOID DriverUnload(PDRIVER_OBJECT object)
{
	// 未引用
	UNREFERENCED_PARAMETER(object);

	// unhook全部
	hooks::start_all_un_hook();

	// 延迟退出
	LARGE_INTEGER LargeInteger{ };
	LargeInteger.QuadPart = -10000000;
	KeDelayExecutionThread(KernelMode, FALSE, &LargeInteger);

	printfs("驱动卸载完毕");
}

/* 驱动入口 */
EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT object, PUNICODE_STRING reg)
{
	// 未引用
	UNREFERENCED_PARAMETER(reg);

	// 设置卸载函数
	object->DriverUnload = DriverUnload;

	if (hooks::initialize())					// 初始化
		if (hooks::start_all_hook())		// hook全部
			printfs("全部hook完毕");

	printfs("驱动加载完毕");
	return STATUS_SUCCESS;
}