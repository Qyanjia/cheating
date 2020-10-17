#pragma once

#include "util.hpp"

#include <ntifs.h>

/* LDR表结构 */
typedef struct _KLDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY	 InLoadOrderLinks;
	void		 *ExceptionTable;
	unsigned int	 ExceptionTableSize;
	void		 *GpValue;
	void		 *NonPagedDebugInfo;
	void	     *DllBase;										//基址
	void		 *EntryPoint;								//入口点
	unsigned int	 SizeOfImage;					//映像大小
	UNICODE_STRING	 FullDllName;			//全路径
	UNICODE_STRING	 BaseDllName;			//名称
	unsigned int	 Flags;
	unsigned __int16 LoadCount;
	unsigned __int16 u1;
	void	         *SectionPointer;
	unsigned int	 CheckSum;
	unsigned int	 CoverageSectionSize;
	void		 *CoverageSection;
	void		 *LoadedImports;
	void		 *Spare;
	unsigned int	 SizeOfImageNotRounded;
	unsigned int	 TimeDateStamp;
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

namespace modules
{
	/* 根据名称获取LDR模块 */
	PKLDR_DATA_TABLE_ENTRY get_ldr_data_by_name(const wchar_t* szmodule);

	/* 根据名称获取基址 */
	uintptr_t get_kernel_module_base(const wchar_t* szmodule);
}

EXTERN_C PLIST_ENTRY PsLoadedModuleList;