#include "clean.hpp"

/*
功能 : 清理驱动文件加载/卸载记录,防止反作弊引擎的跟踪封禁
系统 : Win10 x64 1809
*/

extern "C" VOID DriverUnload(PDRIVER_OBJECT driver_object)
{
	// 未引用
	UNREFERENCED_PARAMETER(driver_object);

	log("[clear] : 驱动卸载");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT  driver_object, PUNICODE_STRING registry_path)
{
	// 未引用
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	// 设置卸载函数
	driver_object->DriverUnload = DriverUnload;

	// 清理PiddbCacheTable
	UNICODE_STRING driver_name = RTL_CONSTANT_STRING(L"ReadWriteDriver.sys");
	if (clear::clearCache(driver_name, 0x5F7884B1) == STATUS_SUCCESS)
		log("[clear] : 清理PiddbCacheTable成功");
	else
		log("[clear] : 清理PiddbCacheTable失败");

	// 清理mmUnloadedDrivers
	if (FindMmDriverData() == STATUS_SUCCESS)
	{
		if (clear::ClearUnloadedDriver(&driver_name, true) == STATUS_SUCCESS)
			log("[clear] : 清理mmUnloadedDrivers成功");
		else
			log("[clear] : 清理mmUnloadedDrivers失败");
	}
	else log("[clear] : 查找mmUnloadedDrivers表失败");

	return STATUS_SUCCESS;
}