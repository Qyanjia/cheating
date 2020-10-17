#include "util.hpp"
#include "module.hpp"

PIMAGE_SECTION_HEADER utils::get_section_header(const uintptr_t image_base, const char* section_name)
{
	// 参数判断
	if (!image_base || !section_name)
		return nullptr;

	// 获取dos头
	const auto pimage_dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);

	// 获取nt头
	const auto pimage_nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS64>(image_base + pimage_dos_header->e_lfanew);

	// 获取节区头
	auto psection = reinterpret_cast<PIMAGE_SECTION_HEADER>(pimage_nt_headers + 1);

	PIMAGE_SECTION_HEADER psection_hdr = nullptr;

	// 节区数量
	const auto NumberOfSections = pimage_nt_headers->FileHeader.NumberOfSections;

	// 遍历节区
	for (auto i = 0; i < NumberOfSections; ++i)
	{
		if (strstr(psection->Name, section_name))
		{
			psection_hdr = psection;
			break;
		}

		++psection;
	}

	return psection_hdr;
}

bool utils::data_compare(const char* pdata, const char* bmask, const char* szmask)
{
	for (; *szmask; ++szmask, ++pdata, ++bmask)
	{
		if (*szmask == 'x' && *pdata != *bmask)
			return false;
	}

	return !*szmask;
}

uintptr_t utils::find_pattern(const uintptr_t base, const size_t size, const char* bmask, const char* szmask)
{
	for (size_t i = 0; i < size; ++i)
		if (data_compare(reinterpret_cast<const char*>(base + i), bmask, szmask))
			return base + i;

	return 0;
}

uintptr_t utils::find_pattern_km(const wchar_t* szmodule, const char* szsection, const char* bmask, const char* szmask)
{
	// 参数判断
	if (!szmodule || !szsection || !bmask || !szmask)
		return 0;

	// 获取模块数据
	const auto* pldr_entry = modules::get_ldr_data_by_name(szmodule);
	if (!pldr_entry)
		return 0;

	// 获取基址
	const auto  module_base = reinterpret_cast<uintptr_t>(pldr_entry->DllBase);

	// 获取节区指针
	const auto* psection = get_section_header(reinterpret_cast<uintptr_t>(pldr_entry->DllBase), szsection);

	return psection ? find_pattern(module_base + psection->VirtualAddress, psection->VirtualSize, bmask, szmask) : 0;
}