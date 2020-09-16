#include "overlay.hpp"

INT WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	HWND hWnd = FindWindowA("Valve001", "Counter-Strike: Global Offensive");
	assert(hWnd && "无法找到CSGO窗口句柄");

	overlay* g = new overlay(hWnd);
	g->create_overlay_window();
	g->initialize();
	g->message_handle();

	return 0;
}