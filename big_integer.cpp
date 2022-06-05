#include "big_integer.h"
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;

u_int const BIT = 32;
u_ll const RADIX = static_cast<u_ll>(1) << BIT;

void big_integer::normalize() {
    delete_zeroes();
    if (this->eq_short(0)) {
        sign = false;
    }
}

void shift_array_left(vector<u_int> &arr, u_int const &shift) {
    u_int big = shift / BIT, small = shift % BIT;
    vector<u_int> res(big + arr.size(), 0);
    for (size_t i = big; i < res.size(); ++i) {
        res[i] = arr[i - big];
    }
    if (small > 0) {
        u_int prev = 0;
        for (size_t i = big; i < res.size(); ++i) {
            u_int buff = res[i];
            res[i] = (res[i] << small) | prev;
            prev = buff >> (BIT - small);
        }
        if (prev) {
            res.push_back(prev);
        }
    }
    while (res.size() > 1 && res.back() == 0) {
        res.pop_back();
    }
    arr.swap(res);
}

void shift_array_right(vector<u_int> &arr, u_int const &shift) {
    u_int big = shift / BIT, small = shift % BIT;
    if (big >= arr.size()) {
        arr.resize(1, 0);
        return;
    }
    vector<u_int> res(arr.size() - big);
    for (size_t i = 0; i < res.size(); ++i) {
        res[i] = arr[i + big];
    }
    if (small > 0) {
        for (size_t i = 0; i < res.size(); ++i) {
            res[i] >>= small;
            if (i + 1 < res.size()) {
                res[i] |= res[i + 1] << (BIT - small);
            }
        }
    }
    while (res.size() > 1 && res.back() == 0) {
        res.pop_back();
    }
    arr.swap(res);
}

void big_integer::delete_zeroes() {
    while (data.size() > 1 && data.back() == 0) {
        data.pop_back();
    }
}

vector<u_int> make_two_complement(vector<u_int> const &arr, bool const &s) {
    vector<u_int> res = arr;
    res.push_back(0);
    if (s) {
        for (u_int &re : res) {
            re = ~re;
        }
        for (u_int &re : res) {
            if (re == RADIX - 1) {
                re = 0;
            } else {
                re++;
                break;
            }
        }
    }
    return res;
}

big_integer::big_integer(vector<u_int> const &arr) {
    vector<u_int> copy = arr;
    if (copy.back() == 0) {
        copy.pop_back();
        sign = false;
    } else {
        copy.pop_back();
        size_t i = 0;
        while (i < copy.size() && copy[i] == 0) {
            copy[i++]--;
        }
        copy[i]--;
        for (u_int &i : copy) {
            i = ~i;
        }
        sign = true;
    }
    data = copy;
    delete_zeroes();
}

inline u_ll make_long_from_int(u_int const &a, u_int const &b) {
    return static_cast<u_ll>(a) * RADIX + static_cast<u_ll>(b);
}

bool big_integer::eq_short(u_int const &b) const {
    return data.size() == 1 && data[0] == b;
}

bool big_integer::not_eq_short(u_int const &b) const {
    return !(data.size() == 1 && data[0] == b);
}

big_integer big_integer::mul_long_short(u_int const &b) const {
    u_ll prev = 0;
    big_integer res;
    res.data.resize(data.size() + 1);
    res.sign = sign;
    for (size_t i = 0; i < data.size(); ++i) {
        u_ll cur = static_cast<u_ll>(data[i]) * static_cast<u_ll>(b) + prev;
        res.data[i] = static_cast<u_int>(cur % RADIX);
        prev = cur / RADIX;
    }
    if (prev) {
        res.data.back() = prev % RADIX;
    } else {
        res.data.pop_back();
    }
    res.delete_zeroes();
    return res;
}

void big_integer::mul_eq_long_short(u_int const &b) {
    *this = this->mul_long_short(b);
}

void big_integer::add_eq_long_short(u_int const &b) {
    u_ll prev = static_cast<u_ll>(b);
    for (u_int &i : data) {
        u_ll cur = i + prev;
        i = static_cast<u_int>(cur % RADIX);
        prev = cur / RADIX;
        if (prev == 0) {
            break;
        }
    }
    if (prev) {
        data.push_back(prev % RADIX);
    }
    delete_zeroes();
}

u_int big_integer::divide_eq_long_short(u_int const &b) {
    u_int prev = 0;
    for (ptrdiff_t i = data.size() - 1; i >= 0; --i) {
        u_ll cur = make_long_from_int(prev, data[i]);
        prev = static_cast<u_int>(cur % static_cast<u_ll>(b));
        data[i] = static_cast<u_int>(cur / static_cast<u_ll>(b));
    }
    delete_zeroes();
    return prev;
}


big_integer::big_integer() {
    sign = false;
    data.resize(1);
}

big_integer::big_integer(int const &n) {
    data.resize(1);
    sign = n < 0;
    data[0] = abs(static_cast<ll>(n));
}

big_integer::big_integer(big_integer const &other) {
    data = other.data;
    sign = other.sign;
}

big_integer::big_integer(string const &s) {
    data.resize(1, 0);
    if (s.empty() || s == "-0") {
        sign = false;
    } else {
        sign = s[0] == '-';
        for (size_t i = sign; i < s.length(); ++i) {
            this->mul_eq_long_short(10);
            this->add_eq_long_short(static_cast<u_int>(s[i] - '0'));
        }
        this->delete_zeroes();
    }
}

string to_string(big_integer const &a) {
    string res;
    big_integer copy = a;
    while (copy.not_eq_short(0)) {
        u_int mod = copy.divide_eq_long_short(10);
        res += static_cast<char>(mod + '0');
    }
    if (res.empty()) {
        res = "0";
    }
    if (a.sign) {
        res += '-';
    }
    reverse(res.begin(), res.end());
    return res;
}

ostream &operator<<(ostream &os, big_integer const &number) {
    os << to_string(number);
    return os;
}

big_integer &big_integer::operator=(big_integer const &other) {
    sign = other.sign;
    data = other.data;
    return *this;
}

const bool operator==(big_integer const &a, big_integer const &b) {
    return a.sign == b.sign && a.data == b.data;
}

const bool operator!=(big_integer const &a, big_integer const &b) {
    return a.sign != b.sign || a.data != b.data;
}

const bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign && !b.sign) {
        return true;
    }
    if (!a.sign && b.sign) {
        return false;
    }
    bool res;
    size_t a_size = a.data.size(), b_size = b.data.size();
    if (a_size != b_size) {
        res = a_size < b_size;
    } else {
        res = true;
        ptrdiff_t i = a_size - 1;
        for (; i >= 0; --i) {
            if (a.data[i] != b.data[i]) {
                res = a.data[i] < b.data[i];
                break;
            }
        }
        if (i == -1) {
            return false;
        }
    }
    return res ^ a.sign;
}

const bool operator<=(big_integer const &a, big_integer const &b) {
    return a < b || a == b;
}

const bool operator>(big_integer const &a, big_integer const &b) {
    return !(a <= b);
}

const bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

const big_integer operator-(big_integer const &a) {
    big_integer res = a;
    if (a.not_eq_short(0)) {
        res.sign ^= true;
    } else {
        res.sign = false;
    }
    return res;
}

const big_integer operator+(big_integer const &a) {
    const big_integer &res = a;
    return res;
}

const big_integer operator+(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        if (a.sign) {
            return b - -a;
        } else {
            return a - -b;
        }
    }
    big_integer res;
    res.sign = a.sign;
    u_int prev = 0;
    size_t a_size = a.data.size(), b_size = b.data.size();
    res.data.resize(max(a_size, b_size));
    for (size_t i = 0; i < max(a_size, b_size); ++i) {
        u_ll cur = static_cast<u_ll>(prev);
        if (i < a_size) {
            cur += static_cast<u_ll>(a.data[i]);
        }
        if (i < b_size) {
            cur += static_cast<u_ll>(b.data[i]);
        }
        res.data[i] = static_cast<u_int>(cur % RADIX);
        prev = static_cast<u_int>(cur / RADIX);
    }
    if (prev) {
        res.data.push_back(static_cast<u_int>(prev));
    }
    res.normalize();
    return res;
}

big_integer unsigned_sub_long(big_integer const &a, big_integer const &b) {
    big_integer res = a;
    res.sign = false;
    bool borrowed = false;
    for (size_t i = 0; i < res.data.size(); ++i) {
        ll first = static_cast<ll>(res.data[i]), second = 0ll;
        if (b.data.size() > i) {
            second = static_cast<ll>(b.data[i]);
        }
        first -= second + borrowed;
        if (first < 0) {
            borrowed = true;
            first += RADIX;
        } else {
            borrowed = false;
        }
        res.data[i] = static_cast<u_int>(first);
    }
    res.delete_zeroes();
    return res;
}

const big_integer operator-(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        if (a.sign) {
            return -(-a + b);
        } else {
            return a + -b;
        }
    }
    if (a.sign) {
        return a + -b;
    }
    if (a < b) {
        return -unsigned_sub_long(b, a);
    } else {
        return unsigned_sub_long(a, b);
    }
}

const big_integer operator*(big_integer const &a, big_integer const &b) {
    big_integer res;
    for (ptrdiff_t i = b.data.size() - 1; i >= 0; --i) {
        shift_array_left(res.data, BIT);
        res += a.mul_long_short(b.data[i]);
    }
    res.sign = a.sign ^ b.sign;
    res.normalize();
    return res;
}

big_integer &operator+=(big_integer &a, big_integer const &b) {
    return a = a + b;
}

big_integer &operator-=(big_integer &a, big_integer const &b) {
    return a = a - b;
}

big_integer &operator*=(big_integer &a, big_integer const &b) {
    return a = a * b;
}

const big_integer &operator++(big_integer &a) {
    if (a.sign) {
        a.sign ^= true;
        --a;
        a.sign ^= true;
    } else {
        bool was = true;
        for (u_int &i : a.data) {
            u_ll cur = static_cast<u_ll>(i) + static_cast<u_ll>(was);
            if (cur == RADIX) {
                i = 0;
            } else {
                i++;
                was = false;
                break;
            }
        }
        if (was) {
            a.data.push_back(1);
        }
    }
    a.normalize();
    return a;
}

const big_integer operator++(big_integer &a, int) {
    big_integer res = a;
    ++a;
    return res;
}

const big_integer &operator--(big_integer &a) {
    if (a.sign) {
        a.sign ^= true;
        ++a;
        a.sign ^= true;
    } else {
        if (a.eq_short(0)) {
            a.sign = true;
            a.data[0] = 1;
        } else {
            ptrdiff_t i = 0;
            while (a.data[i] == 0) {
                ++i;
            }
            a.data[i--]--;
            for (; i >= 0; --i) {
                a.data[i] = static_cast<u_int>(RADIX - 1);
            }
        }
    }
    a.normalize();
    return a;
}

const big_integer operator--(big_integer &a, int) {
    big_integer res = a;
    --a;
    return res;
}

const big_integer operator&(big_integer const &a, big_integer const &b) {
    vector<u_int> first = make_two_complement(a.data, a.sign), second = make_two_complement(b.data, b.sign);
    size_t n = max(first.size(), second.size());
    vector<u_int> res(n);
    for (size_t i = 0; i < n; ++i) {
        u_int c = i < first.size() ? first[i] : 0;
        u_int d = i < second.size() ? second[i] : 0;
        res[i] = c & d;
    }
    return big_integer(res);
}

const big_integer operator|(big_integer const &a, big_integer const &b) {
    vector<u_int> first = make_two_complement(a.data, a.sign), second = make_two_complement(b.data, b.sign);
    size_t n = max(first.size(), second.size());
    vector<u_int> res(n);
    for (size_t i = 0; i < n; ++i) {
        u_int c = i < first.size() ? first[i] : 0;
        u_int d = i < second.size() ? second[i] : 0;
        res[i] = c | d;
    }
    return big_integer(res);
}

const big_integer operator^(big_integer const &a, big_integer const &b) {
    vector<u_int> first = make_two_complement(a.data, a.sign), second = make_two_complement(b.data, b.sign);
    size_t n = max(first.size(), second.size());
    vector<u_int> res(n);
    for (size_t i = 0; i < n; ++i) {
        u_int c = i < first.size() ? first[i] : 0;
        u_int d = i < second.size() ? second[i] : 0;
        res[i] = c ^ d;
    }
    return big_integer(res);
}

big_integer &operator&=(big_integer &a, big_integer const &b) {
    return a = a & b;
}

big_integer &operator|=(big_integer &a, big_integer const &b) {
    return a = a | b;
}

big_integer &operator^=(big_integer &a, big_integer const &b) {
    return a = a ^ b;
}

const big_integer operator~(big_integer const &a) {
    vector<u_int> res = make_two_complement(a.data, a.sign);
    for (u_int &re : res) {
        re = ~re;
    }
    return big_integer(res);
}

const big_integer operator<<(big_integer const &a, u_int const &shift) {
    vector<u_int> new_data;
    if (a.sign) {
        new_data = make_two_complement(a.data, a.sign);
    } else {
        new_data = a.data;
    }
    shift_array_left(new_data, shift);
    if (!a.sign) {
        new_data.push_back(0);
    }
    return big_integer(new_data);

}

const big_integer operator>>(big_integer const &a, u_int const &shift) {
    vector<u_int> new_data;
    if (a.sign) {
        new_data = make_two_complement(a.data, a.sign);
    } else {
        new_data = a.data;
    }
    shift_array_right(new_data, shift);
    if (!a.sign) {
        new_data.push_back(0);
    }
    return big_integer(new_data);
}

big_integer &operator>>=(big_integer &a, u_int const &shift) {
    return a = a >> shift;
}

big_integer &operator<<=(big_integer &a, u_int const &shift) {
    return a = a << shift;
}

const big_integer operator/(big_integer const &a, big_integer const &b) {
    big_integer a_copy = a, b_copy = b;
    a_copy.sign = b_copy.sign = false;
    if (a_copy < b_copy) {
        return 0;
    }
    if (b.data.size() == 1) {
        big_integer res = a;
        res.divide_eq_long_short(b.data[0]);
        res.sign ^= b.sign;
        if (res.eq_short(0)) {
            res.sign = false;
        }
        return res;
    }
    if (b_copy.data.back() < RADIX / 2) {
        u_int ssh = 0, bit = b_copy.data.back();
        while (bit < static_cast<u_int>(1 << 31)) {
            ++ssh;
            bit <<= 1;
        }
        shift_array_left(a_copy.data, ssh);
        shift_array_left(b_copy.data, ssh);
    }
    big_integer res;
    size_t n = b_copy.data.size(), m = a_copy.data.size() - n;
    res.data.resize(m + 1);
    big_integer shift = b_copy;
    shift_array_left(shift.data, BIT * m);
    if (a_copy >= shift) {
        res.data.back() = 1;
        a_copy -= shift;
    }
    for (ptrdiff_t j = m - 1; j >= 0; --j) {
        u_ll qj;
        if (a_copy.data.size() > 1) {
            qj = (static_cast<u_ll>(RADIX * a_copy.data.back()) + a_copy.data[a_copy.data.size() - 2]);
        } else {
            qj = static_cast<u_ll>(a_copy.data.back());
        }
        qj /= b_copy.data.back();
        if (qj > RADIX - 1) {
            qj = RADIX - 1;
        }
        shift_array_right(shift.data, BIT);
        a_copy -= shift.mul_long_short(qj);
        while (a_copy.sign) {
            --qj;
            a_copy += shift;
        }
        res.data[j] = qj;
    }
    res.sign = a.sign ^ b.sign;
    res.normalize();
    return res;
}

const big_integer operator%(big_integer const &a, big_integer const &b) {
    return a - b * (a / b);
}

big_integer &operator/=(big_integer &a, big_integer const &b) {
    return a = a / b;
}

big_integer &operator%=(big_integer &a, big_integer const &b) {
    return a = a % b;
}
