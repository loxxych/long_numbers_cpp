#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "long_numbers.hpp"

using namespace LongNumbers;

LongNumber calculatePi(int precision) {
    // Leibniz formula
    LongNumber pi("0.0", precision);
    LongNumber one("1.0", precision);
    LongNumber four("4.0", precision);

    int maxIterations = precision * 10;

    for (int i = 0; i < maxIterations; ++i) {
        LongNumber term = one / LongNumber(std::to_string(2 * i + 1), precision); // (1 / (2i + 1))
        if (i % 2 == 0) {
            pi = pi + term; 
        } else {
            pi = pi - term;
        }
    }

    return four * pi;
}


int main() {
    LongNumber pi = calculatePi(3);
    std::cout << "Pi with given precision: ";
    std::cout << pi.toString() << "\n";
    return 0;
}

// int main(int argc, char* argv[]) {
//     int precision = std::atoi(argv[1]);
//     LongNumber pi = calculatePi(precision);
//     std::cout << "Pi with given precision: ";
//     std::cout << pi.toString() << "\n";

//     return 0;
// }
