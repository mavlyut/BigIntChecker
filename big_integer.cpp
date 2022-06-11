#include "big_integer.h"
#include <cstddef>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <utility>

typedef std::vector<uint32_t> digits;
static constexpr uint64_t ONE_64 = 1;

uint32_t cast_to_uint32_t(uint64_t x) {
  return (uint32_t) (x & UINT32_MAX);
}

big_integer::big_integer() : sgn_(false), data_(0) {}

big_integer::big_integer(big_integer const& other) : sgn_(other.sgn_), data_(other.data_) {
  delete_leading_zeroes();
}

big_integer::big_integer(int a) : sgn_(a < 0), data_(1, cast_to_uint32_t(a)) {
  delete_leading_zeroes();
}

big_integer::big_integer(std::string const& str) : big_integer() {
  uint32_t tmp = 0, pow = 1;
  uint32_t MAX_TMP = (UINT32_MAX - 9) / 10, MAX_POW = UINT32_MAX / 10;
  bool tmp_sgn = str[0] == '-';
  for (size_t i = tmp_sgn; i < str.length(); i++) {
    if (!(str[i] >= '0' && str[i] <= '9')) throw std::runtime_error("Error while parsing number");
    tmp = tmp * 10 + (str[i] - '0');
    pow *= 10;
    if (tmp > MAX_TMP || pow > MAX_POW) {
      *this *= pow;
      if (tmp_sgn) *this -= tmp;
      else *this += tmp;
      tmp = 0;
      pow = 1;
    }
  }
  if (tmp != 0 || pow > 1) {
    *this *= pow;
    if (tmp_sgn) *this -= tmp;
    else *this += tmp;
  }
}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) = default;

big_integer& big_integer::operator+=(big_integer const& rhs) {
  return *this = *this + rhs;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
  return *this = *this - rhs;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
  return *this = *this * rhs;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
  return *this = *this / rhs;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
  return *this = *this % rhs;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
  return *this = *this & rhs;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
  return *this = *this | rhs;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
  return *this = *this ^ rhs;
}

big_integer& big_integer::operator<<=(int rhs) {
  return *this = *this << rhs;
}

big_integer& big_integer::operator>>=(int rhs) {
  return *this = *this >> rhs;
}

big_integer big_integer::operator+() const {
  return *this;
}

// todo, done?
big_integer big_integer::operator-() const {
  if (eq_zero()) return *this;
  return ~*this + 1;
}

big_integer big_integer::operator~() const {
  digits new_data(size());
  for (size_t i = 0; i < size(); i++) new_data[i] = ~data_[i];
  return big_integer(new_data, !sgn_);
}

big_integer& big_integer::operator++() {
  *this += 1;
  return *this;
}

big_integer big_integer::operator++(int) {
  big_integer tmp(*this);
  ++*this;
  return tmp;
}

big_integer& big_integer::operator--() {
  *this -= 1;
  return *this;
}

big_integer big_integer::operator--(int) {
  big_integer tmp(*this);
  --*this;
  return tmp;
}

big_integer operator+(big_integer a, big_integer const& b) {
  size_t new_size = std::max(a.size(), b.size()) + 1;
  digits new_data(new_size);
  uint32_t carry = 0;
  for (size_t i = 0; i < new_size; i++) {
    uint64_t tmp = carry + a[i] + b[i];
    new_data[i] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
  return big_integer(new_data, new_data[new_size - 1] & (1 << 31));
}

big_integer operator-(big_integer a, big_integer const& b) {
  size_t new_size = std::max(a.size(), b.size()) + 1;
  digits new_data(new_size);
  uint64_t carry = 1;
  for (size_t i = 0; i < new_size; i++) {
    uint64_t tmp = carry + a[i] + (~b[i]);
    new_data[i] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
  return big_integer(new_data, (new_data[new_size - 1] & (1 << 31)));
}

big_integer operator*(big_integer a, big_integer const& b) {
  if (a.eq_zero() || b.eq_zero()) {
    return 0;
  }
  big_integer c = a.abs();
  big_integer d = b.abs();
  if (c.size() > d.size()) c.swap(d);
  digits new_data;
  if (c.size() == 1) new_data = d.mul_uint32_t(c[0]);
  else {
    new_data.resize(c.size() + d.size() + 1, 0);
    for (size_t i = 0; i < c.size(); i++) {
      uint64_t carry = 0;
      for (size_t j = 0; j < d.size(); j++) {
        uint64_t mul = (uint64_t) c[i] * d[i];
        uint64_t tmp = carry + new_data[i + j] + cast_to_uint32_t(mul);
        new_data[i + j] = cast_to_uint32_t(tmp);
        carry = (tmp >> 32) + (mul >> 32);
      }
      new_data[i + d.size()] += cast_to_uint32_t(carry);
    }
  }
  return big_integer(new_data, a.sgn() ^ b.sgn()).norm();
}

uint32_t trial(uint32_t const a, uint32_t const b, uint32_t const div) {
  return cast_to_uint32_t(std::min(ONE_64 + UINT32_MAX, ((uint64_t(a) << 32) + b) / div));
}

void delete_leading_zeroes(digits& x) {
  while (!x.empty() && x.back() == 0) x.pop_back();
}

bool smaller(digits const& a, digits const& b, size_t x) {
  for (size_t i = b.size() - 1; ; i--) {
    if (a[i + x] != b[i]) return a[i + x] < b[x];
    if (i == 0) break;
  }
  return false;
}

void difference(digits& a, digits const& b, size_t x) {
  uint64_t carry = 1;
  for (size_t i = 0; i < b.size(); i++) {
    uint64_t tmp = carry + a[i + x] + ~b[x];
    a[i + x] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
}

big_integer operator/(big_integer a, big_integer const& b) {
  if (b.eq_zero()) throw std::runtime_error("Error while evaluating a / b: division by zero");
  big_integer c = a.abs();
  big_integer d = b.abs();
  if (c < d) return 0;
  digits new_data;
  if (d.size() == 1) new_data = c.div_uint32_t(d[0]);
  else {
    uint32_t f = cast_to_uint32_t((ONE_64 + UINT32_MAX) / (ONE_64 + d.data_.back()));
    digits x = c.data_, y = d.data_, dq;
    size_t ys = y.size();
    digits q1 = mul_uint32_t(x, f), q2 = mul_uint32_t(y, f);
    delete_leading_zeroes(q1);
    delete_leading_zeroes(q2);
    uint32_t div = q2.back();
    new_data.resize(x.size() - ys + 2);
    q1.push_back(0);
    for (size_t k = new_data.size() - 1; ; k--) {
      size_t qt = trial(q1[k + ys], q1[k + ys - 1], div);
      dq = mul_uint32_t(q2, qt);
      while (smaller(q1, dq, k)) {
        qt--;
        dq = mul_uint32_t(q2, qt);
      }
      new_data[k] = qt;
      difference(q1, dq, k);
      if (k == 0) break;
    }
  }
  return big_integer(new_data, a.sgn() ^ b.sgn()).norm();
}

big_integer operator%(big_integer a, big_integer const& b) {
  return a - a / b * b;
}

big_integer operator&(big_integer a, big_integer const& b) {
  return bit_operation(big_integer::bit_and, a, b);
}

big_integer operator|(big_integer a, big_integer const& b) {
  return bit_operation(big_integer::bit_or, a, b);
}

big_integer operator^(big_integer a, big_integer const& b) {
  return bit_operation(big_integer::bit_xor, a, b);
}

big_integer operator<<(big_integer a, int b) {
  digits new_data(b / 32, 0);
  size_t mod = b % 32;
  new_data.push_back((a[0] << mod) & UINT32_MAX);
  for (size_t i = 1; i < a.size(); i++) {
    new_data.push_back((a[i] << mod) & UINT32_MAX + (a[i - 1] >> (32 - mod)));
  }
  return big_integer(new_data, a.sgn_);
}

big_integer operator>>(big_integer a, int b) {
  digits new_data;
  size_t mod = b % 32;
  for (size_t i = b / 32; i < a.size(); i++) {
    new_data.push_back((a[i] >> mod) + (a[i + 1] << (32 - mod)) & UINT32_MAX);
  }
  return big_integer(new_data, a.sgn_);
}

bool operator==(big_integer const& a, big_integer const& b) {
  return a.sgn_ == b.sgn_ && a.data_ == b.data_;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
  if (a.sgn_ != b.sgn_) return a.sgn_;
  if (a.size() != b.size()) return a.size() < b.size();
  for (size_t i = a.size() - 1; ; i--) {
    if (a[i] != b[i]) return a[i] < b[i];
    if (i == 0) break;
  }
  return false;
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
  if (a.data_.empty()) return a.sgn_ ? "-1" : "0";
  std::string ans;
  big_integer b = a.abs();
  while (!b.eq_zero()) {
    uint32_t tmp = (b % 1000000000)[0];
    for (size_t i = 0; i < 9; i++) {
      ans.push_back(char((tmp % 10) + '0'));
      tmp /= 10;
    }
    b /= 1000000000;
  }
  while (ans[ans.length() - 1] == '0') ans.pop_back();
  if (a.sgn_) ans.push_back('-');
  std::reverse(ans.begin(), ans.end());
  return ans;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  return s << to_string(a);
}

size_t big_integer::size() const {
  return data_.size();
}

bool big_integer::sgn() const {
  return sgn_;
}

/// Private

const std::function<uint32_t(uint32_t, uint32_t)> big_integer::bit_and = [](uint32_t a, uint32_t b) { return a & b; };
const std::function<uint32_t(uint32_t, uint32_t)> big_integer::bit_or  = [](uint32_t a, uint32_t b) { return a | b; };
const std::function<uint32_t(uint32_t, uint32_t)> big_integer::bit_xor = [](uint32_t a, uint32_t b) { return a ^ b; };

big_integer::big_integer(digits data, bool sgn) : data_(std::move(data)), sgn_(sgn) {
  delete_leading_zeroes();
}

void big_integer::swap(big_integer &other) {
  std::swap(data_, other.data_);
  std::swap(sgn_, other.sgn_);
}

bool big_integer::eq_zero() const {
  return data_.empty() && !sgn_;
}

uint32_t big_integer::operator[](size_t ind) const {
  if (ind >= size()) return sgn_ ? UINT32_MAX : 0;
  return data_[ind];
}

big_integer big_integer::abs() const {
  return sgn_ ? -*this : *this;
}

big_integer bit_operation(std::function<uint32_t(uint32_t, uint32_t)> f, big_integer a, big_integer const& b) {
  digits new_data(std::max(a.size(), b.size()));
  for (size_t i = 0; i < new_data.size(); i++) new_data[i] = f(a[i], b[i]);
  return big_integer(new_data, f(a.sgn_, b.sgn_));
}

void big_integer::delete_leading_zeroes() {
  while (!data_.empty() && (sgn_ && data_[size() - 1] == UINT32_MAX || !sgn_ && data_[size() - 1] == 0)) {
    data_.pop_back();
  }
}

digits big_integer::mul_uint32_t(uint32_t x) const {
  return ::mul_uint32_t(data_, x);
}

digits mul_uint32_t(digits const& d, uint32_t x) {
  uint32_t carry = 0;
  digits new_data(d.size() + 1);
  for (size_t i = 0; i < d.size(); i++) {
    uint64_t tmp = uint64_t(d[i]) * x + carry;
    new_data[i] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
  new_data[d.size()] = carry;
  return new_data;
}

digits big_integer::div_uint32_t(uint32_t x) const {
  uint64_t carry = 0;
  digits new_data(size());
  for (size_t i = size() - 1; ; i--) {
    uint64_t tmp = (carry << 32) + operator[](i);
    new_data[i] = cast_to_uint32_t(tmp / x);
    carry = tmp % x;
    if (i == 0) break;
  }
  return new_data;
}

big_integer big_integer::norm() const {
  if (!sgn_) return *this;
  uint64_t carry = 1, tmp = 0;
  digits new_data(size() + 1);
  for (size_t i = 0; i < size(); i++) {
    tmp = carry + ~operator[](i);
    new_data[i] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
  big_integer ans(new_data, sgn_);
  ans.delete_leading_zeroes();
  return ans;
}

