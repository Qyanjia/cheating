#include "cheats.hpp"

#include <conio.h>

#include <string>
#include <time.h>
#include <process.h>

/* 随机化标题 */
void set_random_title()
{
	srand((unsigned int)time(NULL));
	const char *maps = "QAZXSWEDCVFRTGBNHYUJMKIOLPqwertyuiopasdfghjklzxcvbnm123654789";
	char title[100]{ "title " };
	for (int i = 6; i < 30; i++) title[i] = maps[rand() % (strlen(maps) - 1)];
	system(title);
}

int main(int argc, char* argv[])
{
	// 设置随机标题
	set_random_title();

	while (true)
	{
		// 暂停一下,进入游戏后按任意键开始作弊
		system("cls");
		system("pause");

		// 开始作弊
		apex_cheats g;
		g.start_cheats();
	}

	return 0;
}