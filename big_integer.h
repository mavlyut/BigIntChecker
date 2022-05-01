#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <iostream>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <algorithm>

struct big_integer
{
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    explicit big_integer(std::string const& str);

    big_integer(uint32_t a);

    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
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

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    uint32_t operator[](size_t index) const;
    void swap(big_integer& b);
    friend std::string to_string(big_integer const& a);

private:
    bool sign;
    size_t sz_;
    void shrink();
    void expand(size_t len);
    bool compare_prefix(const big_integer &a, const big_integer &b, size_t pos, size_t m);
    uint32_t calc3to2(const big_integer &a, const big_integer &b, size_t i, size_t m);
    big_integer& div_uint(uint32_t const b);
    big_integer& mul_uint(uint32_t const rhs);
    uint32_t first();
    std::vector <uint32_t> data_;

    big_integer &difference(const big_integer &b, size_t offset, size_t m);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
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
uint32_t current_digit(const big_integer &a, const big_integer &b, size_t i);
bool compare_prefix(const big_integer &a, const big_integer &b, size_t pos);

#endif // BIG_INTEGER_H
