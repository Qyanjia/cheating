#pragma once
#pragma warning(disable : 4267)

#include <Windows.h>
#include <WinInet.h>
#pragma comment(lib,"Wininet.lib")

#include <string>

namespace web
{
	//网页
	//http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp
	//http://quan.suning.com/getSysTime.do
	//http://www.beijing-time.org/time15.asp

	//时间结构
	struct n_time
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
	};

	//获取当前网页时间
	bool get_current_web_time(n_time& t, const std::string url = "http://quan.suning.com/getSysTime.do")
	{
		if (url.empty()) return false;

		std::string buffer;

		auto manager = InternetOpenA("time web url", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
		if (manager)
		{
			auto target = InternetOpenUrlA(manager, url.c_str(), nullptr, 0, INTERNET_FLAG_RELOAD, 0);
			if (target)
			{
				char temp[1024]{ 0 };
				unsigned long res_byte = 0;

				InternetReadFile(target, temp, 1024, &res_byte);
				buffer = temp;

				InternetCloseHandle(target);
			}

			InternetCloseHandle(manager);
		}

		//{"sysTime2":"2020-12-16 12:16:18","sysTime1":"20201216121618"}
		//----------------------------------------------------------------------------

		auto pos = buffer.find(":");
		if (pos == std::string::npos) return false;

		auto stop = buffer.find(",", pos);
		if (stop == std::string::npos) return false;

		std::string time_str = buffer.substr(pos + 2, (stop - 1 - (pos + 2)));
		sscanf_s(time_str.c_str(), "%d-%d-%d %d:%d:%d",
			&t.year,
			&t.month,
			&t.day,
			&t.hour,
			&t.minute,
			&t.second);

		return true;
	}
}