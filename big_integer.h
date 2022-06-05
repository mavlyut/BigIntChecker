#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <gmp.h>
#include <iosfwd>
#include <iostream>
#include <vector>

using namespace std;

typedef unsigned int u_int;
typedef long long ll;
typedef unsigned long long u_ll;

struct big_integer {
    vector<u_int> data;
    bool sign;

    void delete_zeroes();

    big_integer(vector<u_int> const &arr);

    bool eq_short(u_int const &b) const;

    bool not_eq_short(u_int const &b) const;

    big_integer mul_long_short(u_int const &b) const;

    void mul_eq_long_short(u_int const &b);

    void add_eq_long_short(u_int const &b);

    u_int divide_eq_long_short(u_int const &b);

    void normalize();

public:
    big_integer();

    big_integer(int const &n);

    big_integer(big_integer const &other);

    explicit big_integer(string const &s);

    friend string to_string(big_integer const &a);

    big_integer &operator=(big_integer const &other);

    friend const bool operator==(big_integer const &a, big_integer const &b);

    friend const bool operator!=(big_integer const &a, big_integer const &b);

    friend const bool operator<(big_integer const &a, big_integer const &b);

    friend const bool operator<=(big_integer const &a, big_integer const &b);

    friend const bool operator>(big_integer const &a, big_integer const &b);

    friend const bool operator>=(big_integer const &a, big_integer const &b);

    friend const big_integer operator+(big_integer const &a, big_integer const &b);

    friend const big_integer operator-(big_integer const &a, big_integer const &b);

    friend const big_integer operator-(big_integer const &a);

    friend const big_integer operator+(big_integer const &a);

    friend ostream &operator<<(ostream &os, big_integer const &number);

    friend const big_integer operator*(big_integer const &a, big_integer const &b);

    friend big_integer &operator+=(big_integer &a, big_integer const &b);

    friend big_integer &operator-=(big_integer &a, big_integer const &b);

    friend big_integer &operator*=(big_integer &a, big_integer const &b);

    friend const big_integer &operator++(big_integer &a);

    friend const big_integer operator++(big_integer &a, int);

    friend const big_integer &operator--(big_integer &a);

    friend const big_integer operator--(big_integer &a, int);

    friend const big_integer operator&(big_integer const &a, big_integer const &b);

    friend const big_integer operator|(big_integer const &a, big_integer const &b);

    friend const big_integer operator^(big_integer const &a, big_integer const &b);

    friend big_integer &operator&=(big_integer &a, big_integer const &b);

    friend big_integer &operator|=(big_integer &a, big_integer const &b);

    friend big_integer &operator^=(big_integer &a, big_integer const &b);

    friend const big_integer operator~(big_integer const &a);

    friend const big_integer operator<<(big_integer const &a, u_int const &shift);

    friend const big_integer operator>>(big_integer const &a, u_int const &shift);

    friend big_integer &operator>>=(big_integer &a, u_int const &shift);

    friend big_integer &operator<<=(big_integer &a, u_int const &shift);

    friend const big_integer operator/(big_integer const &a, big_integer const &b);

    friend const big_integer operator%(big_integer const &a, big_integer const &b);

    friend big_integer &operator/=(big_integer &a, big_integer const &b);

    friend big_integer &operator%=(big_integer &a, big_integer const &b);
};

#endif // BIG_INTEGER_H
