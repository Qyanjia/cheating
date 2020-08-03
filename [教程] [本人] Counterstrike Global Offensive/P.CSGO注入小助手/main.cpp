/*
CSGO注入小工具
原理 : 修复NtOpenFile函数的inline hook
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <filesystem>

#include <Windows.h>
#include <TlHelp32.h>

/* 获取CSGO进程句柄 */
HANDLE GetProcessHandle()
{
	/* 打开遍历进程句柄 */
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Snap == INVALID_HANDLE_VALUE) return Snap;

	/* 初始化进程信息结构 */
	PROCESSENTRY32 ProcessInfo{ 0 };
	ProcessInfo.dwSize = sizeof(ProcessInfo);

	/* 循环查找进程信息 */
	if (Process32First(Snap, &ProcessInfo))
	{
		do
		{
			if (strcmp("csgo.exe", ProcessInfo.szExeFile) == 0)
			{
				CloseHandle(Snap);
				return OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInfo.th32ProcessID);
			}
		} while (Process32Next(Snap, &ProcessInfo));
	}

	CloseHandle(Snap);
	return INVALID_HANDLE_VALUE;
}

int main(int argc, char* argv[])
{
	HANDLE Handle = INVALID_HANDLE_VALUE;
	LPVOID DllMemory = nullptr;
	HANDLE Thread = NULL;

	do
	{
		/* 获取作弊DLL路径 filesystem报错请设置C++17版本*/
		std::cout << "输入DLL全路径 : ";
		std::string dll;
		getline(std::cin, dll);
		if (std::filesystem::exists(dll) == false)
		{
			std::cout << "[-] DLL文件不存在" << std::endl;
			break;
		}

		/* 获取CSGO进程句柄 */
		Handle = GetProcessHandle();
		if (Handle == INVALID_HANDLE_VALUE)
		{
			std::cout << "[-] 获取游戏进程句柄失败" << std::endl;
			break;
		}

		/* 获取NtOpenFile函数地址 */
		LPVOID pNtOpenFile = GetProcAddress(LoadLibraryA("ntdll"), "NtOpenFile");
		if (pNtOpenFile == nullptr)
		{
			std::cout << "[-] 获取NtOpenFile函数地址失败" << std::endl;
			break;
		}

		/* 获取原始字节 */
		char OriginalBytes[5]{ 0 };
		memcpy(OriginalBytes, pNtOpenFile, 5);

		/* 将原始字节写入CSGO进程的NtOpenFile函数地址处,解除inline hook */
		SIZE_T Bytes = 0;
		WriteProcessMemory(Handle, pNtOpenFile, OriginalBytes, 5, &Bytes);
		if (Bytes == 0)
		{
			std::cout << "[-] 无法写入内存解除inline hook" << std::endl;
			break;
		}

		/* 在CSGO游戏中申请一块内存保存我们的DLL路径 */
		DllMemory = VirtualAllocEx(Handle, nullptr, dll.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (DllMemory == nullptr)
		{
			std::cout << "[-] 无法在CSGO游戏内申请内存" << std::endl;
			break;
		}

		/* 把我们的DLL路径写入该块内存中 */
		WriteProcessMemory(Handle, DllMemory, dll.c_str(), dll.size(), &Bytes);
		if (Bytes == 0)
		{
			std::cout << "[-] 无法写入DLL路径" << std::endl;
			break;
		}

		/* 创建远程线程加载我们的DLL */
		Thread = CreateRemoteThread(Handle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, DllMemory, 0, 0);
		if (Thread == NULL)
		{
			std::cout << "[-] 无法创建远程线程" << std::endl;
			break;
		}

		/* 等待远程线程的执行完毕 */
		WaitForSingleObject(Thread, INFINITE);

		/* 加载成功 */
		std::cout << "[+] 注入成功" << std::endl;
	} while (false);

	/* 清理相关 */
	if (Handle != INVALID_HANDLE_VALUE) CloseHandle(Handle);
	if (DllMemory) VirtualFree(DllMemory, 0, MEM_RELEASE);
	if (Thread != NULL) CloseHandle(Thread);

	system("pause");
	return 0;
}