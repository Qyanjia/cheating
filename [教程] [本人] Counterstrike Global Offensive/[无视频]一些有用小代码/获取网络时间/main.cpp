#include "web_time.hpp"

int main(int argc, char* argv[])
{
	web::n_time result{ 0 };
	if (web::get_current_web_time(result))
		std::printf("当前时间 :  %d-%d-%d %d:%d:%d\n",
			result.year, result.month, result.day,
			result.hour, result.minute, result.second);

	return 0;
}