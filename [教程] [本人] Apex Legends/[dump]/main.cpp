#include "memory.hpp"

#include <iostream>
#include <string>
#include <regex>

/*
APEX Dump
*/

void dump()
{
	HANDLE driver = open_device();
	if (driver == INVALID_HANDLE_VALUE)
	{
		std::cout << "[-] 驱动程序未加载" << std::endl;
		return;
	}

	DWORD32 pid = 0;
	std::cout << "[+] 输入APEX进程ID : ";
	std::cin >> pid;

	DWORD64 base = 0;
	std::cout << "[+] 输入APEX进程基址 : ";
	std::cin >> base;

	std::cout << "[+] 目标进程 : " << pid << "\t 目标基址 : 0x" << std::hex << base << std::endl;

	// 读取dos头
	IMAGE_DOS_HEADER dos = read<IMAGE_DOS_HEADER>(pid, base);
	if (dos.e_magic != IMAGE_DOS_SIGNATURE)
	{
		std::cout << "[-] DOS头有误" << std::endl;
		return;
	}
	else std::cout << "[+] DOS头偏移为 : 0x" << dos.e_lfanew << std::endl;

	// 读取NT头
	IMAGE_NT_HEADERS64 nt64 = read<IMAGE_NT_HEADERS64>(pid, base + dos.e_lfanew);
	if (nt64.Signature != IMAGE_NT_SIGNATURE || nt64.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		std::cout << "[-] NT64头有误" << std::endl;
		return;
	}

	std::cout << "[+] 时间戳 : 0x" << nt64.FileHeader.TimeDateStamp << std::endl;
	std::cout << "[+] 校验和 : 0x" << nt64.OptionalHeader.CheckSum << std::endl;
	std::cout << "[+] 映像大小 : 0x" << nt64.OptionalHeader.SizeOfImage << std::endl;

	const size_t target_len = nt64.OptionalHeader.SizeOfImage;
	uint8_t* data = read_array(pid, base, target_len);
	if (data == nullptr)
	{
		std::cout << "[-] 读取APEX内存失败" << std::endl;
		return;
	}

	// 获取NT指针
	PIMAGE_NT_HEADERS64 p_nt64 = reinterpret_cast<PIMAGE_NT_HEADERS64>(data + dos.e_lfanew);

	// 获取节区头
	PIMAGE_SECTION_HEADER p_section = reinterpret_cast<PIMAGE_SECTION_HEADER>(
		data +
		static_cast<size_t>(dos.e_lfanew) +
		static_cast<size_t>(FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader)) +
		static_cast<size_t>(nt64.FileHeader.SizeOfOptionalHeader));

	// 遍历节区
	for (size_t i = 0; i < nt64.FileHeader.NumberOfSections; i += 1)
	{
		//获取节区
		auto& section = p_section[i];

		// 虚拟地址
		section.PointerToRawData = section.VirtualAddress;
		section.SizeOfRawData = section.Misc.VirtualSize;

		// 重定位
		if (!memcmp(section.Name, ".reloc\0\0", 8))
		{
			p_nt64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC] =
			{
				section.VirtualAddress,
				section.Misc.VirtualSize,
			};
		}
	}

	// 创建一个文件
	HANDLE dump = CreateFileA("apex.bin", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_COMPRESSED, NULL);
	if (dump != INVALID_HANDLE_VALUE)
	{
		if (WriteFile(dump, data, target_len, NULL, NULL))
			std::cout << "[+] 写入成功" << std::endl;
		else
			std::cout << "[-] 写入失败" << std::endl;
		CloseHandle(dump);
	}

	// 释放内存
	delete[] data;

	std::cout << "操作完毕" << std::endl;
}

int main(int argc, char* argv[])
{
	dump();

	system("pause");
	return 0;
}