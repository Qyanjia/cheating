#pragma warning(disable : 4244)
#include "Utils.h"

Util::Util() : m_Handle(INVALID_HANDLE_VALUE), m_ProcessID(0)
{
}

Util::~Util()
{
	if (m_Handle != INVALID_HANDLE_VALUE) CloseHandle(m_Handle);
}

BOOL Util::InitDriver(CONST WCHAR* DriverPath)
{
	return KbLoader::KbLoadAsDriver(DriverPath);
}

BOOL Util::OpenWinProcess(DWORD Process)
{
	if (m_Handle != INVALID_HANDLE_VALUE) CloseHandle(m_Handle);
	m_Handle = INVALID_HANDLE_VALUE;

	// 打开进程,注意这里要求的权限
	m_Handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, Process);
	if (m_Handle == INVALID_HANDLE_VALUE || m_Handle == NULL)
	{
		MessageBoxA(NULL, "无法打开该进程句柄,请尝试以管理员方式运行", "错误", MB_OK | MB_ICONHAND);
		return FALSE;
	}

	// 保存进程ID
	m_ProcessID = Process;

	return TRUE;
}

BOOL Util::OpenWinProcess(CONST WCHAR* ProcessName)
{
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Snap == INVALID_HANDLE_VALUE) return FALSE;

	PROCESSENTRY32W ProcessInfo{ 0 };
	ProcessInfo.dwSize = sizeof(ProcessInfo);

	BOOL State = Process32First(Snap, &ProcessInfo);
	while (State)
	{
		if (wcscmp(ProcessInfo.szExeFile, ProcessName) == 0)
		{
			CloseHandle(Snap);
			return OpenWinProcess(ProcessInfo.th32ProcessID);
		}

		State = Process32Next(Snap, &ProcessInfo);
	}

	CloseHandle(Snap);
	return FALSE;
}

DWORD64 Util::PatternCheck(
	BYTE* Region, /* 内存区域 */
	DWORD RegionSize, /* 内存区域大小 */
	BYTE* Pattern, /* 模式 */
	DWORD PatternSize, /* 模式大小 */
	DWORD StartOffset /*= 0*/)
{
	for (DWORD i = StartOffset; i < RegionSize - PatternSize; i++)
	{
		BOOL State = TRUE;
		for (DWORD j = 0; j < PatternSize; j++)
		{
			// 掩码
			if (Pattern[j] == '?') continue;

			if (Pattern[j] != Region[i + j])
			{
				State = FALSE;
				break;
			}
		}
		if (State) return i;
	}
	return -1;
}

//BYTE Pattern[] = { 0x91, 0x03, 0x7C, 0x5B, '?', 0x6B, 0xBC, 0xB6, 0xC3, 0x6B };
VOID Util::PatternSearch(
	BYTE* Pattern, /* 模式 */
	DWORD PatternSize, /* 模式大小 */
	std::vector<DWORD64>& Result, /* 查找结果 */
	DWORD64 StartAddr, /* 查找的开始地址 */
	DWORD64 StopAddr)
{
	// 地址区间遍历
	while (StartAddr < StopAddr)
	{
		// 内存查询
		MEMORY_BASIC_INFORMATION Info{ 0 };
		if (VirtualQueryEx(m_Handle, (LPCVOID)StartAddr, &Info, sizeof(Info)))
		{
			// 地址向前移动
			StartAddr = (DWORD64)Info.BaseAddress + Info.RegionSize;

			// 内存区域的属性问题
			BOOL State = (Info.State == MEM_COMMIT)
				&& (Info.Protect != PAGE_NOACCESS)
				&& (Info.Protect & PAGE_GUARD) == 0
				&& (Info.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE;
			if (State == FALSE) continue;

			// 读取内存区域
			DWORD64 Base = (DWORD64)Info.BaseAddress;
			BYTE* Buffer = ReadByte(Base, Info.RegionSize);
			if (Buffer)
			{
				DWORD Offset = 0;
				while (TRUE)
				{
					// 模式匹配
					DWORD64 Pos = PatternCheck(Buffer, Info.RegionSize, Pattern, PatternSize, Offset);
					if (Pos == -1) break;

					// 保存地址
					Result.push_back(Base + Pos);

					// 地址向前移动
					Offset = Pos + 1;
				}

				delete[] Buffer;
			}
		}
		else
		{
			MessageBoxA(NULL, "内存区域访问失败", "错误", MB_OK | MB_ICONHAND);
			break;
		}
	}
}

VOID Util::PatternSearchX32(
	BYTE* Pattern, /* 模式 */
	DWORD PatternSize, /* 模式大小 */
	std::vector<DWORD64>& Result, /* 查找结果 */
	DWORD64 StartAddr /*= 0x00010000*/, /* 查找的开始地址 */
	DWORD64 StopAddr /*= 0x7ffeffff*/)
{
	return PatternSearch(Pattern, PatternSize, Result, StartAddr, StopAddr);
}

VOID Util::PatternSearchX64(
	BYTE* Pattern, /* 模式 */
	DWORD PatternSize, /* 模式大小 */
	std::vector<DWORD64>& Result, /* 查找结果 */
	DWORD64 StartAddr /*= 0x0000000000010000*/, /* 查找的开始地址 */
	DWORD64 StopAddr /*= 0x00007ffffffeffff*/)
{
	return PatternSearch(Pattern, PatternSize, Result, StartAddr, StopAddr);
}

CHAR* Util::ReadCharStr(DWORD64 Addr, DWORD Size)
{
	CHAR* Buff = new CHAR[Size];
	if (Buff)
	{
		memset(Buff, 0, Size);
		Processes::MemoryManagement::KbReadProcessMemory(m_ProcessID, (WdkTypes::PVOID)Addr, Buff, Size);
	}
	return Buff;
}

WCHAR* Util::ReadWCharStr(DWORD64 Addr, DWORD Size)
{
	WCHAR* Buff = new WCHAR[Size];
	if (Buff)
	{
		memset(Buff, 0, Size * 2);
		Processes::MemoryManagement::KbReadProcessMemory(m_ProcessID, (WdkTypes::PVOID)Addr, Buff, Size);
	}
	return Buff;
}

BYTE* Util::ReadByte(DWORD64 Addr, DWORD Size)
{
	BYTE* Buff = new BYTE[Size];
	if (Buff)
	{
		memset(Buff, 0, Size);
		Processes::MemoryManagement::KbReadProcessMemory(m_ProcessID, (WdkTypes::PVOID)Addr, Buff, Size);
	}
	return Buff;
}

VOID Util::WriteCharStr(DWORD64 Addr, CONST CHAR* Buff, DWORD Size)
{
	Processes::MemoryManagement::KbWriteProcessMemory(m_ProcessID, (WdkTypes::PVOID)Addr, (PVOID)Buff, Size, FALSE);
}

VOID Util::WriteWCharStr(DWORD64 Addr, CONST WCHAR* Buff, DWORD Size)
{
	Processes::MemoryManagement::KbWriteProcessMemory(m_ProcessID, (WdkTypes::PVOID)Addr, (PVOID)Buff, Size, FALSE);
}