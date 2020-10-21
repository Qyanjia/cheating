#include "cheats.hpp"

#include <string>
#include <time.h>

int main(int argc, char* argv[])
{
	std::string t = "title QQ" + std::to_string(time(NULL));
	system(t.c_str());

	apex_cheats g;
	g.start_cheats();

	return 0;
}