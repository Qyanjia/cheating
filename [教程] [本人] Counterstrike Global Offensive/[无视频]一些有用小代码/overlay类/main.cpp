#include "overlay.hpp"

int __stdcall WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	HWND hWnd = FindWindowA(nullptr, "ShadowVolume");

	overlay* g = new overlay(hWnd);
	g->create_overlay_window();
	g->initialize();
	g->message_handle();

	return 0;
}