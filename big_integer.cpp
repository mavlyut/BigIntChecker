#include "big_integer.h"
#include <cstddef>
#include <stdexcept>
#include <functional>
#include <utility>

big_integer::big_integer() : _data(1, 0u), _sgn(false) {}

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(int a) : big_integer(static_cast<long long>(a)) {}

big_integer::big_integer(long a) : big_integer(static_cast<long long>(a)) {}

big_integer::big_integer(long long a) {
  feel(static_cast<uint64_t>(std::abs(a)));
  if (a < 0) *this = -*this;
}

big_integer::big_integer(unsigned int a) {
  feel(static_cast<uint64_t>(a));
}

big_integer::big_integer(unsigned long a) {
  feel(static_cast<uint64_t>(a));
}

big_integer::big_integer(unsigned long long a) {
  feel(a);
}

void big_integer::feel(uint64_t a) {
  uint32_t x = a >> 32;
  _data.push_back(a - x);
  _data.push_back(x);
  _sgn = false;
  shrink_to_fit();
}

big_integer::big_integer(std::vector<uint32_t> data, bool sgn) : _data(std::move(data)), _sgn(sgn) {}

big_integer::big_integer(std::string const& str) : big_integer() {
  if (str.length() == 0) throw std::invalid_argument("can't parse empty string");
  if (str[0] == '-' && str.length() == 1) throw std::invalid_argument("can't parse string");
  for (size_t i = (str[0] == '-' ? 1 : 0); i < str.length(); i++) {
    if ('0' > str[i] || str[i] > '9') throw std::invalid_argument("can't parse string");
    *this *= 10;
    *this += (str[i] - '0');
  }
  bool f = true;
  for (size_t i = 0; i < length(); i++)
    if (_data[i] != UINT32_MAX) {
      f = false;
      break;
    }
  if (str[0] == '-') {
    if (f) {
      *this = ~(*this);
      (*this)--;
    } else *this = -*this;
  }
}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) = default;

big_integer& big_integer::operator+=(big_integer const& rhs) {
  reserve(std::max<size_t>(rhs.length(), length()) + 1);
  uint64_t carry = 0;
  for (size_t i = 0; i < length(); i++) {
    uint64_t res = _data[i] + carry + rhs[i];
    carry = res >> 32u;
    _data[i] = static_cast<uint32_t>(res);
  }
  _sgn = _data.back() >> 31u;
  shrink_to_fit();
  return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
  return *this += -rhs;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
  if (rhs == -1) return *this = -*this;
  big_integer &a = *this;
  big_integer b = rhs;
  bool sgn = false;
  if (a._sgn) {
    sgn ^= a._sgn;
    a = -a;
  }
  if (b._sgn) {
    sgn ^= b._sgn;
    b = -b;
  }
  big_integer c;
  c.reserve(b.length() + a.length());
  for (size_t i = 0; i < a.length(); i++) {
    uint64_t carry = 0;
    for (size_t j = 0; j < b.length() || carry; j++) {
      uint64_t tmp = a._data[i] * static_cast<uint64_t>(j < b.length() ? b._data[j] : 0u) + c._data[i + j] + carry;
      c._data[i + j] = static_cast<uint32_t>(tmp);
      carry = tmp >> 32;
    }
  }
  if (sgn) c = -c;
  c.shrink_to_fit();
  return *this = c;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
  big_integer &a = *this;
  big_integer b = rhs;
  bool sgn = false;
  if (a._sgn) {
    sgn ^= a._sgn;
    a = -a;
  }
  if (b._sgn) {
    sgn ^= b._sgn;
    b = -b;
  }
  size_t n = a.length();
  size_t m = b.length();
  big_integer c;
  bool zero = false;
  if (m == 1) {
    c = a;
    c.div(b[0]);
  } else if (m > n) {
    *this = 0;
    zero = true;
  } else {
    uint32_t base = static_cast<uint32_t>((1ull << 32) / static_cast<uint64_t>(b[m - 1] + 1));
    a.mul(base);
    a._data.push_back(0);
    b.mul(base);
    n = a.length();
    m = b.length();
    c = 0;
    c.reserve(n - m + 1);
    for (int32_t i = n - m; i >= 0; i--) {
      uint32_t tmp = trial(a, b, i, m);
      big_integer d = b;
      d.mul(tmp);
      if (smaller(a, d, i, m)) {
        tmp--;
        d = b;
        d.mul(tmp);
      }
      c._data[i] = tmp;
      a.difference(d, i, m);
    }
  }
  c.shrink_to_fit();
  if (sgn && !zero) c = -c;
  return *this = c;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
  return (*this) -= *this / rhs * rhs;
}

big_integer& big_integer::make_bool_op(const std::function<uint32_t(uint32_t, uint32_t)>& f, big_integer const& rhs) {
  _sgn = f(_sgn, rhs._sgn);
  size_t size = std::max<size_t>(rhs.length(), length());
  reserve(size);
  for (size_t i = 0; i < size; i++) _data[i] = f(_data[i], i < rhs.length() ? rhs[i] : _default());
  shrink_to_fit();
  return *this;
}

const std::function<uint32_t(uint32_t, uint32_t)>& _and = [](uint32_t x, uint32_t y) -> uint32_t {return x & y;};
const std::function<uint32_t(uint32_t, uint32_t)>& _or = [](uint32_t x, uint32_t y) -> uint32_t {return x | y;};
const std::function<uint32_t(uint32_t, uint32_t)>& _xor = [](uint32_t x, uint32_t y) -> uint32_t {return x ^ y;};

big_integer& big_integer::operator&=(big_integer const& rhs) {
  return make_bool_op(_and, rhs);
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
  return make_bool_op(_or, rhs);
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
  return make_bool_op(_xor, rhs);
}

big_integer& big_integer::operator<<=(int rhs) {
  if (rhs < 0) return (*this >>= -rhs);
  *this *= (1u << (rhs % 32));
  reverse(_data.begin(), _data.end());
  for (size_t i = 0; i < rhs / 32; i++) _data.push_back(0u);
  reverse(_data.begin(), _data.end());
  shrink_to_fit();
  return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
  if (rhs < 0) return (*this <<= -rhs);
  big_integer tmp = (_sgn ? -*this : *this);
  tmp.div(1u << (rhs % 32));
  reverse(tmp._data.begin(), tmp._data.end());
  for (uint32_t cnt = rhs / 32; cnt > 0 && !tmp._data.empty(); cnt--) tmp._data.pop_back();
  reverse(tmp._data.begin(), tmp._data.end());
  *this = _sgn ? -(++tmp) : tmp;
  shrink_to_fit();
  return *this;
}

big_integer big_integer::operator+() const {
  return *this;
}

big_integer big_integer::operator-() const {
  if (*this == 0u) return *this;
  big_integer res(~*this);
  (++res).shrink_to_fit();
  return res;
}

big_integer big_integer::operator~() const {
  big_integer res(*this);
  res._sgn ^= true;
  for (size_t i = 0; i < length(); i++) res._data[i] = ~res._data[i];
  res.shrink_to_fit();
  return res;
}

big_integer& big_integer::operator++() {
  return *this += 1;
}

big_integer big_integer::operator++(int) {
  big_integer res = *this;
  ++*this;
  return res;
}

big_integer& big_integer::operator--() {
  return *this -= 1;
}

big_integer big_integer::operator--(int) {
  big_integer res = *this;
  --*this;
  return res;
}

big_integer operator+(big_integer a, big_integer const& b) {
  return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
  return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
  return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
  return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
  return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
  return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
  return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
  return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
  return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
  return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b) {
  return a._sgn == b._sgn && a._data == b._data;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
  return (a - b)._sgn;
}

bool operator>(big_integer const& a, big_integer const& b) {
  return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b) {
  return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b) {
  return !(a < b);
}

std::string to_string(big_integer const& a) {
  if (a == 0) return "0";
  std::string res;
  big_integer b = a;
  if (b._sgn) b = -b;
  while (b != 0) {
    res.push_back(b._data[0] % 10 + '0');
    b.div(10);
  }
  if (a._sgn) res.push_back('-');
  std::reverse(res.begin(), res.end());
  return res;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  return s << to_string(a);
}

//// Private

size_t big_integer::length() const {
  return _data.size();
}

uint32_t big_integer::operator[](size_t ind) const {
  return ind < length() ? _data[ind] : _default();
}

void big_integer::reserve(size_t new_capacity) {
  for (size_t i = 0; i < new_capacity - length(); i++) _data.push_back(_default());
}

void big_integer::shrink_to_fit() {
  while (length() > 1 && _data.back() == _default()) _data.pop_back();
  if (_data.empty()) *this = 0;
}

uint32_t big_integer::trial(const big_integer &a, const big_integer &b, size_t i, size_t j) {
  size_t ij = i + j;
  __uint128_t ai = a[ij];
  __uint128_t ai1 = a[ij - 1];
  __uint128_t ai2 = a[ij - 2];
  __uint128_t bj = b[j - 1];
  __uint128_t bj1 = b[j - 2];
  __uint128_t a3 = (ai << 64u) + (ai1 << 32u) + ai2;
  __uint128_t b2 = (bj << 32u) + bj1;
  return std::min(static_cast<uint32_t>(a3 / b2), UINT32_MAX);
}

bool big_integer::smaller(const big_integer &a, const big_integer &b, size_t pos, size_t m) {
  size_t j;
  bool ans = false;
  for (int32_t i = m; i >= 0; i--)
    if (a[i + pos] != b[i]) {
      j = i;
      ans = true;
      break;
    }
  return ans && (a[j + pos] < b[j]);
}

big_integer& big_integer::difference(const big_integer& b, size_t offset, size_t m) {
  big_integer& a = *this;
  uint32_t carry = 0;
  for (size_t i = 0; i <= m; i++) {
    uint64_t tmp = static_cast<uint64_t>(a[i + offset]) - b[i] - carry;
    carry = (tmp >> 32u) != 0;
    if (i + offset < a.length()) a._data[i + offset] = static_cast<uint32_t>(tmp);
    else a._data.push_back(static_cast<uint32_t>(tmp));
  }
  shrink_to_fit();
  return a;
}

uint32_t big_integer::_default() const {
  return _sgn ? UINT32_MAX : 0u;
}

big_integer& big_integer::mul(uint32_t rhs) {
  uint32_t carry = 0u;
  for (size_t i = 0; i < length(); i++) {
    uint64_t tmp = _data[i] * rhs + carry;
    carry = tmp >> 32u;
    _data[i] = static_cast<uint32_t>(tmp);
  }
  if (carry != 0) _data.push_back(carry);
  return *this;
}

big_integer& big_integer::div(uint32_t b) {
  uint64_t carry = 0u;
  for (int32_t i = length() - 1; i >= 0; i--) {
    uint64_t tmp = _data[i] + (carry << 32u);
    _data[i] = tmp / b;
    carry = tmp % b;
  }
  shrink_to_fit();
  return *this;
}


