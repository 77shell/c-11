#include <filesystem>
#include <iostream>

int
main(int argc, char *argv[])
{
	if (std::filesystem::exists(argv[1])) {
		std::cout << "Hello\n";
		std::filesystem::remove(argv[1]);
		std::filesystem::create_directory(argv[1]);
	}
	else
		std::cout << "NG\n";

	char buf[7];
	snprintf(buf, 7, "%06d", 1);
	printf("%s\n", buf);
	return 0;
}
