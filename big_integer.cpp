#include "big_integer.h"

big_integer::big_integer() : data_(1, 0u), sign(false) {}

big_integer::big_integer(big_integer const& other) = default;


big_integer::big_integer(int a)
{
    sign = false;
    if (a < 0) {
        data_.push_back(static_cast<uint32_t>((a == INT32_MIN ? 1 : -1)) * a);
        data_[0] *= (a == INT32_MIN ? -1 : 1);
        *this = -(*this);
    } else {
        data_ = std::vector<uint32_t>(1, static_cast<uint32_t>(a));
    }
}

big_integer::big_integer(uint32_t a) {
    sign = false;
    data_.push_back(a);
}

big_integer::big_integer(std::string const& str) : big_integer()
{
    for (size_t i = (str[0] == '-' ? 1 : 0); i < str.length(); i++) {
        *this *= 10;
        *this += (str[i] - '0');
    }
    bool f = true;
    for (size_t i = 0; i < data_.size(); i++) {
        if (data_[i] != UINT32_MAX) {
            f = false;
            break;
        }
    }
    if (str[0] == '-' && f) {
        *this = ~(*this);
        (*this)--;
    } else if (str[0] == '-' && !f) {
        *this = -(*this);
    }
}

big_integer::~big_integer() = default;

void big_integer::swap(big_integer &b) {
    std::swap(data_, b.data_);
    std::swap(sign, b.sign);
}

big_integer& big_integer::operator=(big_integer const& other) = default;

big_integer& big_integer::operator+=(big_integer const& rhs)
{
    size_t l = std::max(rhs.data_.size(), data_.size());
    expand(l + 1);
    uint64_t carry = 0;
    for (size_t i = 0; i < data_.size(); i++) {
        uint64_t res = data_[i] + carry;
        res += rhs[i];
        carry = res >> 32u;
        uint32_t dig = static_cast<uint32_t>(res);
        data_[i] = dig;
    }
    sign = (data_.back() >> (31u));
    shrink();
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs)
{
    return (*this += (-rhs));
}

big_integer& big_integer::mul_uint(uint32_t const rhs) {
    uint32_t carry = 0u;
    for (size_t i = 0; i < data_.size(); i++) {
        uint64_t cur = data_[i];
        cur *= rhs;
        cur += carry;
        carry = cur >> 32u;
        data_[i] = static_cast<uint32_t>(cur);
    }
    if (carry != 0) {
        data_.push_back(carry);
    }
    return *this;
}

big_integer& big_integer::div_uint(uint32_t const b) {
    uint64_t carry = 0u;
    for (int32_t i = data_.size() - 1; i >= 0; i--) {
        uint64_t was = data_[i];
        uint64_t cur_digit = was + (carry << 32u);
        data_[i] = cur_digit / b;
        carry = cur_digit % b;
    }
    shrink();
    return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs)
{
    if (rhs == -1) {
        *this = -(*this);
        return *this;
    }
    big_integer &a = *this;
    big_integer b = rhs;
    bool res_sign = false;
    if (a.sign) {
        res_sign ^= a.sign;
        a = -a;
    }
    if (b.sign) {
        res_sign ^= b.sign;
        b = -b;
    }
    big_integer c;
    c.expand(b.data_.size() + a.data_.size());
    for (size_t i = 0; i < a.data_.size(); i++) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b.data_.size() || carry; j++) {
            uint64_t cur = a.data_[i];
            cur *= static_cast<uint64_t>(j < b.data_.size() ? b.data_[j] : 0u);
            cur += c.data_[i + j];
            cur += carry;
            c.data_[i + j] = static_cast<uint32_t>(cur);
            carry = cur >> 32;
        }
    }
    if (res_sign) {
        c = -c;
    }
    c.shrink();
    *this = c;
    return *this;
}

uint32_t big_integer::calc3to2(const big_integer &a, const big_integer &b, size_t i, size_t m) {
    __uint128_t ai = a[i + m];
    __uint128_t ai1 = a[i + m - 1];
    __uint128_t ai2 = a[i + m - 2];
    __uint128_t bj = b[m - 1];
    __uint128_t bj1 = b[m - 2];
    __uint128_t a3 = (ai << 64u) + (ai1 << 32u) + ai2;
    __uint128_t b2 = (bj << 32u) + bj1;
    return std::min(static_cast<uint32_t>(a3 / b2), UINT32_MAX);
}

bool big_integer::compare_prefix(const big_integer &a, const big_integer &b, size_t pos, size_t m) {
    size_t j;
    bool ok = false;
    for (int32_t i = m; i >= 0; i--) {
        if (a[i + pos] != b[i]) {
            j = i;
            ok = true;
            break;
        }
    }
    return ok && (a[j + pos] < b[j]);
}

big_integer& big_integer::difference(const big_integer& b, size_t offset, size_t m) {
    big_integer& a = *this;
    uint32_t borrow = 0;
    for (size_t i = 0; i <= m; i++) {
        uint64_t cur = static_cast<uint64_t>(a[i + offset]) - b[i] - borrow;
        borrow = (cur >> 32u) != 0;
        if (i + offset < a.data_.size()) {
            a.data_[i + offset] = static_cast<uint32_t>(cur);
        } else {
            a.data_.push_back(static_cast<uint32_t>(cur));
        }
    }
    shrink();
    return a;
}

big_integer& big_integer::operator/=(big_integer const& rhs)
{
    big_integer &a = *this;
    big_integer b = rhs;
    bool res_sign = false;
    if (a.sign) {
        res_sign ^= a.sign;
        a = -a;
    }
    if (b.sign) {
        res_sign ^= b.sign;
        b = -b;
    }
    size_t n = a.data_.size();
    size_t m = b.data_.size();
    big_integer c;
    bool zero = false;
    if (m == 1) {
        c = a;
        c.div_uint(b[0]);
    } else {
        if (m > n) {
            *this = 0;
            zero = true;
        } else {
            uint32_t base = static_cast<uint32_t>(
                    (1ull << 32) / static_cast<uint64_t>(b[m - 1] + 1)
            );
            a.mul_uint(base);
            a.data_.push_back(0);
            b.mul_uint(base);
            n = a.data_.size();
            m = b.data_.size();
            c = 0;
            c.expand(n - m + 1);
            for (int32_t i = n - m; i >= 0; i--) {
                uint32_t cur_digit = calc3to2(a, b, i, m);
                big_integer product = b;
                product.mul_uint(cur_digit);
                if (compare_prefix(a, product, i, m)) {
                    cur_digit--;
                    product = b;
                    product.mul_uint(cur_digit);
                }
                c.data_[i] = cur_digit;
                a.difference(product, i, m);
            }
        }
    }
    c.shrink();
    if (res_sign && !zero) {
        c = -c;
    }
    *this = c;
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs)
{
    return (*this) = ((*this) - rhs * ((*this) / rhs));
}

big_integer& big_integer::operator&=(big_integer const& rhs)
{
    sign &= rhs.sign;
    size_t l = std::max(rhs.data_.size(), data_.size());
    expand(l);
    uint32_t val = sign ? UINT32_MAX : 0u;
    for (size_t i = 0; i < l; i++) {
        data_[i] &= (i < rhs.data_.size()) ? rhs.data_[i] : val;
    }
    shrink();
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs)
{
    sign |= rhs.sign;
    size_t l = std::max(rhs.data_.size(), data_.size());
    expand(l);
    uint32_t val = sign ? UINT32_MAX : 0u;
    for (size_t i = 0; i < l; i++) {
        data_[i] |= (i < rhs.data_.size()) ? rhs.data_[i] : val;
    }
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs)
{
    sign ^= rhs.sign;
    size_t l = std::max(rhs.data_.size(), data_.size());
    expand(l);
    uint32_t val = sign ? UINT32_MAX : 0u;
    for (size_t i = 0; i < l; i++) {
        data_[i] ^= (i < rhs.data_.size()) ? rhs.data_[i] : val;
    }
    return *this;
}

big_integer& big_integer::operator<<=(int rhs)
{
    if (rhs < 0) {
        return ((*this) >>= (-rhs));
    }
    size_t mod = rhs % 32;
    size_t cnt = rhs / 32;
    *this *= (1u << mod);
    std::vector<uint32_t> &data = this->data_;
    reverse(data.begin(), data.end());
    for (size_t i = 0; i < cnt; i++) {
        data.push_back(0u);
    }
    reverse(data.begin(), data.end());
    if (data.empty()) {
        *this = 0;
    }
    shrink();
    return *this;
}

big_integer& big_integer::operator>>=(int rhs)
{
    if (rhs < 0) {
        return ((*this) <<= (-rhs));
    }
    size_t mod = rhs % 32;
    size_t cnt = rhs / 32;
    big_integer tmp = (!sign ? (*this) : -(*this));
    tmp /= (1u << mod);
    std::vector<uint32_t> &data = tmp.data_;
    reverse(data.begin(), data.end());
    for (;cnt && !data.empty(); cnt--) {
        data.pop_back();
    }
    reverse(data.begin(), data.end());
    if (sign) {
        tmp++;
        *this = -tmp;
    } else {
        *this = tmp;
    }
    shrink();
    return *this;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    if (*this == 0u) {
        return *this;
    }
    big_integer res(~(*this));
    res += 1;
    res.shrink();
    return res;
}

big_integer big_integer::operator~() const
{
    big_integer res(*this);
    res.sign = !res.sign;
    for (size_t i = 0; i < data_.size(); i++) {
        res.data_[i] = ~(res.data_[i]);
    }
    res.shrink();
    return res;
}

big_integer& big_integer::operator++()
{
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int)
{
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer& big_integer::operator--()
{
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int)
{
    big_integer r = *this;
    --*this;
    return r;
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

big_integer operator/(big_integer a, big_integer const& b)
{
    return a /= b;
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
    bool res = a.sign == b.sign;
    if (res) {
        res &= (a.data_ == b.data_);
    }
    return res;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
    bool res = a.sign != b.sign;
    if (!res) {
        res |= (a.data_ != b.data_);
    }
    return res;
}

bool operator<(big_integer const& a, big_integer const& b)
{
    big_integer c(a - b);
    return c.sign;
}

bool operator>(big_integer const& a, big_integer const& b)
{
    return !(a <= b);
}

bool operator<=(big_integer const& a, big_integer const& b)
{
    big_integer c(a - b);
    return c.sign || (c.data_.size() == 1 && c.data_[0] == 0u);
}

bool operator>=(big_integer const& a, big_integer const& b)
{
    return (a == b || a > b);
}

std::string to_string(big_integer const& a)
{
    if (a == 0) {
        return "0";
    }
    std::string res = "";
    big_integer b = a, c;
    bool res_sign = false;
    if (b < 0) {
        res_sign = b.sign;
        b *= -1;
    }
    while (b != 0) {
        c = (b / 10) * 10;
        char cur = '0' + (b - c).first();
        res.push_back(cur);
        b /= 10;
    }
    if (res_sign) {
        res.push_back('-');
    }
    std::reverse(res.begin(), res.end());
    return res;
}

void big_integer::shrink() {
    uint32_t x = sign ? (UINT32_MAX) : 0u;
    while (data_.size() > 1 && data_.back() == x) {
        data_.pop_back();
    }
}

void big_integer::expand(size_t len) {
    if (data_.size() > len) { return; }
    size_t cnt = len - data_.size();
    for (size_t i = 0; i < cnt; i++) {
        data_.push_back(sign ? UINT32_MAX : 0u);
    }
}

uint32_t big_integer::operator[](size_t index) const {
    return (index < data_.size() ? data_[index] : (sign ? UINT32_MAX : 0u));
}

uint32_t big_integer::first() {
    return data_[0];
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}
