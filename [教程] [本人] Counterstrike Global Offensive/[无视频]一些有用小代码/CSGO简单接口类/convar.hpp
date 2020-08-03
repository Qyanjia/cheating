#pragma once

#include <Windows.h>

class convar
{
private:
	/* 获取名称哈希值 */
	int get_name_hash(const char* name)
	{
		const int len = 256;
		int data[len]{ 0 };
		SIZE_T bytes = 0;
		ReadProcessMemory(m_handle, (LPCVOID)(m_vstdlib + m_convar_name_hash_table), data, sizeof(data), &bytes);
		if (bytes == 0) return 0;

		int v2 = 0, v3 = 0, size = strlen(name);
		for (int i = 0; i < size; i += 2)
		{
			v3 = data[v2 ^ toupper(name[i])];
			if (i + 1 == size) break;
			v2 = data[v3 ^ toupper(name[i + 1])];
		}
		return v2 | (v3 << 8);
	}

public:
	HANDLE m_handle;									//进程句柄
	DWORD m_vstdlib;									//基址
	DWORD m_convar_name_hash_table;	//哈希表地址
	DWORD m_interface_engine_cvar;			//引擎接口地址

	convar() {}
	~convar() {}

	DWORD get_convar_address(const char* name)
	{
		int hash = get_name_hash(name);
		if (hash == 0) return 0;

		SIZE_T bytes = 0;
		DWORD convar_engine = 0;
		ReadProcessMemory(m_handle, (LPCVOID)(m_vstdlib + m_interface_engine_cvar), &convar_engine, sizeof(convar_engine), &bytes);
		if (convar_engine == 0) return 0;

		DWORD address = 0;
		ReadProcessMemory(m_handle, (LPCVOID)(convar_engine + 0x34), &address, sizeof(address), &bytes);
		if (address == 0) return 0;

		DWORD target = 0;
		ReadProcessMemory(m_handle, (LPCVOID)(address + ((unsigned char)hash * 4)), &target, sizeof(target), &bytes);
		if (target == 0) return 0;

		while (target)
		{
			int temp = 0;
			ReadProcessMemory(m_handle, (LPCVOID)target, &temp, sizeof(temp), &bytes);
			if (temp == hash)
			{
				DWORD convar_point = 0;
				ReadProcessMemory(m_handle, (LPCVOID)(target + 0x4), &convar_point, sizeof(convar_point), &bytes);
				if (convar_point)
				{
					DWORD same = 0;
					ReadProcessMemory(m_handle, (LPCVOID)(convar_point + 0xC), &same, sizeof(same), &bytes);
					if (same)
					{
						char buf[32]{ 0 };
						ReadProcessMemory(m_handle, (LPCVOID)same, buf, sizeof(buf), &bytes);
						if (strcmp(buf, name) == 0) return convar_point;
					}
				}
			}

			ReadProcessMemory(m_handle, (LPCVOID)(target + 0xC), &target, sizeof(target), &bytes);
		}

		return 0;
	}

	void set_convar_value(const char* name, DWORD value)
	{
		DWORD addr = get_convar_address(name);
		if (addr)
		{
			SIZE_T bytes = 0;
			DWORD temp = value ^ addr;
			WriteProcessMemory(m_handle, (LPVOID)(addr + 0x30), &temp, sizeof(temp), &bytes);
		}
	}
};