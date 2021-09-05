#include <iostream>

int main(int argc, char *argv[])
{
        char b;
        char bytes[1];
        char c;

        b = 'x';
        bytes[0] = 'a';
        c = 'b';

        std::cout << bytes[0] << std::endl;
}
