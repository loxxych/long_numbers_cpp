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
    void LongNumber::normalize(){
        while (this->digits.size() > this->point_id && this->digits.front() == 0) {
            this->digits.erase(this->digits.begin());
        }
        while (this->point_id < this->precision){
            this->digits.push_back(0);
            this->point_id++;
        }
        while (this->point_id > this->precision){
            this->digits.pop_back();
            this->point_id--;
        }
    }

    // for division operator
    void LongNumber::alignPrecision(LongNumber const& other) {
        auto max_precision = std::max(this->precision, other.getPrecision());
        while (this->point_id < max_precision) {
            this->digits.push_back(0);
            this->point_id++;
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
        std::string integer_str = num.find('.') == std::string::npos ? num : num.substr(0, num.find('.'));
        std::string fraction_str = num.find('.') == std::string::npos ? "" : num.substr(num.find('.') + 1);

        this->sign = (num[0] == '-');
        if (this->sign){ num = integer_str.substr(1); }

        auto integer_val = std::stoull(integer_str);
        while (integer_val > 0){
            digits.insert(digits.begin(), integer_val % 2);
            integer_val /= 2;
        }

        auto fraction_val = fraction_str.empty() ? 0.0 : std::stod("0." + fraction_str);

        for (int i = 0; i < precision; i++) {
            fraction_val *= 2;
            digits.push_back(fraction_val >= 1.0);
            if (fraction_val >= 1.0) { fraction_val -= 1.0; }
        }

        this->point_id = this->digits.size() - this->precision;

        normalize();
    }

    // long double constructor
    LongNumber::LongNumber(long double num, int prec) : precision(prec) {
        this->sign = (num < 0);
        num = std::abs(num);
    
        auto integer_val = static_cast<unsigned long long>(num);
        auto fraction_val = num - static_cast<long double>(integer_val);
    
        if (integer_val == 0) {
            digits.push_back(0);
        } else {
            while (integer_val > 0) {
                digits.insert(digits.begin(), integer_val % 2);
                integer_val /= 2;
            }
        }
    
        this->point_id = digits.size();
    
        for (int i = 0; i < precision; i++) {
            fraction_val *= 2;
            if (fraction_val >= 1.0) {
                digits.push_back(1);
                fraction_val -= 1.0;
            } else {
                digits.push_back(0);
            }
        }
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
    LongNumber LongNumber::operator + (const LongNumber& other) const{
        std::vector<short> other_digits = other.getDigits();
        std::vector<short> this_digits = this->digits;
        size_t res_size = std::max(this_digits.size(), other_digits.size()) + 1;
        while (this_digits.size() < res_size) this_digits.insert(this_digits.begin(), 0);
        while (other_digits.size() < res_size) other_digits.insert(other_digits.begin(), 0);

        LongNumber res;
        res.setSize(res_size);
        std::vector<short> temp_res(res_size, 0);

        bool is_add_op = !(other.getSign() ^ this->sign); // checks if the signs are the same
        bool is_borrow = false;
        if (is_add_op){ // signs are the same, we add
            for(long int i = res_size - 1; i >= 0; i--){
                short sum = (i < this_digits.size() ? this_digits[i] : 0) +
                            (i < other_digits.size() ? other_digits[i] : 0) +
                            is_borrow;
                temp_res[i] = sum % 2;
                is_borrow = sum / 2;
            }
            res.setSign(this->sign);
        }
        else{  // signs differ, we substract
            const LongNumber& larger = (this->abs() > other.abs()) ? *this : other;
            const LongNumber& smaller = (this->abs() > other.abs()) ? other : *this;
            auto larger_digits = larger.getDigits();
            auto smaller_digits = smaller.getDigits();

            for (long int i = res_size - 1; i >= 0; --i) {
                short diff = (i < larger_digits.size() ? larger_digits[i] : 0) -
                            (i < smaller_digits.size() ? smaller_digits[i] : 0) -
                            is_borrow;
                if (diff < 0) {
                    diff += 2;
                    is_borrow = true;
                } else {
                    is_borrow = false;
                }
                temp_res[i] = diff;
            }
            res.setSign(larger.getSign());
        }

        res.setDigits(temp_res);
        res.normalize();

        return res;
    };

    LongNumber LongNumber::operator - (LongNumber const& other) const{
        // a - b = a + (-b)
        LongNumber new_other = other;
        if (other.getSign() == 1){ new_other.setSign(0); }
        else{ new_other.setSign(1); }
        return *this + new_other;
    };

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

    LongNumber LongNumber::operator / (LongNumber const& other) const{
        // if (other.isZero()) {
        //     throw std::invalid_argument("Division by zero");
        // }

        LongNumber dividend = this->abs();
        LongNumber divisor = other.abs();

        dividend.alignPrecision(divisor);
        auto dividend_digits = dividend.getDigits();

        LongNumber res;
        bool res_sign = this->sign ^ other.sign;
        res.setSign(res_sign);
        res.setPointId(dividend.getPointId());
        auto temp_res = res.getDigits();

        LongNumber temp_dividend;
        temp_dividend.setDigits(dividend.getDigits());
        temp_dividend.setPointId(dividend.getPointId());
        auto temp_dividend_digits = dividend.getDigits();

        for (size_t i = 0; i < dividend_digits.size(); i++) {
            // <<res
            temp_res[i] = 0; // заполняем по индексу

            temp_dividend_digits.push_back(dividend_digits[i]);
            temp_dividend.setDigits(temp_dividend_digits);
            temp_dividend.normalize();

            if (temp_dividend >= divisor) {
                temp_dividend = temp_dividend - divisor;
                temp_res.back() = 1;
            }
        }

        res.setDigits(temp_res);
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