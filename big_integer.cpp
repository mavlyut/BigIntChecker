#include "big_integer.h"
#include <cstddef>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <utility>

big_integer::big_integer() : _sgn(false) {}

big_integer::big_integer(big_integer const& other) : _data(other._data), _sgn(other._sgn) {}

big_integer::big_integer(int a) : _sgn(a < 0) {
  _data.push_back(a);
}

// todo
big_integer::big_integer(std::string const& str) {}

big_integer::big_integer(std::vector<uint32_t> data, bool sgn) : _data(std::move(data)), _sgn(sgn) {}

big_integer::~big_integer() {
  _data.clear();
}

big_integer& big_integer::operator=(big_integer const& other) {
  _data = other._data;
  _sgn = other._sgn;
  return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
  *this = *this + rhs;
  return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
  *this = *this - rhs;
  return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
  *this = *this * rhs;
  return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
  *this = *this / rhs;
  return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
  *this = *this % rhs;
  return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
  *this = *this & rhs;
  return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
  *this = *this | rhs;
  return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
  *this = *this ^ rhs;
  return *this;
}

big_integer& big_integer::operator<<=(int rhs) {
  *this = *this << rhs;
  return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
  *this = *this >> rhs;
  return *this;
}

big_integer big_integer::operator+() const {
  return *this;
}

big_integer big_integer::operator-() const {
  if (is_zero()) return *this;
  big_integer res(*this);
  res._sgn ^= true;
  return res;
}

big_integer big_integer::operator~() const {
  return -*this - 1;
}

big_integer& big_integer::operator++() {
  return operator+=(1);
}

big_integer big_integer::operator++(int) {
  big_integer res(*this);
  operator++();
  return res;
}

big_integer& big_integer::operator--() {
  return operator-=(1);
}

big_integer big_integer::operator--(int) {
  big_integer res(*this);
  operator--();
  return res;
}

// todo
big_integer operator+(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator-(big_integer a, big_integer const& b) {
  return a + (-b);
}

// todo
big_integer operator*(big_integer a, big_integer const& b) {
  return a;
}

// todo
big_integer operator/(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator%(big_integer a, big_integer const& b) {
  return a - (a / b) * b;
}

big_integer operator&(big_integer a, big_integer const& b) {
  std::vector<uint32_t> data(std::max<size_t>(a.length(), b.length()));
  for (size_t i = 0; i < data.size(); i++) data[i] = a.get(i) & b.get(i);
  return big_integer(data, a.sgn() & b.sgn());
}

big_integer operator|(big_integer a, big_integer const& b) {
  std::vector<uint32_t> data(std::max<size_t>(a.length(), b.length()));
  for (size_t i = 0; i < data.size(); i++) data[i] = a.get(i) | b.get(i);
  return big_integer(data, a.sgn() | b.sgn());
}

big_integer operator^(big_integer a, big_integer const& b) {
  std::vector<uint32_t> data(std::max<size_t>(a.length(), b.length()));
  for (size_t i = 0; i < data.size(); i++) data[i] = a.get(i) ^ b.get(i);
  return big_integer(data, a.sgn() ^ b.sgn());
}

big_integer operator<<(big_integer a, int b) {
  return a * (1 << b);
}

big_integer operator>>(big_integer a, int b) {
  big_integer res = a / (1 << b);
  if (a < 0) res--;
  return res;
}

bool operator==(big_integer const& a, big_integer const& b) {
  return a._sgn == b._sgn && a._data == b._data;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
  if (a._sgn != b._sgn) return a._sgn;
  if (a.length() < b.length()) return !a._sgn;
  if (a.length() > b.length()) return a._sgn;
  for (size_t i = a.length() - 1; i >= 0; i--) {
    if (a._data[i] > b._data[i]) return a._sgn;
    else if (a._data[i] < b._data[i]) return !a._sgn;
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

// todo
std::string to_string(big_integer const& a) {
  return "";
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  return s << to_string(a);
}

size_t big_integer::length() const {
  return _data.size();
}

bool big_integer::is_zero() const {
  return length() == 1 && _data[0] == 0;
}

big_integer big_integer::abs() const {
  return _sgn ? -*this : *this;
}

char big_integer::last_digit() const {
  return (char) (_data[0] % 10) + '0';
}

bool big_integer::sgn() const {
  return _sgn;
}

uint32_t big_integer::get(size_t ind) const {
  return _data[ind];
}

