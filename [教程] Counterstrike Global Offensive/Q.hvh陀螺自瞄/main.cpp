#pragma warning(disable : 4244)

#include "cheats.hpp"
#include <process.h>

//作弊线程
void __cdecl hack_thread(void*)
{
	cheats* g = new cheats();
	if (g->initialize() == false) return;

	// 值设置
	g->handle_convar();

	while (true)
	{
		// 更新地图
		g->update_map();

		// 静默自瞄
		if (g->silent() == false) {}//g->big_top();

		// 辉光
		g->glow();

		// 连跳
		g->bunnyhop();

		Sleep(10);
	}
}

int main(int argc, char* argv[])
{
	HANDLE hThread = (HANDLE)_beginthread(hack_thread, 0, nullptr);
	WaitForSingleObject(hThread, INFINITE);
	system("pause");
	return 0;
}