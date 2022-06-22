#include "big_integer.h"
#include <cstddef>
#include <stdexcept>

typedef std::vector<uint32_t> digits;
static constexpr uint64_t ONE_64 = 1;
static constexpr uint64_t POW32 = ONE_64 + UINT32_MAX;

template<typename T>
uint32_t cast_to_uint32_t(T x) {
  return static_cast<uint32_t>(x);
}

digits mul_uint32_t(digits const& d, uint32_t x) {
  uint64_t carry = 0;
  digits new_data(d.size() + 1);
  for (size_t i = 0; i < d.size(); i++) {
    uint64_t tmp = uint64_t(d[i]) * x + carry;
    new_data[i] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
  new_data[d.size()] = cast_to_uint32_t(carry);
  return new_data;
}

big_integer::big_integer() : data_(0), sgn_(false) {}

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(int a) : big_integer(static_cast<long>(a)) {}

big_integer::big_integer(unsigned a) : big_integer(static_cast<unsigned long>(a)) {}

big_integer::big_integer(long a) : big_integer(static_cast<long long>(a)) {}

big_integer::big_integer(unsigned long a) : big_integer(static_cast<unsigned long long>(a)) {}

big_integer::big_integer(long long a) : sgn_(a < 0) {
  push(cast_to_uint32_t(a));
  push(cast_to_uint32_t(a >> 32));
}

big_integer::big_integer(unsigned long long a) : sgn_(false) {
  while (a != 0) {
    push(cast_to_uint32_t(a));
    a >>= 32;
  }
}

big_integer::big_integer(std::string const& str) : big_integer() {
  bool tmp_sgn = str[0] == '-';
  if (str.substr(tmp_sgn).empty()) {
    throw std::invalid_argument("Can't parse empty string to big_integer");
  }
  uint32_t tmp = 0, pow = 1;
  uint32_t MAX_TMP = (UINT32_MAX - 9) / 10, MAX_POW = UINT32_MAX / 10;
  for (size_t i = tmp_sgn; i < str.length(); i++) {
    if (!(str[i] >= '0' && str[i] <= '9')) {
      throw std::invalid_argument("Error while parsing number");
    }
    tmp = tmp * 10 + (str[i] - '0');
    pow *= 10;
    if (tmp > MAX_TMP || pow > MAX_POW) {
      *this *= pow;
      if (tmp_sgn) {
        *this -= tmp;
      } else {
        *this += tmp;
      }
      tmp = 0;
      pow = 1;
    }
  }
  if (tmp != 0 || pow > 1) {
    *this *= pow;
    if (tmp_sgn) {
      *this -= tmp;
    } else {
      *this += tmp;
    }
  }
}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) = default;

big_integer& big_integer::operator+=(big_integer const& rhs) {
  size_t new_size = std::max(size(), rhs.size()) + 1;
  expand(new_size, get_zero());
  uint64_t carry = 0;
  for (size_t i = 0; i < new_size; i++) {
    uint64_t tmp = carry + data_[i] + rhs[i];
    data_[i] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
  sgn_ = data_.back() & (1 << 31);
  return delete_leading_zeroes();
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
  size_t new_size = std::max(size(), rhs.size()) + 1;
  expand(new_size, get_zero());
  uint64_t carry = 1;
  for (size_t i = 0; i < new_size; i++) {
    uint64_t tmp = carry + data_[i] + ~rhs[i];
    data_[i] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
  sgn_ = data_.back() & (1 << 31);
  return delete_leading_zeroes();
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
  if (eq_zero() || rhs.eq_zero()) return *this;
  big_integer c = abs();
  big_integer d = rhs.abs();
  if (d.size() == 1) {
    data_ = mul_uint32_t(c.data_, d[0]);
  } else if (c.size() == 1) {
    data_ = mul_uint32_t(d.data_, c[0]);
  } else {
    data_.clear();
    expand(c.size() + d.size() + 1, 0);
    for (size_t i = 0; i < c.size(); i++) {
      uint64_t carry = 0;
      for (size_t j = 0; j < d.size(); j++) {
        uint64_t mul = (uint64_t) c[i] * d[j];
        uint64_t tmp = carry + data_[i + j] + cast_to_uint32_t(mul);
        data_[i + j] = cast_to_uint32_t(tmp);
        carry = (tmp >> 32) + (mul >> 32);
      }
      data_[i + d.size()] += cast_to_uint32_t(carry);
    }
  }
  sgn_ ^= rhs.sgn_;
  return norm();
}

void difference(digits& a, digits const& b, size_t x) {
  uint64_t carry = 1;
  for (size_t i = 0; i < b.size(); i++) {
    uint64_t tmp = carry + a[i + x] + ~b[i];
    a[i + x] = cast_to_uint32_t(tmp);
    carry = tmp >> 32;
  }
}

uint32_t trial(uint32_t const a, uint32_t const b, uint32_t const div) {
  return cast_to_uint32_t(std::min(POW32, ((uint64_t(a) << 32) + b) / div));
}

bool smaller(digits const& a, digits const& b, size_t x) {
  for (size_t i = b.size() - 1; ; i--) {
    if (a[i + x] != b[i]) {
      return a[i + x] < b[i];
    }
    if (i == 0) {
      break;
    }
  }
  return false;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
  if (rhs.eq_zero()) {
    throw std::invalid_argument("Error while evaluating a / b: division by zero");
  }
  big_integer c = abs();
  big_integer d = rhs.abs();
  if (c < d) {
    data_.clear();
    sgn_ = false;
    return *this;
  }
  if (d.size() == 1) {
    data_ = c.div_uint32_t(d[0]);
  } else {
    digits x = c.data_, y = d.data_, dq;
    uint32_t f = cast_to_uint32_t(POW32 / (ONE_64 + y.back()));
    size_t ys = y.size();
    digits q = mul_uint32_t(x, f), r = mul_uint32_t(y, f);
    while (!q.empty() && q.back() == 0) q.pop_back();
    while (!r.empty() && r.back() == 0) r.pop_back();
    uint32_t div = r.back();
    expand(x.size() - ys + 1, 0);
    q.push_back(0);
    for (size_t k = data_.size() - 1; ; k--) {
      uint32_t qt = trial(q[k + ys], q[k + ys - 1], div);
      dq = mul_uint32_t(r, qt);
      while (smaller(q, dq, k)) {
        qt--;
        dq = mul_uint32_t(r, qt);
      }
      data_[k] = qt;
      difference(q, dq, k);
      if (k == 0) {
        break;
      }
    }
  }
  sgn_ ^= rhs.sgn_;
  return norm();
}


big_integer& big_integer::operator%=(big_integer const& rhs) {
  return *this -= *this / rhs * rhs;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
  return bit_operation(bit_and, rhs);
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
  return bit_operation(bit_or, rhs);
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
  return bit_operation(bit_xor, rhs);
}

big_integer& big_integer::operator<<=(int rhs) {
  if (rhs < 0) {
    return operator>>=(-rhs);
  }
  digits new_data(rhs / 32, 0);
  size_t mod = rhs % 32;
  for (size_t i = 0; i <= size(); i++) {
    uint32_t tmp = (operator[](i) << mod) & UINT32_MAX;
    if (i > 0) tmp += (operator[](i - 1) >> (32 - mod));
    new_data.push_back(tmp);
  }
  data_ = new_data;
  return delete_leading_zeroes();
}

big_integer& big_integer::operator>>=(int rhs) {
  if (rhs < 0) {
    return operator<<=(-rhs);
  }
  digits new_data;
  size_t mod = rhs % 32;
  for (size_t i = rhs / 32; i < size(); i++) {
    new_data.push_back((operator[](i) >> mod) + ((operator[](i + 1) << (32 - mod)) & UINT32_MAX));
  }
  data_ = new_data;
  return delete_leading_zeroes();
}

big_integer big_integer::operator+() const {
  return *this;
}

big_integer big_integer::operator-() const {
  if (eq_zero()) return *this;
  return ~*this + 1;
}

big_integer big_integer::operator~() const {
  big_integer ans(*this);
  ans.sgn_ ^= true;
  for (uint32_t& i : ans.data_) {
    i = ~i;
  }
  return ans.delete_leading_zeroes();
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
  return a.sgn_ == b.sgn_ && a.data_ == b.data_;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
  if (a.sgn_ != b.sgn_) {
    return a.sgn_;
  }
  if (a.size() != b.size()) {
    return a.size() < b.size();
  }
  for (size_t i = a.size() - 1; ; i--) {
    if (a[i] != b[i]) {
      return a[i] < b[i];
    }
    if (i == 0) {
      break;
    }
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
  if (a.data_.empty()) {
    return a.sgn_ ? "-1" : "0";
  }
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
  while (ans.length() > 0 && ans[ans.length() - 1] == '0') {
    ans.pop_back();
  }
  if (a.sgn_) {
    ans.push_back('-');
  }
  std::reverse(ans.begin(), ans.end());
  return ans;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  return s << to_string(a);
}

size_t big_integer::size() const {
  return data_.size();
}

/// Private

const std::function<uint32_t(uint32_t, uint32_t)> big_integer::bit_and = [](uint32_t a, uint32_t b) { return a & b; };
const std::function<uint32_t(uint32_t, uint32_t)> big_integer::bit_or  = [](uint32_t a, uint32_t b) { return a | b; };
const std::function<uint32_t(uint32_t, uint32_t)> big_integer::bit_xor = [](uint32_t a, uint32_t b) { return a ^ b; };

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

big_integer& big_integer::bit_operation(std::function<uint32_t(uint32_t, uint32_t)> const& f, big_integer const& b) {
  digits new_data(std::max(size(), b.size()));
  for (size_t i = 0; i < new_data.size(); i++) new_data[i] = f(operator[](i), b[i]);
  data_ = new_data;
  sgn_ = f(sgn_, b.sgn_);
  delete_leading_zeroes();
  return *this;
}

big_integer& big_integer::delete_leading_zeroes() {
  while (!data_.empty() && ((sgn_ && data_[size() - 1] == UINT32_MAX) || (!sgn_ && data_[size() - 1] == 0))) {
    data_.pop_back();
  }
  return *this;
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

big_integer& big_integer::norm() {
  if (sgn_) {
    uint64_t carry = 1, tmp = 0;
    data_.resize(size() + 1);
    for (size_t i = 0; i < size(); i++) {
      tmp = carry + (~operator[](i));
      data_[i] = cast_to_uint32_t(tmp);
      carry = tmp >> 32;
    }
  }
  delete_leading_zeroes();
  return *this;
}

void big_integer::push(uint32_t x) {
  if (sgn_ && x == UINT32_MAX || !sgn_ && x == 0) return;
  data_.push_back(x);
}

uint32_t big_integer::get_zero() const {
  return sgn_ ? UINT32_MAX : 0;
}

void big_integer::expand(size_t x, uint32_t y) {
  while (data_.size() > x) {
    data_.pop_back();
  }
  while (data_.size() < x) {
    data_.push_back(y);
  }
}

