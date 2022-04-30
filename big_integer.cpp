#include "big_integer.h"
#include <stdexcept>
#include <exception>
#include <algorithm>

// private functions:

void big_integer::negate()
{
    signum = -signum;
}

void big_integer::mul(uint32_t val)
{
    uint64_t tmp = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp += (uint64_t) data[i] * (uint64_t) val;
        data[i] = (uint32_t) (tmp & max_int32);
        tmp >>= log_int32;
    }
    while (tmp)
    {
        data.push_back((uint32_t) (tmp & max_int32));
        tmp >>= log_int32;
    }
}

void big_integer::add(uint32_t val) // suppose : BigInt  >= 0
{
    uint64_t tmp = val;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp += (uint64_t) data[i];
        data[i] = (uint32_t) (tmp & max_int32);
        tmp >>= log_int32;
        if (!tmp)
            break;
    }
    if (tmp)
    {
        data.push_back((uint32_t) tmp);
    }
}

uint32_t big_integer::div(uint32_t val)
{
    uint32_t mod = 0, base_mod_val = (uint32_t) (base % val);
    uint64_t carry = 0, tmp = 0;
    for (size_t i = data.size() - 1;; --i)
    {
        mod = (uint32_t) ((uint64_t) mod * base_mod_val + (uint64_t) data[i]) % val;
        tmp = carry * base + (uint64_t) data[i];
        data[i] = (uint32_t) (tmp / val);
        carry = tmp % val;
        if (!i)
            break;
    }
    correct_size(0);
    return mod;
}

uint32_t big_integer::div10()
{
    uint32_t mod = 0;
    uint64_t carry = 0, tmp = 0;
    for (size_t i = data.size() - 1; ; --i)
    {
        mod = (uint32_t) ((uint64_t) mod * base_mod_ten + (uint64_t) data[i]) % 10;
        tmp = carry * base + (uint64_t) data[i];
        data[i] = (uint32_t) (tmp / 10ll);
        carry = tmp % 10ll;
        if (!i)
            break;
    }
    correct_size(0);
    return mod;
}

uint32_t big_integer::mod(uint32_t val)
{
    uint32_t ans = 0, base_mod_val = val == 10 ? base_mod_ten : (uint32_t) (base % val);
    for (size_t i = 0; i < data.size(); ++i)
    {
        ans = (uint32_t) ((uint64_t) ans * base_mod_val + data[i]) % 10;
    }
    return ans;
}

void big_integer::correct_size(size_t expected_size = 0)
{
    if (expected_size == 0)
    {
        while (data.size() > 1 && !data.back())
            data.pop_back();
    }
    else
    {
        while (data.size() < expected_size)
            data.push_back(0);
    }
    if (data.empty())
        data.push_back(0);
    if (!data.back())
        signum = 0;
}

void big_integer::add(big_integer const& b)  // suppose : sign(this) == sign(b)
{
    uint64_t tmp = 0;
    correct_size(b.size());
    for (size_t i = 0; i < b.size(); ++i)
    {
        tmp += (uint64_t) data[i] + (uint64_t) b[i];
        data[i] = (uint32_t) (tmp & max_int32);
        tmp >>= log_int32;
    }
    size_t pos = b.size();
    while (tmp)
    {
        correct_size(pos + 1);
        tmp += (uint64_t) data[pos];
        data[pos] = (uint32_t) (tmp & max_int32);
        tmp >>= log_int32;
    }
    correct_size();
}

void big_integer::subtract(big_integer const& b)  // suppose : sign(this) == sign(b), |this| >= |b|
{
    int64_t tmp = 0;
    for (size_t i = 0; i < b.size(); ++i)
    {
        tmp += (int64_t) data[i] - (int64_t) b[i];
        if (tmp < 0) {
            data[i] = (uint32_t) (tmp + (int64_t) base);
            tmp = -1;
        } else {
            data[i] = (uint32_t) (tmp);
            tmp = 0;
        }
    }
    size_t pos = b.size();
    while (tmp != 0)
    {
        tmp += (int64_t) data[pos];
        if (tmp < 0) {
            data[pos] = (uint32_t) (tmp + base);
            tmp = -1;
        } else {
            data[pos] = (uint32_t) (tmp);
            tmp = 0;
        }
    }
    correct_size();
}

void big_integer::mul_max_int32()
{
    data.push_back(0);
    for (size_t i = data.size() - 1; i > 0; --i)
        data[i] = data[i - 1];
    data[0] = 0;
}

void big_integer::div_max_int32()
{
    for (size_t i = 0; i < data.size() - 1; ++i)
        data[i] = data[i + 1];
    data.back() = 0;
    correct_size();
}

void big_integer::invert()
{
    for (size_t i = 0; i < size(); ++i)
        data[i] = ~data[i];
}

void big_integer::to_add_code()
{
    push_back(0), push_back(0);
    if (signum < 0)
        invert(), add(1), signum = 1;
}

void big_integer::to_simple_code()
{
    if (back() != 0)
        signum = -1, invert(), add(1);
    correct_size();
}

uint32_t& big_integer::operator[](size_t i)
{
    return data[i];
}

const uint32_t& big_integer::operator[](size_t i) const
{
    return data[i];
}

size_t big_integer::size() const
{
    return data.size();
}

uint32_t& big_integer::back()
{
    return data.back();
}

const uint32_t& big_integer::back() const
{
    return data.back();
}

void big_integer::pop_back()
{
    data.pop_back();
}

void big_integer::push_back(uint32_t x)
{
    data.push_back(x);
}

bool big_integer::empty() const
{
    return data.empty();
}


//constructors :

big_integer::big_integer()
: data(std::vector<uint32_t>(1, 0))
, signum(0)
{}

big_integer::big_integer(std::vector<uint32_t>&v, short signum = 1)
: data(v)
, signum(signum)
{}

big_integer::big_integer(int val)
{
    data.clear();
    data.push_back((uint32_t) std::abs((int64_t) val));
    signum = val ? (val < 0 ? -1 : 1) : 0;
}

big_integer::big_integer(uint32_t val)
{
    data.clear();
    data.push_back(val);
    signum = val != 0;
}

big_integer::big_integer(uint64_t val)
{
    data.clear();
    data.push_back(val & max_int32);
    data.push_back(val >> log_int32);
    correct_size();
    signum = val != 0;
}


big_integer::big_integer(std::string const& s)
{
    data.clear();
    data.push_back(0);
    signum = 0;
    for (char c : s)
    {
        if (c == '+')
        {
            signum = 1;
            continue;
        }
        if (c == '-')
        {
            signum = -1;
            continue;
        }
        if (!signum && c != '0') signum = 1;
        mul(10);
        add((uint32_t) (c - '0'));
    }
    if (signum == -1 && !data.back())
        signum = 0;
}

// copy constructor
big_integer::big_integer(big_integer const& other)
{
    data = other.data;
    signum = other.signum;
}

std::string big_integer::to_string() const
{
    std::string answer = "";
    big_integer cur = *this;
    while (cur.signum != 0)
    {
        answer.push_back(cur.div10() + '0');
    }
    if (signum < 0)
        answer.push_back('-');
    std::reverse(answer.begin(), answer.end());
    if (answer == "")
        answer = "0";
    return answer;
}

std::vector<uint32_t> big_integer::get_digits() const
{
    return data;
}

bool big_integer::is_deg2() const {
    if (back() != 1) return 0;
    for (size_t i = 0; i < size() - 1; ++i)
        if (data[i]) return 0;
    return 1;
}

// operators :

big_integer& big_integer::operator=(const big_integer& other)
{
    data = other.data;
    signum = other.signum;
    return *this;
}

bool operator==(big_integer const& self, big_integer const& other)
{
    if (self.size() != other.size() || self.signum != other.signum)
        return false;
    for (size_t i = 0; i < self.size(); ++i)
        if (self[i] != other[i])
            return false;
    return true;
}

bool operator!=(big_integer const& self, big_integer const& other)
{
    return !(self == other);
}

bool operator<(big_integer const&self, big_integer const& other)
{
    if (self.signum != other.signum)
        return self.signum < other.signum;
    if (self.size() > other.size())
        return self.signum < 0;
    if (self.size() < other.size())
        return self.signum > 0;
    for (size_t i = self.size() - 1; ; --i)
    {
        if (self[i] > other[i])
            return self.signum < 0;
        if (self[i] < other[i])
            return self.signum > 0;
        if (!i) break;
    }
    return false;
}

bool operator<=(big_integer const& self, big_integer const& other)
{
    return (self < other || self == other);
}

bool operator>(big_integer const& self, big_integer const& other)
{
    return !(self <= other);
}
bool operator>=(big_integer const& self, big_integer const& other)
{
    return !(self < other);
}


//iostream operators :
std::istream& operator>>(std::istream& in, big_integer& bint)
{
    std::string tmp;
    in >> tmp;
    bint = big_integer(tmp);
    return in;
}

std::ostream& operator<<(std::ostream& out, big_integer const& bint)
{
    out << bint.to_string();
    return out;
}


//arithmetic :

// unary:
big_integer operator-(big_integer const& a)
{
    big_integer ans = a;
    ans.signum = -ans.signum;
    return ans;
}

big_integer operator+(big_integer const& a)
{
    return a;
}

big_integer operator+(big_integer const& a, big_integer const& b)
{
    big_integer res = 0;
    if (a.signum == 0)
        return res = b;
    if (b.signum == 0)
        return res = a;
    if (a.signum == b.signum)
    {
        res = a;
        res.add(b);
        return res;
    }
    if (a.signum < 0)
    {
        big_integer tmp = a;
        tmp.negate();
        res = b;
        if (tmp > res) {
            tmp.subtract(res);
            res = -tmp;
        } else {
            res.subtract(tmp);
        }
    }
    else
    {
        big_integer tmp = b;
        tmp.negate();
        res = a;
        if (tmp > res) {
            tmp.subtract(res);
            res = -tmp;
        } else {
            res.subtract(tmp);
        }
    }
    res.correct_size();
    return res;
}

//binary
big_integer operator-(big_integer const& a, big_integer const& b)
{
    return a + (-b);
}

big_integer operator*(big_integer const& a, big_integer const& b)
{
    big_integer res = 0;
    if (!a.signum || !b.signum)
        return res;
    res.correct_size(a.size() + b.size());
    uint64_t tmp = 0;
    for (size_t i = 0; i < a.size(); ++i)
    {
        for (size_t j = 0; j < b.size(); ++j)
        {
            tmp += (uint64_t) res[i + j] + (uint64_t) a[i] * (uint64_t) b[j];
            res[i + j] = tmp & max_int32;
            tmp >>= log_int32;
        }
        size_t pos = i + b.size();
        while (tmp) {
            tmp += (uint64_t) res[pos];
            res[pos++] = tmp & max_int32;
            tmp >>= log_int32;
        }
    }
    res.correct_size();
    res.signum = a.signum * b.signum;
    return res;
}

big_integer operator/(big_integer const& a, big_integer const& b) {
    if (!b) {
        throw std::runtime_error("DBZ Exception");
    }
    if (!a) {
        return 0;
    }
    short signum = a.signum * b.signum;
    big_integer left = a * a.signum, right = b * b.signum;
    if (left < right)
        return 0;
    if (left == right)
        return signum;
    if (right.size() == 1) {
        left.div(right[0]);
        left.signum = signum;
        return left;
    }
    uint32_t normalization = (uint32_t) (base / (right.back() + 1));
    left.mul(normalization);
    right.mul(normalization);
    size_t n = left.size(), m = right.size();
    std::vector<uint32_t> q(n);
    big_integer beta = big_integer::base_deg(n - m) * right, tmp;
    for (size_t j = n - m; ; --j) {
        uint64_t q_star = 0;
        if (m + j - 1 < left.size())
            q_star = left[m + j - 1] / right.back();
        if (m + j < left.size())
            q_star = ((uint64_t) left[m + j] * base + left[m + j - 1]) / right.back();
        q[j] = (uint32_t) std::min(q_star, (uint64_t) max_int32);
        tmp = beta;
        tmp.mul(q[j]);
        tmp.correct_size();
        left -= tmp;
        int cnt = 0;
        while (left.signum == -1) {
            --q[j];
            left += beta;
            ++cnt;
        }
        beta.div_max_int32();
        if (!j) break;
    }
    big_integer answer(q, 1);
    answer.correct_size();
    answer.signum = signum;
    return answer;
}

big_integer operator%(big_integer const& a, big_integer const& b) {
    return a - (a / b) * b;
}

big_integer& operator+=(big_integer& res, big_integer const& param)
{
    return res = res + param;
}

big_integer& operator-=(big_integer& res, big_integer const& param)
{
    return res = res - param;
}

big_integer& operator*=(big_integer& res, big_integer const& param)
{
    return res = res * param;
    
}

big_integer& operator/=(big_integer& res, big_integer const& param)
{
    return res = res / param;
}

big_integer& operator%=(big_integer& res, big_integer const& param)
{
    return res = res % param;
}

big_integer& operator^=(big_integer& res, big_integer const& param)
{
    return res = res ^ param;
}

big_integer& operator|=(big_integer& res, big_integer const& param)
{
    return res = res | param;
}

big_integer& operator&=(big_integer& res, big_integer const& param)
{
    return res = res & param;
}

big_integer& operator<<=(big_integer& res, int rhs)
{
    return res = res << rhs;
}

big_integer& operator>>=(big_integer& res, int rhs)
{
    return res = res >> rhs;
}

big_integer big_integer::any_binary(big_integer const& a,
                                    big_integer const& b,
                                    std::function<uint32_t(uint32_t, uint32_t)> f)
{
    big_integer answer = a, tmp = b;
    answer.correct_size(tmp.size());
    tmp.correct_size(answer.size());
    answer.to_add_code();
    tmp.to_add_code();
    for (size_t i = 0; i < tmp.size(); ++i)
        answer[i] = f(answer[i], tmp[i]);
    answer.to_simple_code();
    return answer;
}

big_integer operator^(big_integer const& a, big_integer const& b)
{
    return big_integer::any_binary(a, b, [](uint32_t x, uint32_t y) {return x ^ y;});
}

big_integer operator|(big_integer const& a, big_integer const& b)
{
    return big_integer::any_binary(a, b, [](uint32_t x, uint32_t y) {return x | y;});
}

big_integer operator&(big_integer const& a, big_integer const& b)
{
    return big_integer::any_binary(a, b, [](uint32_t x, uint32_t y) {return x & y;});
}

big_integer operator<<(big_integer const& a, int rhs)
{
    if (rhs < 0) return a >> rhs;
    int complete = rhs >> 5;
    rhs &= 31;
    big_integer ans = a;
    ans.to_add_code();
    uint64_t tmp = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        tmp |= ((uint64_t) a[i] << rhs);
        ans[i] = tmp & max_int32;
        tmp >>= log_int32;
    }
    if (tmp) ans.push_back((uint32_t) tmp);
    ans.correct_size();
    return ans * big_integer::base_deg((size_t) complete);
}

big_integer operator>>(big_integer const& a, int rhs)
{
    if (rhs < 0) return a << rhs;
    int complete = rhs >> 5;
    rhs &= 31;
    big_integer ans = a;
    uint32_t tmp = 0;
    for (size_t i = a.size() - 1; ; --i) {
        uint32_t digit = ans[i];
        ans[i] = (tmp << (log_int32 - rhs)) + (ans[i] >> rhs);
        tmp = digit & ((1ll << rhs) - 1);
        if (!i) break;
    }
    ans.correct_size();
    if (!a.is_deg2() && a.signum == -1) ans -= 1;
    return ans * big_integer::base_deg((size_t) complete);
}


big_integer operator~(big_integer const& a)
{
    big_integer answer = a;
    answer.to_add_code();
    answer.invert();
    answer.to_simple_code();
    return answer;
}

bool operator!(big_integer const& x)
{
    return !x.signum;
}

big_integer big_integer::pow(int n)
{
    if (!n) return 1;
    big_integer d = pow(n >> 1);
    d *= d;
    if (n & 1) d *= *this;
    return d;
}

big_integer big_integer::absolute() const {
    big_integer answer = *this;
    answer.signum = abs(answer.signum);
    return answer;
}

big_integer big_integer::base_deg(size_t n) {
    std::vector<uint32_t> res_data(n + 1, 0);
    res_data.back() = 1;
    return big_integer(res_data, 1);
}


// destructor:
big_integer::~big_integer()
{
    data.clear();
}

std::string to_string(big_integer const& b)  {
    return b.to_string();
}
