#include <iostream>
#include <cmath>

int main() {
    float num = 123.456;
    float intPart;
    float fracPart = std::modf(num, &intPart);

    std::cout << "Integer part: " << intPart << std::endl;
    std::cout << "Fractional part: " << fracPart << std::endl;

    return 0;
}
