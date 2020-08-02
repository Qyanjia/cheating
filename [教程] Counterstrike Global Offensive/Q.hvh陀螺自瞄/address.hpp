#pragma once

#include <Windows.h>

#include <WinInet.h>
#pragma comment(lib,"Wininet.lib")

#include <iostream>
#include <string>
#include <sstream>

class address
{
private:
	std::string m_data;

public:
	address() {}
	~address() { }

	/* 初始化,下载网页数据 */
	bool Initialize(const char* url = "https://github.com/frk1/hazedumper/blob/master/csgo.hpp")
	{
		/* 检查网页路径是否有效 */
		int size = strlen(url);
		if (size <= 0) return false;

		/* 打开一个类似管理器的句柄 */
		HINTERNET manager = InternetOpenA("CSGO Web", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
		if (manager == nullptr) return false;

		/* 尝试连接该网页 */
		HINTERNET target = InternetOpenUrlA(manager, url, nullptr, 0, INTERNET_FLAG_RELOAD, 0);
		if (target == nullptr)
		{
			InternetCloseHandle(manager);
			return false;
		}

		/* 将网页的全部数据读取出来 */
		m_data = "";
		DWORD bytes = 0;
		do
		{
			static const int len = 1024 * 10;
			char buffer[len]{ 0 };
			InternetReadFile(target, buffer, len, &bytes);
			m_data += buffer;
		} while (bytes);

		/* 关闭句柄 */
		InternetCloseHandle(manager);
		InternetCloseHandle(target);
		return true;
	}

	/* 获取指定字段的地址 */
	int get_address(const char* name)
	{
		/* 字段有效性检查 */
		int size = strlen(name);
		if (size <= 0) return 0;

		/* 网页数据有效性检查 */
		if (m_data.empty()) return 0;

		/* 字段查找 */
		size_t pos = m_data.find(name);
		if (pos == std::string::npos) return 0;

		size_t beg = m_data.find("0x", pos);
		size_t end = m_data.find('<', beg);
		if (beg == ::std::string::npos || end == std::string::npos) return 0;
		std::string buf = m_data.substr(beg, end - beg);

		/* 转换为十六进制数据 */
		int addr = 0;
		std::stringstream translater;
		translater << buf;
		translater >> std::hex >> addr;
		return addr;
	}
};