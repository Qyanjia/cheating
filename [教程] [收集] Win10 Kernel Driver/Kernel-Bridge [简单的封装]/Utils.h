#pragma once

/************************************************************************/
/*								整合一下经常用到的函数                                        */
/************************************************************************/

#include "User-Bridge.h"

#include <tlhelp32.h>

#include <iostream>
#include <vector>

class Util
{
private:
	// 进程ID
	DWORD m_ProcessID;

	// 进程句柄
	HANDLE m_Handle;

public:
	Util();
	~Util();

public:
	/* 初始化驱动 */
	BOOL InitDriver(CONST WCHAR* DriverPath);

	/* 打开一个进程 */
	BOOL OpenWinProcess(CONST WCHAR* ProcessName);
	BOOL OpenWinProcess(DWORD Process);

	/* 模式匹配 */
	DWORD64 PatternCheck(
		BYTE* Region,							// 内存区域
		DWORD RegionSize,				// 内存区域大小
		BYTE* Pattern,							// 模式
		DWORD PatternSize,				// 模式大小
		DWORD StartOffset = 0);			// 开始的位置偏移

	/* 模式查找 */
	VOID PatternSearch(
		BYTE* Pattern,									// 模式
		DWORD PatternSize,						// 模式大小
		std::vector<DWORD64>& Result,	// 查找结果
		DWORD64 StartAddr,						// 查找的开始地址
		DWORD64 StopAddr);						// 查找的结束地址

	/* 32位的模式查找 */
	VOID PatternSearchX32(
		BYTE* Pattern,											// 模式
		DWORD PatternSize,								// 模式大小
		std::vector<DWORD64>& Result,			// 查找结果
		DWORD64 StartAddr = 0x00010000,		// 查找的开始地址
		DWORD64 StopAddr = 0xFFFFFFFF);		// 查找的结束地址

	/* 64位的模式查找 */
	VOID PatternSearchX64(
		BYTE* Pattern,															// 模式
		DWORD PatternSize,												// 模式大小
		std::vector<DWORD64>& Result,							// 查找结果
		DWORD64 StartAddr = 0x0000000000010000,			// 查找的开始地址
		DWORD64 StopAddr = 0x00007FFFFFFFFFFF);			// 查找的结束地址

	/* 读取字符串 */
	CHAR* ReadCharStr(DWORD64 Addr, DWORD Size);
	WCHAR* ReadWCharStr(DWORD64 Addr, DWORD Size);

	/* 读取字节 */
	BYTE* ReadByte(DWORD64 Addr, DWORD Size);

	/* 写入字符串 */
	VOID WriteCharStr(DWORD64 Addr, CONST CHAR* Buff, DWORD Size);
	VOID WriteWCharStr(DWORD64 Addr, CONST WCHAR* Buff, DWORD Size);

	/* 读取内存 */
	template<class T>
	T Read(DWORD64 Addr)
	{
		T Res{};
		Processes::MemoryManagement::KbReadProcessMemory(m_ProcessID, (WdkTypes::PVOID)Addr, &Res, sizeof(Res));
		return Res;
	}

	/* 写入内存 */
	template<class T>
	VOID Write(DWORD64 Addr, T Buff)
	{
		Processes::MemoryManagement::KbWriteProcessMemory(m_ProcessID, (WdkTypes::PVOID)Addr, &Buff, sizeof(Buff), FALSE);
	}
};
