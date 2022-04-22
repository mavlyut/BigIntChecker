#include "big_integer_gmp.h"

#include <cstring>
#include <ostream>
#include <stdexcept>
#include <string>

big_integer_gmp::big_integer_gmp()
{
    mpz_init(mpz);
}

big_integer_gmp::big_integer_gmp(big_integer_gmp const& other)
{
    mpz_init_set(mpz, other.mpz);
}

big_integer_gmp::big_integer_gmp(int a)
{
    mpz_init_set_si(mpz, a);
}

big_integer_gmp::big_integer_gmp(std::string const& str)
{
    if (mpz_init_set_str(mpz, str.c_str(), 10))
    {
        mpz_clear(mpz);
        throw std::runtime_error("invalid string");
    }
}

big_integer_gmp::~big_integer_gmp()
{
    mpz_clear(mpz);
}

big_integer_gmp& big_integer_gmp::operator=(big_integer_gmp const& other)
{
    mpz_set(mpz, other.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator+=(big_integer_gmp const& rhs)
{
    mpz_add(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator-=(big_integer_gmp const& rhs)
{
    mpz_sub(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator*=(big_integer_gmp const& rhs)
{
    mpz_mul(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator/=(big_integer_gmp const& rhs)
{
    mpz_tdiv_q(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator%=(big_integer_gmp const& rhs)
{
    mpz_tdiv_r(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator&=(big_integer_gmp const& rhs)
{
    mpz_and(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator|=(big_integer_gmp const& rhs)
{
    mpz_ior(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator^=(big_integer_gmp const& rhs)
{
    mpz_xor(mpz, mpz, rhs.mpz);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator<<=(int rhs)
{
    mpz_mul_2exp(mpz, mpz, rhs);
    return *this;
}

big_integer_gmp& big_integer_gmp::operator>>=(int rhs)
{
    mpz_div_2exp(mpz, mpz, rhs);
    return *this;
}

big_integer_gmp big_integer_gmp::operator+() const
{
    return *this;
}

big_integer_gmp big_integer_gmp::operator-() const
{
    big_integer_gmp r;
    mpz_neg(r.mpz, mpz);
    return r;
}

big_integer_gmp big_integer_gmp::operator~() const
{
    big_integer_gmp r;
    mpz_com(r.mpz, mpz);
    return r;
}

big_integer_gmp& big_integer_gmp::operator++()
{
    mpz_add_ui(mpz, mpz, 1);
    return *this;
}

big_integer_gmp big_integer_gmp::operator++(int)
{
    big_integer_gmp r = *this;
    ++* this;
    return r;
}

big_integer_gmp& big_integer_gmp::operator--()
{
    mpz_sub_ui(mpz, mpz, 1);
    return *this;
}

big_integer_gmp big_integer_gmp::operator--(int)
{
    big_integer_gmp r = *this;
    --* this;
    return r;
}

big_integer_gmp operator+(big_integer_gmp a, big_integer_gmp const& b)
{
    return a += b;
}

big_integer_gmp operator-(big_integer_gmp a, big_integer_gmp const& b)
{
    return a -= b;
}

big_integer_gmp operator*(big_integer_gmp a, big_integer_gmp const& b)
{
    return a *= b;
}

big_integer_gmp operator/(big_integer_gmp a, big_integer_gmp const& b)
{
    return a /= b;
}

big_integer_gmp operator%(big_integer_gmp a, big_integer_gmp const& b)
{
    return a %= b;
}

big_integer_gmp operator&(big_integer_gmp a, big_integer_gmp const& b)
{
    return a &= b;
}

big_integer_gmp operator|(big_integer_gmp a, big_integer_gmp const& b)
{
    return a |= b;
}

big_integer_gmp operator^(big_integer_gmp a, big_integer_gmp const& b)
{
    return a ^= b;
}

big_integer_gmp operator<<(big_integer_gmp a, int b)
{
    return a <<= b;
}

big_integer_gmp operator>>(big_integer_gmp a, int b)
{
    return a >>= b;
}

bool operator==(big_integer_gmp const& a, big_integer_gmp const& b)
{
    return mpz_cmp(a.mpz, b.mpz) == 0;
}

bool operator!=(big_integer_gmp const& a, big_integer_gmp const& b)
{
    return mpz_cmp(a.mpz, b.mpz) != 0;
}

bool operator<(big_integer_gmp const& a, big_integer_gmp const& b)
{
    return mpz_cmp(a.mpz, b.mpz) < 0;
}

bool operator>(big_integer_gmp const& a, big_integer_gmp const& b)
{
    return mpz_cmp(a.mpz, b.mpz) > 0;
}

bool operator<=(big_integer_gmp const& a, big_integer_gmp const& b)
{
    return mpz_cmp(a.mpz, b.mpz) <= 0;
}

bool operator>=(big_integer_gmp const& a, big_integer_gmp const& b)
{
    return mpz_cmp(a.mpz, b.mpz) >= 0;
}

std::string to_string(big_integer_gmp const& a)
{
    char* tmp = mpz_get_str(nullptr, 10, a.mpz);
    std::string res = tmp;

    void (*freefunc)(void*, size_t);
    mp_get_memory_functions(nullptr, nullptr, &freefunc);

    freefunc(tmp, strlen(tmp) + 1);

    return res;
}

std::ostream& operator<<(std::ostream& s, big_integer_gmp const& a)
{
    return s << to_string(a);
}
