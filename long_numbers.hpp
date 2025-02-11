#ifndef HEADER_GUARD_LONG_NUMBERS_HPP_INCLUDED
#define HEADER_GUARD_LONG_NUMBERS_HPP_INCLUDED

#include <vector>
#include <string>
#include <math.h>

namespace LongNumbers {
    class LongNumber{

    private:
        std::vector<short> digits;
        unsigned long int point_id;
        bool sign;
        int precision;

    public:
        // getters
        std::vector<short> getDigits() const;
        unsigned long int getPointId() const;
        bool getSign() const;
        int getPrecision() const;

        // setters
        void setDigits(std::vector<short> new_digits);
        void setPointId(unsigned long int new_id);
        void setSize(unsigned long int new_size);
        void setSign(bool new_sign);
        void setPrecision(int new_precision);

        //auxiliary functions
        void normalize();
        void alignPrecision(const LongNumber& other);
        LongNumber abs() const;
        bool isZero() const;

        // constructors, destructors and related operators
        LongNumber();
        LongNumber(std::string num, int prec=0);
        LongNumber(long double num, int prec=0);
        LongNumber(const LongNumber& other);
        LongNumber& operator = (LongNumber&& other);
        ~LongNumber();

        // arithmetic operators
        LongNumber operator + (const LongNumber& other) const;
        LongNumber operator - (const LongNumber& other) const;
        LongNumber operator * (const LongNumber& other) const;
        LongNumber operator / (const LongNumber& other) const;

        // comparison operators
        bool operator == (const LongNumber& other) const;
        bool operator != (const LongNumber& other) const;
        bool operator > (const LongNumber& other) const;
        bool operator >= (const LongNumber& other) const;
        bool operator <= (const LongNumber& other) const;
        bool operator < (const LongNumber& other) const;
    };

    //LongNumber operator ""_longnum(long double num);
    //LongNumber operator ""_longnum(unsigned long long num);

}

#endif 