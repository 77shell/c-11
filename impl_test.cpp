#include <iostream>
#include "impl.h"

int main(int argc, char *argv[])
{
        std::cout << "Instantiate w1\n";
        Widget w1;
        std::cout << "Move w1 to w2\n";
        auto w2 {std::move(w1)};
        std::cout << "Move w2 to w1\n";
        w1 = std::move(w2);
}
