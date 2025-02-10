#include <vector>
#include <string>
#include <math.h>
#include <long_numbers.hpp>

namespace LongNumbers{
class LongNumber{

private:
    std::vector<short> digits; // digits in binary
    unsigned long int point_id; // index of the point in digits
    bool sign; // - = 1, + = 0
    int precision;

public:
    // getters
    std::vector<short> getDigits() const{
        return this->digits;
    }

    unsigned long int getPointId() const{
        return this->point_id;
    }

    bool getSign() const{
        return this->sign;
    }

    int getPrecision() const{
        return this->precision;
    }

    // setters
    void setDigits(std::vector<short> new_digits){
        this->digits = new_digits;
    }

    void setPointId(unsigned long int new_id){
        this->point_id = new_id;
    }

    void setSize(unsigned long int new_size){
        while(this->digits.size() < new_size){
            this->digits.insert(this->digits.begin(), 0);
        }
    }

    void setSign(bool new_sign){
        this->sign = new_sign;
    }

    void setPrecision(int new_precision){
        this->precision = new_precision;
        normalize();
    }

    // auxiliary functions

    // deletes zeros in the beginning and fixes precision
    void normalize(){
        while (this->digits.size() > this->point_id && this->digits.front() == 0) {
            this->digits.erase(this->digits.begin());
        }
        while ((this->digits.size() - this->point_id) < this->precision){
            this->digits.push_back(0);
        }
        while ((this->digits.size() - this->point_id) > this->precision){
            this->digits.pop_back();
        }
    }

    // for division operator
    void alignPrecision(LongNumber const& other) {
        auto max_precision = std::max(this->precision, other.getPrecision());
        while ((this->digits.size() - this->point_id) < max_precision) {
            this->digits.push_back(0);
        }
    }

    LongNumber abs() const{
        LongNumber res = *this;
        res.setSign(0);
        return res;
    }

    bool isZero() const{
        for (auto digit : digits){
            if (digit != 0){ return false; }
        }
        return true;
    }

    // constructors

    // no arguments constructor (aka 0)
    LongNumber() : precision(0), point_id(0), sign(0), digits({0}) {};

    // string constructor
    LongNumber(std::string num, int prec=0) : precision(prec), point_id(prec) {
        std::string integer_str = num.find('.') == std::string::npos ? num : num.substr(0, num.find('.'));
        std::string fraction_str = num.find('.') == std::string::npos ? "" : num.substr(num.find('0') + 1);

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
    LongNumber(long double num, int prec=0) : precision(prec), point_id(prec){
        this->sign = num < 0 ? 1 : 0;
        num = std::abs(num);

        auto integer_val = static_cast<unsigned long long>(num);
        auto fraction_val = num - integer_val;

        while (integer_val > 0){
            digits.insert(digits.begin(), integer_val % 2);
            integer_val /= 2;
        }

        for (int i = 0; i < precision; i++) {
            fraction_val *= 2;
            digits.push_back(fraction_val >= 1.0);
            if (fraction_val >= 1.0) {
               fraction_val -= 1.0;
            }
        }

        this->point_id = this->digits.size() - this->precision;

        normalize();
    };

    // copy constructor
    LongNumber(const LongNumber& other){
        this->sign = other.getSign();
        this->digits = other.getDigits();
        this->point_id = other.getPointId();
        this->precision = other.getPrecision();
    };

    // copy operator
    LongNumber& operator = (const LongNumber& other){
        this->digits = other.getDigits();
        this->point_id = other.getPointId();
        this->sign = other.getSign();
        this->precision = other.getPrecision();
        return *this;
    }

    //destructor
    ~LongNumber() = default;

    // arithmetic operators
    LongNumber operator + (const LongNumber& other) const{
        std::vector<short> other_digits = other.getDigits();

        LongNumber res;
        auto res_size = std::max(this->digits.size(), other_digits.size()) + 1;
        res.setSize(res_size);
        std::vector<short> temp_res(res_size, 0);

        bool is_add_op = !(other.getSign() ^ this->sign); // checks if the signs are the same
        bool is_borrow = false;
        if (is_add_op){ // signs are the same, we add
            for(unsigned long int i = res_size - 1; i >= 0; i--){
                short sum = (i < this->digits.size() ? this->digits[i] : 0) +
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

    LongNumber operator - (LongNumber const& other) const{
        // a - b = a + (-b)
        LongNumber new_other = other;
        if (other.getSign() == 1){ new_other.setSign(0); }
        else{ new_other.setSign(1); }
        return *this + new_other;
    };

    LongNumber operator * (LongNumber const& other) const{
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

    LongNumber operator / (LongNumber const& other) const{
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
            temp_res.push_back(0);

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
    };

    // comparison operators
    bool operator == (const LongNumber& other) const{
        bool abs_equals = true;
        std::vector<short> other_digits = other.getDigits();
        auto max_size = std::max(this->digits.size(), other_digits.size());

        // checking integers
        for (auto i = 0; i < max_size; i++) {
            short this_digit = (i < this->digits.size()) ? this->digits[i] : 0;
            short other_digit = (i < other_digits.size()) ? other_digits[i] : 0;
            if (this_digit != other_digit) {
                abs_equals = false;
                break;
            }
        }
        return (this->sign == other.getSign()) && abs_equals;
    };

    bool operator != (const LongNumber& other) const{
        return !(*this == other);
    };

    bool operator > (const LongNumber& other) const{
        if (*this == other) { return false; }

        if (this->sign == 1 && other.getSign() == 0){ return false; }
        if (this->sign == 0 && other.getSign() == 1){ return true; }
        
        // signs differ ->
        // checking the integer parts
        // lengths of the integers differ
        if (this->point_id > other.getPointId()){ return this->sign == 0; }
        if (this->point_id < other.getPointId()){ return this->sign != 0; }

        // lengths of the integers match
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
    };

    bool operator >= (const LongNumber& other) const{
        return (*this == other || *this > other);
    };

    bool operator <= (const LongNumber& other) const{
        return !(*this > other);
    };

    bool operator < (const LongNumber& other) const{
        return !(*this >= other);
    };
    }; // end of class LongNumber

    /*LongNumber operator ""_longnum(long double num){
        return LongNumber(num);
    };

    LongNumber operator ""_longnum(unsigned long long num){
        return LongNumber(static_cast<long double>(num));
    };*/
} // namespace LongNumbers