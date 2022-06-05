#include "big_integer.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>

constexpr size_t UINT32_DIGITS_CAP = 9;
constexpr std::array<uint32_t, UINT32_DIGITS_CAP + 1> DEC_POWERS = {1,      10,      100,      1000,      10000,
                                                                100000, 1000000, 10000000, 100000000, 1000000000};

constexpr size_t STRING_POW = 10;

// DEPEND ON INNER IMPLEMENTATION WITH <uint32_t> BLOCKS
constexpr size_t BASE_POW = 32;
constexpr uint64_t BASE = 1ULL << BASE_POW;
constexpr uint32_t BASE_MAX_VALUE = BASE - 1;

namespace
{
    bool is_negative(uint32_t value)
    {
        return (value >> (BASE_POW - 1)) & 1;
    }

    void sum_with_carry(uint64_t &flag, uint32_t& a, const uint32_t b)
    {
        flag = flag + a + b;
        a = flag & BASE_MAX_VALUE;
        flag >>= BASE_POW;
    }

    uint32_t string_to_int32(char const *ptr, size_t from, size_t size)
    {
        uint32_t num = 0;
        for (size_t i = 0; i < size; i++)
        {
            char ch = ptr[from + i];
            if (isdigit(ch))
            {
                num = num * STRING_POW + (ch - '0');
            }
            else
            {
                throw std::invalid_argument("not a number");
            }
        }

        return num;
    }
}

big_integer::big_integer() : digits(1) {}

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(std::vector<uint32_t> const& other, bool negative) : digits(other)
{
    negative ? negate() : normalize();
}

void big_integer::push_uint64(uint64_t value)
{
    digits.push_back(static_cast<uint32_t>(value));
    digits.push_back(static_cast<uint32_t>(value >> BASE_POW));
}

big_integer::big_integer(long a) : big_integer(static_cast<long long>(a)) {}
big_integer::big_integer(int value) : big_integer(static_cast<long long>(value)) {}
big_integer::big_integer(long long value)
{
    push_uint64(static_cast<uint64_t>(value));

    normalize();
}
big_integer::big_integer(unsigned int value) : big_integer(static_cast<unsigned long long>(value)) {}
big_integer::big_integer(unsigned long value) : big_integer(static_cast<unsigned long long>(value)) {}
big_integer::big_integer(unsigned long long value)
{
    push_uint64(value);
    digits.push_back(0);

    normalize();
}

big_integer::big_integer(std::string const& str) : big_integer()
{
    if (str.empty() || str == "-")
    {
        throw std::invalid_argument("not a number");
    }

    for (size_t i = (str[0] == '-'); i < str.size(); i += UINT32_DIGITS_CAP)
    {
        size_t size = std::min(str.size() - i, UINT32_DIGITS_CAP);
        this->short_multiply(DEC_POWERS[size]);
        *this += ::string_to_int32(str.c_str(), i, size);
    }

    if (str[0] == '-')
    {
        negate();
    }
}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) = default;

bool big_integer::is_zero() const
{
    return digits.size() == 1 && digits.back() == 0;
}

bool big_integer::is_negative() const
{
    return ::is_negative(digits.back());
}

uint32_t big_integer::get_higher_insignificant() const
{
    return is_negative() ? BASE_MAX_VALUE : 0;
}

uint32_t big_integer::get_higher_significant() const
{
    if (this->is_zero())
    {
        return 0;
    }
    else
    {
        size_t size = this->size();
        return this->digits[size - (this->digits[size - 1] == 0 ? 2 : 1)];
    }
}

uint32_t big_integer::at(size_t index) const
{
    return at(static_cast<int32_t>(index));
}

uint32_t big_integer::at(int32_t index) const
{
    if (0 <= index && index < size())
    {
        return digits[index];
    }
    else if (index < 0)
    {
        return 0;
    }
    else
    {
        return get_higher_insignificant();
    }
}

big_integer& big_integer::reserve(size_t sz)
{
    digits.resize(sz, get_higher_insignificant());
    return *this;
}

big_integer& big_integer::resize(size_t sz)
{
    if (sz > digits.size())
    {
        digits.assign(sz, 0);
    }
    return *this;
}

big_integer& big_integer::normalize()
{
    for (size_t i = size() - 1; i > 0; i--)
    {
        if ((digits[i] == 0 && !::is_negative(digits[i - 1])) ||
            (digits[i] == BASE_MAX_VALUE && ::is_negative(digits[i - 1])))
        {
            digits.pop_back();
        }
        else
        {
            break;
        }
    }

    return *this;
}

big_integer& big_integer::negate()
{
    reserve(size() + 1);

    uint64_t carry_flag = true;
    for (uint32_t & i : digits)
    {
        carry_flag += ~i;
        i = carry_flag & BASE_MAX_VALUE;
        carry_flag >>= BASE_POW;
    }

    return normalize();
}

big_integer big_integer::abs() const
{
    if (this->is_negative())
    {
        return -*this;
    }
    else
    {
        return *this;
    }
}

size_t big_integer::size() const
{
    return digits.size();
}

size_t size_of_posed(size_t size, size_t pos)
{
    return pos < size ? size - pos : 0;
}

bool big_integer::positional_is_less(big_integer const& rhs, size_t this_pos = 0, size_t rhs_pos = 0) const
{
    size_t posed_this = size_of_posed(this->size(), this_pos);
    size_t posed_rhs = size_of_posed(rhs.size(), rhs_pos);

    bool is_a_negative = this->is_negative();
    bool is_b_negative = rhs.is_negative();

    if (is_a_negative != is_b_negative)
    {
        return is_a_negative;
    }
    else if (posed_this != posed_rhs)
    {
        return (posed_this < posed_rhs) ^ is_a_negative;
    }
    else
    {
        for (size_t i = this->size(); this->size() - i < posed_this; i--)
        {
            size_t j = rhs.size() - this->size() + i;
            if (this->at(i - 1) != rhs.at(j - 1))
            {
                return this->at(i - 1) < rhs.at(j - 1);
            }
        }

        return false;
    }
}

big_integer& big_integer::positional_sub(const big_integer& rhs, size_t pos = 0)
{
    size_t posed_size = size_of_posed(size(), pos);
    reserve(pos + std::max(posed_size, rhs.size()) + 1);

    uint64_t carry_flag = 1;
    for (size_t i = pos; i < size(); i++)
    {
        ::sum_with_carry(carry_flag, digits[i], ~rhs.at(i - pos));
    }

    return normalize();
}

big_integer& big_integer::operator+=(big_integer const& rhs)
{
    reserve(std::max(this->size(), rhs.size()) + 1);

    uint64_t carry_flag = 0;
    for (size_t i = 0; i < size(); i++)
    {
        ::sum_with_carry(carry_flag, digits[i], rhs.at(i));
    }

    return normalize();
}

big_integer& big_integer::operator-=(big_integer const& rhs)
{
    return positional_sub(rhs);
}

big_integer& big_integer::short_multiply(uint32_t positive_mul)
{
    reserve(size() + 1);

    uint64_t carry_flag = 0;
    for (uint32_t& i : digits)
    {
        carry_flag += static_cast<uint64_t>(positive_mul) * i;
        i = carry_flag & BASE_MAX_VALUE;
        carry_flag = carry_flag >> BASE_POW;
    }

    return normalize();
}

big_integer& big_integer::operator*=(big_integer const& rhs)
{
    bool sign = this->is_negative() ^ rhs.is_negative();

    big_integer first(this->abs());
    big_integer second(rhs.abs());

    first.reserve(this->size() + 1);
    this->resize(this->size() * second.size() + 2);

    for (size_t i = 0; i < second.size(); i++)
    {
        uint64_t carry_flag = 0;
        for (size_t j = 0; j < first.size(); j++)
        {
            uint64_t to_add = carry_flag + digits[i + j] + static_cast<uint64_t>(second.digits[i]) * first.digits[j];
            digits[i + j] = to_add & BASE_MAX_VALUE;
            carry_flag = to_add >> BASE_POW;
        }
    }

    return sign ? negate() : normalize();
}

uint32_t big_integer::short_divide(const uint32_t rhs)
{
    bool sign = this->is_negative();

    if (sign)
    {
        negate();
    }

    uint64_t carry_flag = 0;
    for (size_t i = size(); i > 0; i--)
    {
        uint64_t current = carry_flag * (1ULL << BASE_POW) + digits[i - 1];
        digits[i - 1] = current / rhs;
        carry_flag = current % rhs;
    }

    if (sign)
    {
        negate();
    }
    else
    {
        normalize();
    }

    return sign ? (rhs - carry_flag) : carry_flag;
}

big_integer& big_integer::operator/=(big_integer const& rhs)
{
    return *this = *this / rhs;
}

big_integer& big_integer::operator%=(big_integer const& rhs)
{
    return *this -= (*this / rhs) * rhs;
}

big_integer& big_integer::bitwise_operation(big_integer const& rhs, void(f)(uint32_t&, uint32_t))
{
    reserve(std::max(this->size(), rhs.size()));

    for (size_t i = 0; i < digits.size(); i++)
    {
        f(digits[i], rhs.at(i));
    }

    return normalize();
}

big_integer& big_integer::operator&=(big_integer const& rhs)
{
    return bitwise_operation(rhs, [](uint32_t& a, uint32_t b) { a &= b; });
}

big_integer& big_integer::operator|=(big_integer const& rhs)
{
    return bitwise_operation(rhs, [](uint32_t& a, uint32_t b) { a |= b; });
}

big_integer& big_integer::operator^=(big_integer const& rhs)
{
    return bitwise_operation(rhs, [](uint32_t& a, uint32_t b) { a ^= b; });
}

big_integer& big_integer::operator<<=(int rhs)
{
    size_t block = rhs / BASE_POW;
    size_t rest = rhs % BASE_POW;

    this->reserve(this->size() + block + 1);

    if (rest == 0)
    {
        for (size_t k = this->size(); k > 0; k--)
        {
            int32_t i = static_cast<int32_t>(k) - 1;
            digits[i] = this->at(i - block);
        }
    }
    else
    {
        for (size_t k = this->size(); k > 0; k--)
        {
            int32_t i = static_cast<int32_t>(k) - 1;
            digits[i] = (this->at(i - block) << rest) | (this->at(i - block - 1) >> (BASE_POW - rest));
        }
    }

    return this->normalize();
}

big_integer& big_integer::operator>>=(int rhs)
{
    size_t block = rhs / BASE_POW;
    size_t rest = rhs % BASE_POW;

    if (rest == 0)
    {
        for (size_t i = 0; i < size(); i++)
        {
            digits[i] = this->at(i + block);
        }
    }
    else
    {
        for (size_t i = 0; i < size(); i++)
        {
            digits[i] = (this->at(i + block) >> rest) | (this->at(i + block + 1) << (BASE_POW - rest));
        }
    }

    return this->normalize();
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    return big_integer(*this).negate();
}

big_integer big_integer::operator~() const
{
    big_integer temp;

    temp.digits.pop_back();
    for (auto& i : digits)
    {
        temp.digits.push_back(~i);
    }

    return temp.normalize();
}

big_integer& big_integer::operator++()
{
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int)
{
    big_integer temp(*this);
    ++(*this);
    return temp;
}

big_integer& big_integer::operator--()
{
    *this += -1;
    return *this;
}

big_integer big_integer::operator--(int)
{
    big_integer temp(*this);
    --(*this);
    return temp;
}

big_integer operator+(big_integer a, big_integer const& b)
{
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b)
{
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b)
{
    return a *= b;
}

big_integer operator/(big_integer const& a, big_integer const& b)
{
    bool sign = a.is_negative() ^ b.is_negative();

    big_integer dividend(a.abs());
    big_integer divider(b.abs());

    if (dividend >= divider)
    {
        std::vector<uint32_t> r_quotient;
        r_quotient.reserve(dividend.size() - divider.size() + 1);

        uint32_t higher_divider = divider.get_higher_significant();
        const uint32_t f = BASE / (static_cast<uint64_t>(higher_divider) + 1);

        dividend.short_multiply(f);
        divider.short_multiply(f);

        higher_divider = divider.get_higher_significant();
        size_t divider_size = divider.size();

        for (size_t i = dividend.size() - 1; i + 2 > divider_size; i--)
        {
            uint32_t candidate = std::min((static_cast<uint64_t>(dividend.at(i)) << BASE_POW | dividend.at(i - 1)) / higher_divider,
                                          static_cast<uint64_t>(BASE_MAX_VALUE));

            big_integer to_add = divider;
            to_add.short_multiply(candidate);

            size_t pos = i >= divider_size - 1 ? i - divider_size + 1 : 0;
            while (candidate > 0 && dividend.positional_is_less(to_add, pos, 0))
            {
                candidate--;
                to_add -= divider;
            }

            r_quotient.push_back(candidate);
            dividend.positional_sub(to_add, pos);
        }

        std::reverse(r_quotient.begin(), r_quotient.end());

        if (::is_negative(r_quotient.back()))
        {
            r_quotient.push_back(0);
        }

        return big_integer(r_quotient, sign);
    }
    else
    {
        return 0;
    }
}

big_integer operator%(big_integer a, big_integer const& b)
{
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b)
{
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
    return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
    return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
    return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b)
{
    return a.digits == b.digits;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
    return a.digits != b.digits;
}

bool operator<(big_integer const& a, big_integer const& b)
{
    return a.positional_is_less(b);
}

bool operator>(big_integer const& a, big_integer const& b)
{
    return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
    return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b)
{
    return !(a < b);
}

std::string to_string(big_integer const& a)
{
    bool sign = a.is_negative();

    big_integer temp = a.abs();
    std::string result;

    do
    {
        result.append(std::to_string(temp.short_divide(STRING_POW)));
    } while (!temp.is_zero());

    if (sign)
    {
        result.append("-");
    }

    std::reverse(result.begin(), result.end());

    return result;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}
