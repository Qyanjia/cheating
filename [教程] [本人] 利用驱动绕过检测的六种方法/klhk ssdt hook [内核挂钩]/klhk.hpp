#pragma once

#include "module.hpp"

#include <ntifs.h>
#include <windef.h>

using f_set_hvm_event = NTSTATUS(*)();

namespace klhk
{
	// 判断klhk驱动是否加载
	bool is_klhk_loaded();

	// 初始化
	bool initialize();

	// hvm初始化
	bool hvm_init();

	// 获取ssdt数量
	unsigned int get_svc_count_ssdt();

	// 获取shadow ssdt数量
	unsigned int get_svc_count_shadow_ssdt();

	// 挂钩指定索引的ssdt函数
	bool hook_ssdt_routine(ULONG index, void* dest, void** poriginal);

	// 还原指定索引的ssdt函数
	bool unhook_ssdt_routine(ULONG index, void* original);

	// 挂钩指定索引的shadow ssdt函数
	bool hook_shadow_ssdt_routine(ULONG index, void* dest, void** poriginal);

	// 还原指定索引的shadow ssdt函数
	bool unhook_shadow_ssdt_routine(ULONG index, void* original);

	// 获取指定索引的ssdt函数地址
	void* get_ssdt_routine(ULONG index);

	// 获取指定索引的shadow ssdt函数地址
	void* get_shadow_ssdt_routine(ULONG index);
}