#pragma once

#include <cstddef>
#include <gmp.h>
#include <iosfwd>

struct big_integer_gmp
{
    big_integer_gmp();
    big_integer_gmp(big_integer_gmp const& other);
    big_integer_gmp(int a);
    explicit big_integer_gmp(std::string const& str);

    template <typename RNG>
    big_integer_gmp& random(size_t sz, RNG&& rng)
    {
        gmp_randstate_t state;
        gmp_randinit_mt(state);
        gmp_randseed_ui(state, rng());
        mpz_urandomb(mpz, state, sz + 1);
        gmp_randclear(state);
        big_integer_gmp shift(1);
        shift <<= sz;
        *this -= shift;
        return *this;
    }

    ~big_integer_gmp();

    big_integer_gmp& operator=(big_integer_gmp const& other);

    big_integer_gmp& operator+=(big_integer_gmp const& rhs);
    big_integer_gmp& operator-=(big_integer_gmp const& rhs);
    big_integer_gmp& operator*=(big_integer_gmp const& rhs);
    big_integer_gmp& operator/=(big_integer_gmp const& rhs);
    big_integer_gmp& operator%=(big_integer_gmp const& rhs);

    big_integer_gmp& operator&=(big_integer_gmp const& rhs);
    big_integer_gmp& operator|=(big_integer_gmp const& rhs);
    big_integer_gmp& operator^=(big_integer_gmp const& rhs);

    big_integer_gmp& operator<<=(int rhs);
    big_integer_gmp& operator>>=(int rhs);

    big_integer_gmp operator+() const;
    big_integer_gmp operator-() const;
    big_integer_gmp operator~() const;

    big_integer_gmp& operator++();
    big_integer_gmp operator++(int);

    big_integer_gmp& operator--();
    big_integer_gmp operator--(int);

    friend bool operator==(big_integer_gmp const& a, big_integer_gmp const& b);
    friend bool operator!=(big_integer_gmp const& a, big_integer_gmp const& b);
    friend bool operator<(big_integer_gmp const& a, big_integer_gmp const& b);
    friend bool operator>(big_integer_gmp const& a, big_integer_gmp const& b);
    friend bool operator<=(big_integer_gmp const& a, big_integer_gmp const& b);
    friend bool operator>=(big_integer_gmp const& a, big_integer_gmp const& b);

    friend std::string to_string(big_integer_gmp const& a);

private:
    mpz_t mpz;
};

big_integer_gmp operator+(big_integer_gmp a, big_integer_gmp const& b);
big_integer_gmp operator-(big_integer_gmp a, big_integer_gmp const& b);
big_integer_gmp operator*(big_integer_gmp a, big_integer_gmp const& b);
big_integer_gmp operator/(big_integer_gmp a, big_integer_gmp const& b);
big_integer_gmp operator%(big_integer_gmp a, big_integer_gmp const& b);

big_integer_gmp operator&(big_integer_gmp a, big_integer_gmp const& b);
big_integer_gmp operator|(big_integer_gmp a, big_integer_gmp const& b);
big_integer_gmp operator^(big_integer_gmp a, big_integer_gmp const& b);

big_integer_gmp operator<<(big_integer_gmp a, int b);
big_integer_gmp operator>>(big_integer_gmp a, int b);

bool operator==(big_integer_gmp const& a, big_integer_gmp const& b);
bool operator!=(big_integer_gmp const& a, big_integer_gmp const& b);
bool operator<(big_integer_gmp const& a, big_integer_gmp const& b);
bool operator>(big_integer_gmp const& a, big_integer_gmp const& b);
bool operator<=(big_integer_gmp const& a, big_integer_gmp const& b);
bool operator>=(big_integer_gmp const& a, big_integer_gmp const& b);

std::string to_string(big_integer_gmp const& a);
std::ostream& operator<<(std::ostream& s, big_integer_gmp const& a);
