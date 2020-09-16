#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <assert.h>

/*
作弊类
*/

constexpr DWORD dwLocalPlayer = 0xD3AC5C;
constexpr DWORD dwEntityList = 0x4D4F1FC;
constexpr DWORD m_iHealth = 0x100;
constexpr DWORD m_dwBoneMatrix = 0x26A8;
constexpr DWORD dwViewMatrix = 0x4D40B44;

class cheat
{
private:
	DWORD m_pid;
	HANDLE m_process;

public:
	cheat() {}
	~cheat() {}

	/* 获取CSGO的进程句柄和ID */
	bool get_process(const char* process = "csgo.exe")
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

					/* 保存进程ID */
					m_pid = ProcessInfo.th32ProcessID;

					/* 保存进程句柄 !!!!危险的方式!!!!! */
					m_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);

					return m_pid && m_process != INVALID_HANDLE_VALUE;
				}
			} while (Process32Next(Snap, &ProcessInfo));
		}

		CloseHandle(Snap);
		return false;
	}

	/* 查找指定模块 */
	DWORD find_module(const char* name)
	{
		MODULEENTRY32 Result{ 0 };
		Result.dwSize = sizeof(Result);

		HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_pid);
		if (Snap == INVALID_HANDLE_VALUE) return 0;

		if (Module32First(Snap, &Result))
		{
			do
			{
				if (strcmp(name, Result.szModule) == 0)
				{
					CloseHandle(Snap);
					return (DWORD)Result.modBaseAddr;
				}
			} while (Module32Next(Snap, &Result));
		}

		CloseHandle(Snap);
		return 0;
	}

	/* 读取内存 */
	template<class T>
	T read(DWORD addr)
	{
		T result{  };
		ReadProcessMemory(m_process, (LPCVOID)addr, (LPVOID)&result, sizeof(T), nullptr);
		return result;
	}

	/* 写入内存 */
	template<class T>
	void write(DWORD addr, T buffer)
	{
		WriteProcessMemory(m_process, (LPVOID)addr, (LPCVOID)&buffer, sizeof(T), nullptr);
	}

	/* 获取骨骼位置 */
	D3DXVECTOR3 get_bone_pos(DWORD base, int index)
	{
		typedef struct {
			float Matrix[3][4];
		} Matrix3x4_t;

		DWORD addr = read<DWORD>(base + m_dwBoneMatrix);
		if (addr)
		{
			Matrix3x4_t result = read<Matrix3x4_t>(addr + index * 0x30);

			return{
				result.Matrix[0][3],
				result.Matrix[1][3],
				result.Matrix[2][3]
			};
		}

		return { 0,0,0 };
	}

	/* 转化为矩阵信息 */
	bool to_rect_info(float matrix[][4], float* location, int window_width, int window_heigt, int& x, int& y, int& w, int& h)
	{
		float to_target = matrix[2][0] * location[0]
			+ matrix[2][1] * location[1]
			+ matrix[2][2] * location[2]
			+ matrix[2][3];
		if (to_target < 0.01f)
		{
			x = y = w = h = 0;
			return false;
		}
		to_target = 1.0f / to_target;

		float to_width = window_width + (matrix[0][0] * location[0]
			+ matrix[0][1] * location[1]
			+ matrix[0][2] * location[2]
			+ matrix[0][3]) * to_target * window_width;

		float to_height_h = window_heigt - (matrix[1][0] * location[0]
			+ matrix[1][1] * location[1]
			+ matrix[1][2] * (location[2] /*+ 75.0f*/)
			+ matrix[1][3]) * to_target * window_heigt;

		float to_height_w = window_heigt - (matrix[1][0] * location[0]
			+ matrix[1][1] * location[1]
			+ matrix[1][2] * (location[2]/* - 5.0f*/)
			+ matrix[1][3]) * to_target * window_heigt;

		x = (int)(to_width - (to_height_w - to_height_h) / 4.0f);
		y = (int)(to_height_h);
		w = (int)((to_height_w - to_height_h) / 2.0f);
		h = (int)(to_height_w - to_height_h);
		return true;
	}
};