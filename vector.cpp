#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
	std::vector<int> v(5, 1);
	for(auto i : v)
		std::cout << "Element: " << i << std::endl;

        std::vector<char> txq;
        unsigned char d {0x31};
        for(ssize_t i=0; i<26; i++, d++)
                txq.push_back(d);
        fprintf(stdout, "%s\n", txq.data());
        fprintf(stdout, "txq size: %d\n", txq.size());
}
