#pragma once

#include<ntifs.h>
#include<windef.h>

/* 调试辅助函数 */
#define printfs(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

/* 读取内存 */
#define Mdl_Read CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ALL_ACCESS)

/* 写入内存 */
#define Mdl_Write CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ALL_ACCESS)

/* 传递信息的结构 */
typedef struct _UserData
{
	DWORD Pid;							//要读写的进程ID
	DWORD64 Address;				//要读写的地址
	DWORD Size;							//读写长度
	PBYTE Data;								//要读写的数据
}UserData, *PUserData;
