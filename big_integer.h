#pragma once

#include <iosfwd>
#include <ostream>
#include <string>
#include <vector>

struct big_integer
{
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    big_integer(long a);
    big_integer(long long a);
    big_integer(unsigned int a);
    big_integer(unsigned long a);
    big_integer(unsigned long long a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& second);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);
    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);
    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend big_integer operator/(big_integer const& a, big_integer const& b);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

    big_integer& negate();
    big_integer abs() const;

    bool is_zero() const;
    bool is_negative() const;

private:
    std::vector<uint32_t> digits;

    big_integer(std::vector<uint32_t> const& other, bool negative);

    uint32_t get_higher_insignificant() const;
    uint32_t get_higher_significant() const;

    big_integer& resize(size_t size);
    big_integer& reserve(size_t size);

    uint32_t at(int32_t index) const;
    uint32_t at(size_t index) const;
    size_t size() const;

    uint32_t short_divide(uint32_t rhs);
    big_integer& short_multiply(uint32_t positive_mul);

    void push_uint64(uint64_t value);

    big_integer& normalize();
    big_integer& bitwise_operation(big_integer const& rhs, void(f)(uint32_t&, uint32_t));
    big_integer& positional_sub(big_integer const& rhs, size_t pos);
    bool positional_is_less(big_integer const& rhs, size_t this_pos, size_t rhs_pos) const;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);
