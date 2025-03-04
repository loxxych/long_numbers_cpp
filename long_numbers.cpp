#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include <sstream>
#include "long_numbers.hpp"

namespace LongNumbers{
    // getters
    std::vector<short> LongNumber::getDigits() const{
        return this->digits;
    }

    unsigned long int LongNumber::getPointId() const{
        return this->point_id;
    }

    bool LongNumber::getSign() const{
        return this->sign;
    }

    int LongNumber::getPrecision() const{
        return this->precision;
    }

    // setters
    void LongNumber::setDigits(std::vector<short> new_digits){
        this->digits = new_digits;
    }

    void LongNumber::setPointId(unsigned long int new_id){
        this->point_id = new_id;
    }

    void LongNumber::setSize(unsigned long int new_size){
        while(this->digits.size() < new_size){
            this->digits.insert(this->digits.begin(), 0);
        }
    }

    void LongNumber::setSign(bool new_sign){
        this->sign = new_sign;
    }

    void LongNumber::setPrecision(int new_precision){
        this->precision = new_precision;
        normalize();
    }

    // auxiliary functions

    // deletes zeros in the beginning and fixes precision
    void LongNumber::normalize() {
        while (this->digits.size() > this->point_id && this->digits.front() == 0) {
            this->digits.erase(this->digits.begin());
            this->point_id--;
        }

        if (this->digits.size() > this->point_id + this->precision) {
            this->digits.resize(this->point_id + this->precision);
        }
    }    

    // for division operator
    void LongNumber::alignPrecision(LongNumber const& other) {
        auto max_precision = std::max(this->precision, other.getPrecision());
        while (this->point_id < max_precision) {
            this->digits.push_back(0);
            // this->point_id++;
        }
        int p1 = this->precision;
        int p2 = other.getPrecision();
        int p = std::max(p1, p2);
        while (p1 < p2) {
            this->digits.insert(this->digits.begin(), 0);
        }
    }

    LongNumber LongNumber::abs() const{
        LongNumber res = *this;
        res.setSign(0);
        return res;
    }

    bool LongNumber::isZero() const{
        for (auto digit : digits){
            if (digit != 0){ return false; }
        }
        return true;
    }

    // constructors

    // no arguments constructor (aka 0)
    LongNumber::LongNumber() : precision(0), point_id(0), sign(0), digits({0}) {};

    // string constructor
    LongNumber::LongNumber(std::string num, int prec) : precision(prec), point_id(prec) {
        if (num.empty()) {
            throw std::invalid_argument("Empty string cannot be converted to LongNumber.");
        }
    
        this->sign = (num[0] == '-');
        if (this->sign) num = num.substr(1);
    
        size_t dot_pos = num.find('.');
        std::string integer_str = (dot_pos == std::string::npos) ? num : num.substr(0, dot_pos);
        std::string fraction_str = (dot_pos == std::string::npos) ? "" : num.substr(dot_pos + 1);
    
        unsigned long long integer_val = 0;
        if (!integer_str.empty()) {
            integer_val = std::stoull(integer_str);
        }
        
        do {
            digits.insert(digits.begin(), integer_val % 2);
            integer_val /= 2;
        } while (integer_val > 0);
    
        this->point_id = digits.size();
    
        double fraction_val = fraction_str.empty() ? 0.0 : std::stod("0." + fraction_str);
        std::vector<short> frac_digits;
    
        for (int i = 0; i < precision + 1; i++) {
            fraction_val *= 2;
            frac_digits.push_back(static_cast<short>(fraction_val));
            if (fraction_val >= 1.0) {
                fraction_val -= 1.0;
            }
        }
    
        if (frac_digits.size() > precision && frac_digits[precision] == 1) {
            int carry = 1;
            for (int i = precision - 1; i >= 0 && carry; i--) {
                frac_digits[i] += carry;
                carry = frac_digits[i] / 2;
                frac_digits[i] %= 2;
            }
            
            if (carry) {
                int carry_int = 1;
                for (int i = point_id - 1; i >= 0 && carry_int; i--) {
                    digits[i] += carry_int;
                    carry_int = digits[i] / 2;
                    digits[i] %= 2;
                }
                if (carry_int) {
                    digits.insert(digits.begin(), 1);
                    point_id++;
                }
            }
        }
    
        frac_digits.resize(precision);
        digits.insert(digits.end(), frac_digits.begin(), frac_digits.end());
    
        normalize();
    }
    
    

    // long double constructor
    LongNumber::LongNumber(long double num, int prec) : precision(prec) {
        this->sign = (num < 0);
        num = std::abs(num);
        
        auto integer_val = static_cast<unsigned long long>(num);
        auto fraction_val = num - static_cast<long double>(integer_val);

        do {
            digits.insert(digits.begin(), integer_val % 2);
            integer_val /= 2;
        } while (integer_val > 0);
        
        this->point_id = digits.size();

        std::vector<short> frac_digits;
        for (int i = 0; i < precision + 1; i++) {
            fraction_val *= 2;
            frac_digits.push_back(static_cast<short>(fraction_val));
            if (fraction_val >= 1.0) {
                fraction_val -= 1.0;
            }
        }

        if (frac_digits.size() > precision && frac_digits[precision] == 1) {
            int carry = 1;
            for (int i = precision - 1; i >= 0 && carry; i--) {
                frac_digits[i] += carry;
                carry = frac_digits[i] / 2;
                frac_digits[i] %= 2;
            }
    
            if (carry) {
                int carry_int = 1;
                for (int i = point_id - 1; i >= 0 && carry_int; i--) {
                    digits[i] += carry_int;
                    carry_int = digits[i] / 2;
                    digits[i] %= 2;
                }
                if (carry_int) {
                    digits.insert(digits.begin(), 1);
                    point_id++;
                }
            }
        }
    
        frac_digits.resize(precision);
        digits.insert(digits.end(), frac_digits.begin(), frac_digits.end());
    
        normalize();
    }
    
    

    // copy constructor
    LongNumber::LongNumber(const LongNumber& other){
        this->sign = other.getSign();
        this->digits = other.getDigits();
        this->point_id = other.getPointId();
        this->precision = other.getPrecision();
    };

    // copy operator
    LongNumber& LongNumber::operator = (const LongNumber& other){
        this->digits = other.getDigits();
        this->point_id = other.getPointId();
        this->sign = other.getSign();
        this->precision = other.getPrecision();
        return *this;
    }

    LongNumber& LongNumber::operator = (const LongNumber&& other){
        this->digits = other.getDigits();
        this->point_id = other.getPointId();
        this->sign = other.getSign();
        this->precision = other.getPrecision();
        return *this;
    }

    //destructor
    LongNumber::~LongNumber() = default;

    // arithmetic operators
    LongNumber LongNumber::operator+(const LongNumber& other) const {
        LongNumber a = *this, b = other;
        a.alignPrecision(b);
        b.alignPrecision(a);
    
        std::vector<short> result_digits(std::max(a.digits.size(), b.digits.size()) + 1, 0);
    
        int carry = 0;
        for (int i = result_digits.size() - 1; i >= 0; i--) {
            // int sum = (i < a.digits.size() ? a.digits[i] : 0) +
            //           (i < b.digits.size() ? b.digits[i] : 0) +
            //           carry;
            int sum = a.digits[i] + b.digits[i] + carry;
            result_digits[i] = sum % 2;
            carry = sum / 2;
        }
    
        LongNumber res;
        res.setDigits(result_digits);
        res.setPointId(a.getPointId());
        res.setSign(this->sign);
        res.normalize();
        return res;
    }
    

    LongNumber LongNumber::operator - (LongNumber const& other) const{
        // a - b = a + (-b)
        LongNumber new_other = other;
        if (other.getSign() == 1){ new_other.setSign(0); }
        else{ new_other.setSign(1); }
        return *this + new_other;
    }

    LongNumber LongNumber::operator * (LongNumber const& other) const{
        LongNumber multiplicand = this->abs();
        LongNumber multiplier = other.abs();
        std::vector<short> multiplicand_digits = multiplicand.getDigits();
        std::vector<short> multiplier_digits = multiplier.getDigits();

        LongNumber res;
        bool res_sign = this->sign ^ other.sign;
        res.setSign(res_sign);
        res.setPointId(multiplicand.getPointId() + multiplier.getPointId());

        std::vector<short> temp_res(multiplicand_digits.size() + multiplier_digits.size(), 0);
        for (auto i = 0; i < multiplicand_digits.size(); i++) {
            for (auto j = 0; j < multiplier_digits.size(); j++) {
                temp_res[i + j] += multiplicand_digits[i] * multiplier_digits[j];
            }
        }

        for (auto i = 0; i < temp_res.size() - 1; i++) {
            if (temp_res[i] >= 2) {
                temp_res[i + 1] += temp_res[i] / 2;
                temp_res[i] %= 2;
            }
        }

        res.setDigits(temp_res);
        res.normalize();

        return res;
    }

    LongNumber LongNumber::operator/(const LongNumber& other) const {
        LongNumber dividend = this->abs();
        LongNumber divisor = other.abs();
        dividend.alignPrecision(divisor);
    
        LongNumber res;
        res.setPointId(dividend.getPointId());
        res.setSign(this->sign ^ other.sign);
        std::vector<short> result_digits(dividend.digits.size(), 0);
    
        LongNumber remainder;
        for (size_t i = 0; i < dividend.digits.size(); i++) {
            remainder.digits.push_back(dividend.digits[i]);
            remainder.normalize();
    
            if (remainder >= divisor) {
                remainder = remainder - divisor;
                result_digits[i] = 1;
            }
        }
    
        res.setDigits(result_digits);
        res.normalize();
        return res;
    }
    

    // comparison operators
    bool LongNumber::operator == (const LongNumber& other) const {
        std::vector<short> this_digits = this->digits;
        std::vector<short> other_digits = other.getDigits();
    
        auto max_precision = std::max(this->precision, other.getPrecision());
        while (this_digits.size() < max_precision + this->point_id) {
            this_digits.push_back(0);
        }
        while (other_digits.size() < max_precision + other.getPointId()) {
            other_digits.push_back(0);
        }
    
        if (this_digits != other_digits) {
            return false;
        }
    
        return this->sign == other.getSign();
    }
    

    bool LongNumber::operator != (const LongNumber& other) const{
        return !(*this == other);
    }

    bool LongNumber::operator > (const LongNumber& other) const{
        if (*this == other) { return false; }

        if (this->sign == 1 && other.getSign() == 0){ return false; }
        if (this->sign == 0 && other.getSign() == 1){ return true; }
        
        // signs are the same
        // -> checking the integer parts

        // lengths of the integers differ:
        if (this->point_id > other.getPointId()){ return this->sign == 0; }
        if (this->point_id < other.getPointId()){ return this->sign != 0; }

        // lengths of the integers match:
        std::vector<short> other_digits = other.getDigits();
        for (auto i = 0; i < this->point_id; i++) {
            if (this->digits[i] > other_digits[i]) {
                return this->sign == 0;
            }
            if (this->digits[i] < other_digits[i]) {
                return !this->sign == 0;
            }
        }

        // integers match
        // -> checking fraction parts
        auto frac_size = std::max(this->digits.size() - this->point_id, other_digits.size() - other.getPointId());
        for (auto i = 0; i < frac_size; i++){
            short this_digit = (i + this->point_id < this->digits.size()) ? this->digits[i + this->point_id] : 0;
            short other_digit = (i + other.getPointId() < other_digits.size()) ? other_digits[i + other.getPointId()] : 0;

            if (this_digit > other_digit) {
                return this->sign == 0;
            }
            if (this_digit < other_digit) {
                return !this->sign == 0;
            }
        }
        return false;
    }

    bool LongNumber::operator >= (const LongNumber& other) const{
        return (*this == other || *this > other);
    }

    bool LongNumber::operator <= (const LongNumber& other) const{
        return !(*this > other);
    }

    bool LongNumber::operator < (const LongNumber& other) const{
        return !(*this >= other);
    }

    //output methods
    std::string LongNumber::toString() const {
        std::ostringstream oss;

        if (this->sign) {
            oss << "-";
        }

        // binary integer -> decimal integer
        unsigned long long integer_part = 0;
        for (size_t i = 0; i < point_id; ++i) {
            integer_part = integer_part * 2 + digits[i];
        }
        oss << integer_part;

        // binary fraction -> decimal fraction
        if (point_id < digits.size()) {
            oss << ".";
            double fractional_part = 0.0;
            for (size_t i = point_id; i < digits.size(); i++) {
                fractional_part = fractional_part * 2 + digits[i];
            }

            // converting to string
            int precision = this->precision;
            for (int i = 0; i < precision; i++) {
                fractional_part *= 10;
                int digit = static_cast<int>(fractional_part);
                oss << digit;
                fractional_part -= digit;
            }
        }

        return oss.str();
    }


    LongNumber operator ""_longnum(long double num){
        return LongNumber(num);
    };

    LongNumber operator ""_longnum(unsigned long long num){
        return LongNumber(static_cast<long double>(num));
    };
} // namespace LongNumbers