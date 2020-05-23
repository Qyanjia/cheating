#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include "offset.h"

using t_dword = DWORD;
using t_handle = HANDLE;

t_dword g_pid;
t_handle g_handle;

t_dword g_client;
t_dword g_client_size;

t_dword g_engine;
t_dword g_engine_size;

void error(const char* text)
{
	MessageBoxA(nullptr, text, nullptr, 0);
	exit(-1);
}

bool is_error()
{
	return GetLastError() != 0;
}

void open_process(t_dword pid)
{
	g_pid = pid;
	g_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (is_error()) error("打开进程句柄失败");
}

void get_moduel_info(const char* name, t_dword& address, t_dword& size)
{
	t_handle snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, g_pid);
	if (is_error()) error("");

	MODULEENTRY32 mod{ sizeof(MODULEENTRY32) };
	BOOL status = Module32First(snap, &mod);
	while (status)
	{
		if (strncmp(name, mod.szModule, strlen(name)) == 0)
		{
			address = (t_dword)mod.modBaseAddr;
			size = mod.modBaseSize;
			CloseHandle(snap);
			return;
		}
		status = Module32Next(snap, &mod);
	}
	CloseHandle(snap);
}

template<class T>
T read_memory(t_dword address)
{
	T buffer;
	SIZE_T finish;
	ReadProcessMemory(g_handle, (LPCVOID)address, &buffer, sizeof(T), &finish);
	return buffer;
}

template<class T>
void write_memory(t_dword address, T buffer)
{
	SIZE_T finish;
	WriteProcessMemory(g_handle, (LPVOID)address, &buffer, sizeof(T), &finish);
}

void glow()
{
	t_dword local_player = read_memory<t_dword>(g_client + hazedumper::signatures::dwLocalPlayer);
	if (local_player == 0) return;

	int team = read_memory<int>(local_player + hazedumper::netvars::m_iTeamNum);

	t_dword glow_manager = read_memory<t_dword>(g_client + hazedumper::signatures::dwGlowObjectManager);
	if (glow_manager == 0) return;

	for (int i = 0; i < 64; i++)
	{
		t_dword entity = read_memory<t_dword>(g_client + hazedumper::signatures::dwEntityList + i * 0x10);
		if (entity == 0 || entity == local_player) continue;

		int entity_team = read_memory<int>(entity + hazedumper::netvars::m_iTeamNum);
		int entity_glow_index = read_memory<int>(entity + hazedumper::netvars::m_iGlowIndex);

		float color = 0;
		if (team != entity_team) color = 2.0f;
		write_memory<float>(glow_manager + ((entity_glow_index * 0x38) + 0x4), color);
		write_memory<float>(glow_manager + ((entity_glow_index * 0x38) + 0x8), 2.0f);
		write_memory<float>(glow_manager + ((entity_glow_index * 0x38) + 0xc), 0);
		write_memory<float>(glow_manager + ((entity_glow_index * 0x38) + 0x10), 0.5f);

		write_memory<bool>(glow_manager + ((entity_glow_index * 0x38) + 0x24), true);
		write_memory<bool>(glow_manager + ((entity_glow_index * 0x38) + 0x25), false);
	}
}

void jump()
{
	t_dword local_player = read_memory<t_dword>(g_client + hazedumper::signatures::dwLocalPlayer);
	if (local_player == 0) return;

	int flag = read_memory<int>(local_player + hazedumper::netvars::m_fFlags);
	if (flag == 257 && GetAsyncKeyState(VK_SPACE) & 0x8000)
		write_memory<int>(g_client + hazedumper::signatures::dwForceJump, 6);
}

//https://github.com/frk1/hazedumper/blob/master/csgo.hpp
int main(int argc, char* argv[])
{
	open_process(14032);

	get_moduel_info("client_panorama.dll", g_client, g_client_size);
	get_moduel_info("engine.dll", g_engine, g_engine_size);

	while (true)
	{
		glow();
		jump();
		Sleep(1);
	}

	return 0;
}