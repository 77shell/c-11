#include <iostream>
#include <fstream>

int main()
{
	std::ofstream log {"logfile.txt", std::ios_base::app | std::ios_base::out};
	log << "line\n";

	return 0;
}
