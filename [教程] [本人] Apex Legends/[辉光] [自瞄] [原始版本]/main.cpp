#include "overlay.hpp"

#include <process.h>

#include <iostream>
#include <string>

bool g_thread = true;

void _cdecl glow_thread(void* data)
{
	while (g_thread)
	{
		// 辉光
		glow_players(true);

		Sleep(1);
	}

	glow_players(false);
	return;
}

void _cdecl aim_thread(void* data)
{
	while (g_thread)
	{
		// 自瞄玩家
		bool state = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_CONTROL) & 0x8000);
		if (state) aimbot_players();

		Sleep(1);
	}
}

int main(int argc, char* argv[])
{
	HANDLE hDriver = open_device();
	if (hDriver == 0) return 0;

	if (initialize() == false) return 0;

	std::cout << _beginthread(glow_thread, 0, nullptr) << std::endl;
	std::cout << _beginthread(aim_thread, 0, nullptr) << std::endl;

	while (true)
	{
		std::cout << "wait for you input : " << std::endl;
		std::string line;
		getline(std::cin, line);

		if (line.size())
			if (line[0] == 'q' || line[0] == 'Q') break;
	}

	g_thread = false;

	// 关闭辉光
	glow_players(false);

	return 0;
}

int __stdcall WinMain__(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	HANDLE hDriver = open_device();
	if (hDriver == 0) return 0;

	HWND hWnd = FindWindowA("Respawn001", "Apex Legends");
	if (hWnd)
	{
		overlay* g = new overlay(hWnd);
		g->create_overlay_window();
		g->initialize();
		g->message_handle();
	}

	return 0;
}