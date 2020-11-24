#include "module.hpp"

PKLDR_DATA_TABLE_ENTRY modules::get_ldr_data_by_name(const wchar_t* szmodule)
{
	// 返回指针
	PKLDR_DATA_TABLE_ENTRY ldr_entry = nullptr;

	// 初始化字符串
	UNICODE_STRING mod{ };
	RtlInitUnicodeString(&mod, szmodule);

	// 为空
	if (!PsLoadedModuleList)
		return ldr_entry;

	// 遍历双向链表
	auto current_ldr_entry = reinterpret_cast<PKLDR_DATA_TABLE_ENTRY>(PsLoadedModuleList->Flink);
	while (reinterpret_cast<PLIST_ENTRY>(current_ldr_entry) != PsLoadedModuleList)
	{
		// 显示
		printfs("name : %ws \t base : %x \t entry : %x ",
			current_ldr_entry->FullDllName.Buffer,
			current_ldr_entry->DllBase,
			current_ldr_entry->EntryPoint);

		// 找到目标
		if (!RtlCompareUnicodeString(&current_ldr_entry->BaseDllName, &mod, TRUE))
		{
			ldr_entry = current_ldr_entry;
			break;
		}

		// 查找下一个
		current_ldr_entry = reinterpret_cast<PKLDR_DATA_TABLE_ENTRY>(current_ldr_entry->InLoadOrderLinks.Flink);
	}

	return ldr_entry;
}

uintptr_t modules::get_kernel_module_base(const wchar_t* szmodule)
{
	// 获取LDR表
	const auto* ldr_entry = get_ldr_data_by_name(szmodule);

	// 获取基址
	return ldr_entry ? reinterpret_cast<uintptr_t>(ldr_entry->DllBase) : 0;
}