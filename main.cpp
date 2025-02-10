#include "long_numbers.hpp"
#include <iostream>
using namespace LongNumbers;

int main() {
    LongNumber num1(10.625, 3); // 10.625 в десятичной = 1010.101 в двоичной
    std::cout << "Digits: ";
    for (auto digit : num1.getDigits()) {
        std::cout << digit;
    }
    std::cout << std::endl;
    std::cout << "Point ID: " << num1.getPointId() << std::endl;
    std::cout << "Sign: " << num1.getSign() << std::endl;
    std::cout << "Precision: " << num1.getPrecision() << std::endl;

    return 0;
}