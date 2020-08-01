/*
CSGO基址和偏移获取软件
*/

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
			char buffer[1024]{ 0 };
			InternetReadFile(target, buffer, 1024, &bytes);
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

int main(int argc, char* argv[])
{
	address* g = new address();
	if (g->Initialize())
	{
		std::cout << std::hex;

		std::cout << "人物骨骼 m_dwBoneMatrix : 0x" << g->get_address("m_dwBoneMatrix") << std::endl;
		std::cout << "当前武器 m_hActiveWeapon : 0x" << g->get_address("m_hActiveWeapon") << std::endl;
		std::cout << "辉光索引 m_iGlowIndex : 0x" << g->get_address("m_iGlowIndex") << std::endl;

		std::cout << std::endl;

		std::cout << "游戏目录 dwClientState_MapDirectory : 0x" << g->get_address("dwClientState_MapDirectory") << std::endl;
		std::cout << "游戏角度 dwClientState_ViewAngles : 0x" << g->get_address("dwClientState_ViewAngles") << std::endl;
		std::cout << "玩家列表 dwEntityList : 0x" << g->get_address("dwEntityList") << std::endl;
	}
	system("pause");
	return 0;
}