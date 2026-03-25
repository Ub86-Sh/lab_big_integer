// =============================================================
//  big_integer.cpp -- BigInteger class implementation
//
//  TASK: Implement all methods declared in big_integer.h
//  This stub file exists only so the project structure is clear.
//  Replace its contents with your implementation.
// =============================================================

#include "big_integer.h"





#include "big_integer.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

// ========== Forward declarations ==========
static void remove_leading_zeros(std::vector<int>& digits);
static std::vector<int> add_abs(const std::vector<int>& a, const std::vector<int>& b);
static std::vector<int> subtract_abs(const std::vector<int>& a, const std::vector<int>& b);
static int compare_abs(const std::vector<int>& a, const std::vector<int>& b);

// ========== Helper implementations ==========

static void remove_leading_zeros(std::vector<int>& digits) {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
}

static std::vector<int> add_abs(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> result;
    size_t n = std::max(a.size(), b.size());
    int carry = 0;
    for (size_t i = 0; i < n || carry; ++i) {
        int sum = carry;
        if (i < a.size()) sum += a[i];
        if (i < b.size()) sum += b[i];
        result.push_back(sum % 10);
        carry = sum / 10;
    }
    return result;
}

static std::vector<int> subtract_abs(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> result = a;
    int borrow = 0;
    for (size_t i = 0; i < b.size() || borrow; ++i) {
        int sub = borrow;
        if (i < b.size()) sub += b[i];
        if (result[i] < sub) {
            result[i] += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[i] -= sub;
    }
    remove_leading_zeros(result);
    return result;
}

static int compare_abs(const std::vector<int>& a, const std::vector<int>& b) {
    if (a.size() != b.size()) {
        return a.size() < b.size() ? -1 : 1;
    }
    for (size_t i = a.size(); i > 0; --i) {
        if (a[i - 1] != b[i - 1]) {
            return a[i - 1] < b[i - 1] ? -1 : 1;
        }
    }
    return 0;
}

// ========== BigInteger methods ==========

BigInteger::BigInteger() : digits_{0}, negative_(false) {}

BigInteger::BigInteger(int value) : BigInteger(static_cast<long long>(value)) {}

BigInteger::BigInteger(long long value) {
    if (value == 0) {
        digits_.push_back(0);
        negative_ = false;
        return;
    }
    if (value < 0) {
        negative_ = true;
        value = -value;
    } else {
        negative_ = false;
    }
    while (value > 0) {
        digits_.push_back(value % 10);
        value /= 10;
    }
}

BigInteger::BigInteger(const std::string& str) {
    if (str.empty()) {
        throw std::invalid_argument("Empty string");
    }
    size_t start = 0;
    if (str[0] == '-') {
        negative_ = true;
        start = 1;
    } else {
        negative_ = false;
        if (str[0] == '+') start = 1;
    }
    while (start < str.size() && str[start] == '0') {
        ++start;
    }
    if (start == str.size()) {
        digits_.push_back(0);
        negative_ = false;
        return;
    }
    for (size_t i = str.size(); i > start; --i) {
        char c = str[i - 1];
        if (!std::isdigit(c)) {
            throw std::invalid_argument("Invalid character");
        }
        digits_.push_back(c - '0');
    }
    remove_leading_zeros(digits_);
    if (digits_.size() == 1 && digits_[0] == 0) {
        negative_ = false;
    }
}

// ==================== Arithmetic ======================

BigInteger BigInteger::operator+(const BigInteger& rhs) const {
    if (negative_ == rhs.negative_) {
        BigInteger result;
        result.negative_ = negative_;
        result.digits_ = add_abs(digits_, rhs.digits_);
        remove_leading_zeros(result.digits_);
        if (result.digits_.size() == 1 && result.digits_[0] == 0) {
            result.negative_ = false;
        }
        return result;
    }
    if (compare_abs(digits_, rhs.digits_) >= 0) {
        BigInteger result;
        result.negative_ = negative_;
        result.digits_ = subtract_abs(digits_, rhs.digits_);
        if (result.digits_.size() == 1 && result.digits_[0] == 0) {
            result.negative_ = false;
        }
        return result;
    } else {
        BigInteger result;
        result.negative_ = rhs.negative_;
        result.digits_ = subtract_abs(rhs.digits_, digits_);
        if (result.digits_.size() == 1 && result.digits_[0] == 0) {
            result.negative_ = false;
        }
        return result;
    }
}

BigInteger BigInteger::operator-(const BigInteger& rhs) const {
    if (negative_ != rhs.negative_) {
        BigInteger result;
        result.negative_ = negative_;
        result.digits_ = add_abs(digits_, rhs.digits_);
        remove_leading_zeros(result.digits_);
        if (result.digits_.size() == 1 && result.digits_[0] == 0) {
            result.negative_ = false;
        }
        return result;
    }
    if (compare_abs(digits_, rhs.digits_) >= 0) {
        BigInteger result;
        result.negative_ = negative_;
        result.digits_ = subtract_abs(digits_, rhs.digits_);
        if (result.digits_.size() == 1 && result.digits_[0] == 0) {
            result.negative_ = false;
        }
        return result;
    } else {
        BigInteger result;
        result.negative_ = !negative_;
        result.digits_ = subtract_abs(rhs.digits_, digits_);
        if (result.digits_.size() == 1 && result.digits_[0] == 0) {
            result.negative_ = false;
        }
        return result;
    }
}

BigInteger BigInteger::operator*(const BigInteger& rhs) const {
    BigInteger result;
    result.digits_.assign(digits_.size() + rhs.digits_.size(), 0);
    result.negative_ = negative_ != rhs.negative_;
    
    for (size_t i = 0; i < digits_.size(); ++i) {
        int carry = 0;
        for (size_t j = 0; j < rhs.digits_.size() || carry; ++j) {
            long long cur = result.digits_[i + j] +
                           static_cast<long long>(digits_[i]) *
                           (j < rhs.digits_.size() ? rhs.digits_[j] : 0) +
                           carry;
            result.digits_[i + j] = static_cast<int>(cur % 10);
            carry = static_cast<int>(cur / 10);
        }
    }
    remove_leading_zeros(result.digits_);
    if (result.digits_.size() == 1 && result.digits_[0] == 0) {
        result.negative_ = false;
    }
    return result;
}

BigInteger BigInteger::operator/(const BigInteger& rhs) const {
    if (rhs.is_zero()) {
        throw std::runtime_error("Division by zero");
    }
    if (is_zero()) {
        return BigInteger();
    }
    
    // Проверка на деление на 1 и -1
    if (rhs.digits_.size() == 1 && rhs.digits_[0] == 1) {
        BigInteger result = *this;
        if (rhs.negative_) {
            result.negative_ = !result.negative_;
        }
        return result;
    }
    
    // Получаем абсолютные значения
    BigInteger dividend = *this;
    dividend.negative_ = false;
    BigInteger divisor = rhs;
    divisor.negative_ = false;
    
    if (dividend < divisor) {
        return BigInteger();
    }
    
    BigInteger result;
    result.digits_.clear();
    result.negative_ = negative_ != rhs.negative_;
    
    BigInteger current;
    for (size_t i = dividend.digits_.size(); i > 0; --i) {
        // Добавляем следующую цифру
        current.digits_.insert(current.digits_.begin(), dividend.digits_[i - 1]);
        // Убираем ведущие нули
        while (current.digits_.size() > 1 && current.digits_.back() == 0) {
            current.digits_.pop_back();
        }
        
        // Бинарный поиск для нахождения цифры частного
        int x = 0;
        int l = 0, r = 9;
        while (l <= r) {
            int m = (l + r) / 2;
            BigInteger temp = divisor * m;
            if (temp <= current) {
                x = m;
                l = m + 1;
            } else {
                r = m - 1;
            }
        }
        
        // Сохраняем цифру результата
        result.digits_.insert(result.digits_.begin(), x);
        
        // Вычитаем divisor * x из current
        current = current - divisor * x;
    }
    
    // Убираем ведущие нули
    while (result.digits_.size() > 1 && result.digits_.back() == 0) {
        result.digits_.pop_back();
    }
    if (result.digits_.size() == 1 && result.digits_[0] == 0) {
        result.negative_ = false;
    }
    
    return result;
}

BigInteger BigInteger::operator%(const BigInteger& rhs) const {
    BigInteger quotient = *this / rhs;
    return *this - quotient * rhs;
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs) {
    *this = *this + rhs;
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& rhs) {
    *this = *this - rhs;
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
    *this = *this * rhs;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& rhs) {
    *this = *this / rhs;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& rhs) {
    *this = *this % rhs;
    return *this;
}

// ====================== Unary =========================

BigInteger BigInteger::operator-() const {
    BigInteger result = *this;
    if (!is_zero()) {
        result.negative_ = !negative_;
    }
    return result;
}

BigInteger& BigInteger::operator++() {
    *this += 1;
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger temp = *this;
    ++(*this);
    return temp;
}

BigInteger& BigInteger::operator--() {
    *this -= 1;
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger temp = *this;
    --(*this);
    return temp;
}

// ======================= Comparison ========================

bool BigInteger::operator==(const BigInteger& rhs) const {
    return negative_ == rhs.negative_ && digits_ == rhs.digits_;
}

bool BigInteger::operator!=(const BigInteger& rhs) const {
    return !(*this == rhs);
}

bool BigInteger::operator<(const BigInteger& rhs) const {
    if (negative_ != rhs.negative_) {
        return negative_;
    }
    if (negative_) {
        return compare_abs(digits_, rhs.digits_) > 0;
    }
    return compare_abs(digits_, rhs.digits_) < 0;
}

bool BigInteger::operator>(const BigInteger& rhs) const {
    return rhs < *this;
}

bool BigInteger::operator<=(const BigInteger& rhs) const {
    return !(*this > rhs);
}

bool BigInteger::operator>=(const BigInteger& rhs) const {
    return !(*this < rhs);
}

// ======================== Misc ==============================

std::string BigInteger::to_string() const {
    if (is_zero()) return "0";
    std::string result;
    if (negative_) result += '-';
    for (auto it = digits_.rbegin(); it != digits_.rend(); ++it) {
        result += static_cast<char>('0' + *it);
    }
    return result;
}

bool BigInteger::is_zero() const {
    return digits_.size() == 1 && digits_[0] == 0;
}

bool BigInteger::is_negative() const {
    return negative_;
}

BigInteger::operator bool() const {
    return !is_zero();
}

// ======================== I/O ==============================

std::ostream& operator<<(std::ostream& os, const BigInteger& value) {
    os << value.to_string();
    return os;
}

std::istream& operator>>(std::istream& is, BigInteger& value) {
    std::string str;
    is >> str;
    value = BigInteger(str);
    return is;
}