#include <iostream>
#include <vector>
#include <string>
#include "long_numbers.hpp"

using namespace LongNumbers;

void printLongNumber(const LongNumber& num) {
    std::cout << "Digits: " << (num.getSign() ? "-" : "");
    for (auto digit : num.getDigits()) {
        std::cout << digit;
    }
    std::cout << "\nPoint Id: " << num.getPointId() << "\nPrecision: " << num.getPrecision() << "\n\n";
}

void runTests() {
    // Test 1: default constructor
    LongNumber num1;
    if (num1.getDigits() == std::vector<short>{0} && num1.getPointId() == 0 && !num1.getSign()) {
        std::cout << "Test 1 (default constructor): OK\n";
    } else {
        std::cout << "Test 1 (default constructor): FAIL\n";
    }


    // Test 2: string constructor
    LongNumber num2("5.625", 3); // 101.101

    if (num2.getDigits() == std::vector<short>{1, 0, 1, 1, 0, 1} && num2.getPointId() == 3 && !num2.getSign()) {
        std::cout << "Test 2 (string constructor): OK\n";
    } else {
        std::cout << "Test 2 (string constructor): FAIL\n";
    }

    // Test 3: long double constructor
    LongNumber num3(-3.25, 2); // -11.01
    if (num3.getDigits() == std::vector<short>{1, 1, 0, 1} && num3.getPointId() == 2 && num3.getSign()) {
        std::cout << "Test 3 (long double constructor): OK\n";
    } else {
        std::cout << "Test 3 (long double constructor): FAIL\n";
    }

    // Test 4: copy constructor
    LongNumber num4(2.5, 1); // 10.1
    LongNumber num5 = num4;

    //printLongNumber(num5);
    //printLongNumber(num4);

    if (num5.getDigits() == std::vector<short>{1, 0, 1} && num5.getPointId() == 1 && !num5.getSign()) {
        std::cout << "Test 4 (copy constructor): OK\n";
    } else {
        std::cout << "Test 4 (copy constructor): FAIL\n";
    }

    // Test 5: copy operator
    LongNumber num6("-13.625", 2); // -1101.01
    LongNumber num7;
    num7 = num6;
    if (num7.getDigits() == std::vector<short>{1, 1, 0, 1, 0, 1} && num7.getPointId() == 2 && num7.getSign()) {
        std::cout << "Test 5 (copy operator): OK\n";
    } else {
        std::cout << "Test 5 (copy operator): FAIL\n";
    }

    // Test 6: addition
    LongNumber num8("2.5", 1); // 10.1
    printLongNumber(num8);
    LongNumber num9("1.5", 1);  // 1.1
    printLongNumber(num9);
    LongNumber sum = num8 + num9;
    printLongNumber(sum);
    if (sum == LongNumber("4.0", 1)) { // 10.0
        std::cout << "Test 6 (addition): OK\n";
    } else {
        std::cout << "Test 6 (addition): FAIL\n";
    }

    // Test 7: substraction
    LongNumber num10("2.5", 1); // 10.1
    LongNumber num11("1.5", 1);  // 1.1
    LongNumber diff = num10 - num11;
    if (diff == LongNumber("1.0", 1)) { // 1.0
        std::cout << "Test 7 (substraction): OK\n";
    } else {
        std::cout << "Test 7 (substraction): FAIL\n";
    }

    // Test 8: mulitplication
    LongNumber num12("2.5", 1); // 10.1
    LongNumber num13("1.5", 1);  // 1.1
    LongNumber mult = num12 * num13;
    if (mult == LongNumber("3.75", 2)) { // 11.11
        std::cout << "Test 8 (mulitplication): OK\n";
    } else {
        std::cout << "Test 8 (mulitplication): FAIL\n";
    }

    // Test 9: division
    LongNumber num14("2.0", 1); // 10.0
    LongNumber num15("1.0", 1);  // 1.0
    LongNumber div = num14 / num15;
    if (div == LongNumber("2.0", 1)) { // 10.0
        std::cout << "Test 9 (division): OK\n";
    } else {
        std::cout << "Test 9 (division): FAIL\n";
    }

    // Test 10: equality
    LongNumber num16("2.0", 1);
    LongNumber num17(2.0, 1);
    if (num16 == num17) {
        std::cout << "Test 10 (equality): OK\n";
    } else {
        std::cout << "Test 10 (equality): FAIL\n";
    }

    // Test 11: unequality
    LongNumber num18("2.5", 1);
    LongNumber num19("1.0", 1);
    //printLongNumber(num18);
    //printLongNumber(num19);
    if (num18 != num19) {
        std::cout << "Test 11 (unequality): OK\n";
    } else {
        std::cout << "Test 11 (unequality): FAIL\n";
    }

    // Test 12: >
    LongNumber num20("2.0", 1);
    LongNumber num21("13.5", 1);
    if (num21 > num20) {
        std::cout << "Test 12 (>): OK\n";
    } else {
        std::cout << "Test 12 (>): FAIL\n";
    }

    // Test 13: >=
    LongNumber num22("2.0", 1);
    LongNumber num23("13.5", 1);
    LongNumber num24(13.5, 1);
    if (num23 >= num22 && num23 >= num24) {
        std::cout << "Test 13 (>=): OK\n";
    } else {
        std::cout << "Test 13 (>=): FAIL\n";
    }

    // Test 14: <=
    LongNumber num25("2.0", 1);
    LongNumber num26("13.5", 1);
    LongNumber num27(13.5, 1);
    if (num25 <= num26 && num26 <= num27) {
        std::cout << "Test 14 (<=): OK\n";
    } else {
        std::cout << "Test 14 (<=): FAIL\n";
    }

    // Test 15: <
    LongNumber num28("2.0", 1);
    LongNumber num29("13.5", 1);
    if (num28 < num29) {
        std::cout << "Test 15 (<): OK\n";
    } else {
        std::cout << "Test 15 (<): FAIL\n";
    }
}

int main() {
    runTests();
    return 0;
}