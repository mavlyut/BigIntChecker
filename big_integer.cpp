#include "big_integer.h"
#include <cstddef>
#include <cstring>
#include <ostream>
#include <stdexcept>

big_integer::big_integer() = default;

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(int a) {}

big_integer::big_integer(std::string const& str) {}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) {
  this->_data = move(other._data);
  this->sgn = other.sgn;
  return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
  return *this;
}

big_integer& big_integer::operator<<=(int rhs) {
  return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
  return *this;
}

big_integer big_integer::operator+() const {
  return *this;
}

big_integer big_integer::operator-() const {
  return *this;
}

big_integer big_integer::operator~() const {
  return -(*this) - 1;
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
  return a.sgn == b.sgn && a._data == b._data;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return a.sgn != b.sgn || a._data != b._data;
}

bool operator<(big_integer const& a, big_integer const& b) {
  if (a.sgn != b.sgn) return a.sgn < b.sgn;
  if (a._data.size() != b._data.size()) return a._data.size() < b._data.size();
  for (ptrdiff_t i = a._data.size() - 1; i >= 0; i--) {
    if (a._data[i] != b._data[i]) return a._data[i] < b._data[i];
  }
  return false;
}

bool operator>(big_integer const& a, big_integer const& b) {
  if (a.sgn != b.sgn) return a.sgn > b.sgn;
  if (a._data.size() != b._data.size()) return a._data.size() > b._data.size();
  for (ptrdiff_t i = a._data.size() - 1; i >= 0; i--) {
    if (a._data[i] != b._data[i]) return a._data[i] > b._data[i];
  }
  return false;
}

bool operator<=(big_integer const& a, big_integer const& b) {
  return a < b || a == b;
}

bool operator>=(big_integer const& a, big_integer const& b) {
  return a > b || a == b;
}

std::string to_string(big_integer const& a) {
  return "";
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  return s << to_string(a);
}
