
#include <unistd.h>
#include <iostream>

int
main(int argc, char *argv[])
{
	while (1) {
		std::cout << "Hello world" << std::endl;
		sleep(1);
	}
        exit(EXIT_SUCCESS);
}
