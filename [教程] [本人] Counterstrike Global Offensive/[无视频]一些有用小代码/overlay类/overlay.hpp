#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

#include <d3dx9.h>
#pragma comment(lib,"d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <Windows.h>
#include <time.h>

#include <vector>

class overlay
{
private:
	IDirect3D9* m_IDirect3D9;
	IDirect3DDevice9* m_IDirect3DDevice9;

	ID3DXLine* m_ID3DXLine;
	ID3DXFont* m_ID3DXFont;

	D3DPRESENT_PARAMETERS m_D3DPRESENT_PARAMETERS;

	HWND m_hwnd;
	HWND m_game;

	/* 随机化字符串 */
	char* random_string()
	{
		static std::vector<char> maps{ 'q','w','e','r','t','y','u','i','o','p','l','k','j','h','g','f','d','s','a','z','x','c','v','b','n','m','Q','A','Z','W','S','X','E','D','C','R','F','V','T','G','B','Y','H','N','U','J','M','I','K','O','L','P','1','2','3','4','5','6','7','8','9','0' };
		static char buffer[100]{ 0 };

		srand((unsigned)time(nullptr));
		for (int i = 0; i < 30; i++) buffer[i] = maps[rand() % maps.size()];
		return buffer;
	}

	/* 窗口过程 */
	static LRESULT CALLBACK window_process(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static MARGINS margins{ 0 };

		switch (uMsg)
		{
		case WM_CREATE:
			DwmExtendFrameIntoClientArea(hWnd, &margins);
			return 1;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 1;
		}
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}

public:
	overlay(HWND hWnd) : m_game(hWnd) {}
	~overlay() {}

	/* 创建一个透明窗口 */
	bool create_overlay_window()
	{
		char sz_class[100]{ 0 }, sz_title[100]{ 0 };
		strcpy(sz_class, random_string());
		strcpy(sz_title, random_string());

		WNDCLASSEXA window_class{ 0 };
		window_class.cbSize = sizeof(window_class);
		window_class.hCursor = LoadCursor(0, IDC_ARROW);
		window_class.hInstance = GetModuleHandle(NULL);
		window_class.lpfnWndProc = window_process;
		window_class.lpszClassName = sz_class;
		window_class.style = CS_VREDRAW | CS_HREDRAW;
		if (RegisterClassExA(&window_class) == 0)
		{
			MessageBoxA(nullptr, "RegisterClassExA", "错误", MB_OK | MB_ICONHAND);
			exit(-1);
		}

		RECT rect{ 0 };
		GetWindowRect(m_game, &rect);
		int x = rect.left;
		int y = rect.top;
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		if (GetWindowLongA(m_game, GWL_STYLE) & WS_CAPTION)
		{
			x += 8;
			width -= 8;
			y += 30;
			height -= 30;
		}

		m_hwnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
			sz_class, sz_title, WS_POPUP, x, y, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
		if (m_hwnd == NULL)
		{
			MessageBoxA(nullptr, "CreateWindowExA", "错误", MB_OK | MB_ICONHAND);
			exit(-1);
		}

		SetLayeredWindowAttributes(m_hwnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
		UpdateWindow(m_hwnd);
		ShowWindow(m_hwnd, SW_SHOW);

		return true;
	}

	/* 消息循环 */
	void message_handle()
	{
		MSG msg{ 0 };
		while (msg.message != WM_QUIT)
		{
			if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
			else
			{
				RECT rect{ 0 };
				GetWindowRect(m_game, &rect);
				int x = rect.left;
				int y = rect.top;
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				if (GetWindowLongA(m_game, GWL_STYLE) & WS_CAPTION)
				{
					x += 8;
					width -= 8;
					y += 30;
					height -= 30;
				}

				MoveWindow(m_hwnd, x, y, width, height, TRUE);

				render();
			}
		}
	}

	/* 初始化 */
	bool initialize()
	{
		m_IDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_IDirect3D9 == nullptr)
		{
			MessageBoxA(nullptr, "Direct3DCreate9", "错误", MB_OK | MB_ICONHAND);
			exit(-1);
		}

		memset(&m_D3DPRESENT_PARAMETERS, 0, sizeof(m_D3DPRESENT_PARAMETERS));
		m_D3DPRESENT_PARAMETERS.Windowed = TRUE;
		m_D3DPRESENT_PARAMETERS.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_D3DPRESENT_PARAMETERS.BackBufferFormat = D3DFMT_UNKNOWN;
		m_D3DPRESENT_PARAMETERS.EnableAutoDepthStencil = TRUE;
		m_D3DPRESENT_PARAMETERS.AutoDepthStencilFormat = D3DFMT_D16;
		m_D3DPRESENT_PARAMETERS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		HRESULT result = m_IDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DPRESENT_PARAMETERS, &m_IDirect3DDevice9);
		if (result != D3D_OK)
		{
			MessageBoxA(nullptr, "CreateDevice", "错误", MB_OK | MB_ICONHAND);
			exit(-1);
		}

		result = D3DXCreateLine(m_IDirect3DDevice9, &m_ID3DXLine);
		if (result != D3D_OK)
		{
			MessageBoxA(nullptr, "D3DXCreateLine", "错误", MB_OK | MB_ICONHAND);
			exit(-1);
		}

		result = D3DXCreateFontA(m_IDirect3DDevice9, 20, 0, FW_DONTCARE, D3DX_DEFAULT, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial", &m_ID3DXFont);//Arial Vernada
		if (result != D3D_OK)
		{
			MessageBoxA(nullptr, "D3DXCreateFontA", "错误", MB_OK | MB_ICONHAND);
			exit(-1);
		}

		return true;
	}

	/* 渲染矩形 */
	void render_rect(float x, float y, float width, float height, D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 255), float size = 1.0f)
	{
		D3DXVECTOR2 vextor[5]{ {x,y},{x + width,y},{x + width,y + height},{x,y + height},{x,y} };
		m_ID3DXLine->SetWidth(size);
		m_ID3DXLine->Draw(vextor, 5, color);
	}

	/* 渲染文本 */
	void render_text(long x, long y, const char* text, D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 255))
	{
		RECT rect{ x,y };
		m_ID3DXFont->DrawTextA(nullptr, text, -1, &rect, DT_CALCRECT, color);
		m_ID3DXFont->DrawTextA(nullptr, text, -1, &rect, DT_LEFT, color);
	}

	/* 渲染线段 */
	void render_line(float left, float top, float right, float down, D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 255), float size = 1.0f)
	{
		D3DXVECTOR2 vextor[2]{ {left,top},{right,down} };
		m_ID3DXLine->SetWidth(size);
		m_ID3DXLine->Draw(vextor, 2, color);
	}

	/* 渲染函数 */
	void render()
	{
		if (m_IDirect3DDevice9)
		{
			m_IDirect3DDevice9->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
			if (SUCCEEDED(m_IDirect3DDevice9->BeginScene()))
			{
				if (m_game == GetForegroundWindow())
				{
					//这里写自己的渲染函数
					render_rect(10, 10, 200, 200);
					render_text(50, 50, "这个就是测试字体", D3DCOLOR_ARGB(255, 255, 255, 0));
					render_line(100, 100, 300, 300, D3DCOLOR_ARGB(255, 0, 255, 0), 3.0f);
				}

				m_IDirect3DDevice9->EndScene();
			}
			m_IDirect3DDevice9->Present(0, 0, 0, 0);
		}
	}
};