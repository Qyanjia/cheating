/*
用户层连接IOCTL
*/

#include <iostream>
#include <thread>

#include <Windows.h>
#include <TlHelp32.h>

/* 读取 */
#define IOCTL_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

/* 写入 */
#define IOCTL_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x998, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

/* 数据结构 */
struct DataStruct
{
	HANDLE PID;			//进程ID
	PVOID64 Addr;		//地址
	PVOID64 Result;		//结果
	SIZE_T Size;				//大小
};

class IOCTL
{
private:
	DWORD m_PID;

public:
	IOCTL() {}
	~IOCTL() {}

	/* 附加指定进程 */
	bool attach(const char* process)
	{
		HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Snap == INVALID_HANDLE_VALUE) return false;

		PROCESSENTRY32 ProcessInfo{ 0 };
		ProcessInfo.dwSize = sizeof(ProcessInfo);

		if (Process32First(Snap, &ProcessInfo))
		{
			do
			{
				if (strcmp(process, ProcessInfo.szExeFile) == 0)
				{
					CloseHandle(Snap);
					m_PID = ProcessInfo.th32ProcessID;
					return true;
				}
			} while (Process32Next(Snap, &ProcessInfo));
		}

		CloseHandle(Snap);
		return false;
	}

	/* 查找指定模块 */
	MODULEENTRY32 find_module(const char* name)
	{
		MODULEENTRY32 Result{ 0 };
		Result.dwSize = sizeof(Result);

		HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_PID);
		if (Snap == INVALID_HANDLE_VALUE) return Result;

		if (Module32First(Snap, &Result))
		{
			do
			{
				if (strcmp(name, Result.szModule) == 0)
				{
					CloseHandle(Snap);
					return Result;
				}
			} while (Module32Next(Snap, &Result));
		}

		CloseHandle(Snap);
		return {};
	}

	/* 连接到内核设备 */
	HANDLE connect(const char* name)
	{
		return CreateFile(name, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	}

	/* 读取内存 */
	template<class T>
	T read(uintptr_t addr)
	{
		T Result;

		DataStruct Data{ 0 };
		Data.PID = (HANDLE)m_PID;
		Data.Addr = (PVOID64)addr;
		Data.Result = &Result;
		Data.Size = sizeof(Result);

		HANDLE Device = connect("\\\\.\\Driver_IOCTL");
		if (Device != INVALID_HANDLE_VALUE)
		{
			DWORD Tips = 0;
			DeviceIoControl(Device, IOCTL_READ, &Data, sizeof(Data), &Result, sizeof(Result), &Tips, NULL);
			CloseHandle(Device);
		}
		else std::cout << "发生错误 : " << GetLastError() << std::endl;

		return Result;
	}

	/* 写入内存 */
	template<class T>
	void write(uintptr_t addr, T Buf)
	{
		DataStruct Data{ 0 };
		Data.PID = (HANDLE)m_PID;
		Data.Addr = (PVOID64)addr;
		Data.Result = &Buf;
		Data.Size = sizeof(Buf);

		HANDLE Device = connect("\\\\.\\Driver_IOCTL");
		if (Device != INVALID_HANDLE_VALUE)
		{
			DWORD Tips = 0;
			DeviceIoControl(Device, IOCTL_WRITE, &Data, sizeof(Data), &Buf, sizeof(Buf), &Tips, NULL);
			CloseHandle(Device);
		}
		else std::cout << "发生错误 : " << GetLastError() << std::endl;
	}
};

void test_read()
{
	IOCTL* g = new IOCTL();
	if (g->attach("Target.exe"))
	{
		MODULEENTRY32 mod = g->find_module("Target.exe");
		DWORD addr = (DWORD)mod.modBaseAddr;

		for (int i = 0; i < 1000000; i++) g->read<int>(addr + 0x1C000);
	}
}

void test_write()
{
	IOCTL* g = new IOCTL();
	if (g->attach("Target.exe"))
	{
		MODULEENTRY32 mod = g->find_module("Target.exe");
		DWORD addr = (DWORD)mod.modBaseAddr;

		for (int i = 0; i < 1000000; i++) g->write<int>(addr + 0x1C000, 200);
	}
}

int main(int argc, char* argv[])
{
	IOCTL* g = new IOCTL();
	if (g->attach("Target.exe"))
	{
		MODULEENTRY32 mod = g->find_module("Target.exe");
		uintptr_t addr = (uintptr_t)mod.modBaseAddr;

		if (addr)
		{
			std::cout << "[+] 模块基址 : " << std::hex << addr << std::dec << std::endl;

			int res = g->read<int>(addr + 0x1C000);
			std::cout << "[+] 读取到的数据为 : " << res << std::endl;

			std::cout << "[+] 尝试修改数据" << std::endl;
			g->write<int>(addr + 0x1C000, 456);

			res = g->read<int>(addr + 0x1C000);
			std::cout << "[+] 修改后,再次读取到的数据为 : " << res << std::endl;
		}
	}
	else std::cout << "[+] 不能查找到指定进程" << std::endl;
	system("pause");
	return 0;
}