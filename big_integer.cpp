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
  return *this;
}

big_integer& big_integer::operator++() {
  return *this;
}

big_integer big_integer::operator++(int) {
  return *this;
}

big_integer& big_integer::operator--() {
  return *this;
}

big_integer big_integer::operator--(int) {
  return *this;
}

big_integer operator+(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator-(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator*(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator/(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator%(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator&(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator|(big_integer a, big_integer const& b) {
  return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
  return a;
}

big_integer operator<<(big_integer a, int b) {
  return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
  return a;
}

bool operator==(big_integer const& a, big_integer const& b) {
  return true;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return true;
}

bool operator<(big_integer const& a, big_integer const& b) {
  return true;
}

bool operator>(big_integer const& a, big_integer const& b) {
  return true;
}

bool operator<=(big_integer const& a, big_integer const& b) {
  return true;
}

bool operator>=(big_integer const& a, big_integer const& b) {
  return true;
}

std::string to_string(big_integer const& a) {
  return "";
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  return s << to_string(a);
}
