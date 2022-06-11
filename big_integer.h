#pragma once

#include <iosfwd>
#include <string>
#include <vector>
#include <functional>

struct big_integer {
  big_integer();
  big_integer(std::vector<uint32_t> data, bool sgn);
  big_integer(big_integer const& other);
  big_integer(int a);
  explicit big_integer(std::string const& str);
  ~big_integer();

  big_integer& operator=(big_integer const& other);

  big_integer& operator+=(big_integer const& rhs);
  big_integer& operator-=(big_integer const& rhs);
  big_integer& operator*=(big_integer const& rhs);
  big_integer& operator/=(big_integer const& rhs);
  big_integer& operator%=(big_integer const& rhs);

  big_integer& operator&=(big_integer const& rhs);
  big_integer& operator|=(big_integer const& rhs);
  big_integer& operator^=(big_integer const& rhs);

  big_integer& operator<<=(int rhs);
  big_integer& operator>>=(int rhs);

  big_integer operator+() const;
  big_integer operator-() const;
  big_integer operator~() const;

  big_integer& operator++();
  big_integer operator++(int);

  big_integer& operator--();
  big_integer operator--(int);

  friend bool operator==(big_integer const& a, big_integer const& b);
  friend bool operator!=(big_integer const& a, big_integer const& b);
  friend bool operator<(big_integer const& a, big_integer const& b);
  friend bool operator>(big_integer const& a, big_integer const& b);
  friend bool operator<=(big_integer const& a, big_integer const& b);
  friend bool operator>=(big_integer const& a, big_integer const& b);

  friend std::string to_string(big_integer const& a);

  friend big_integer operator+(big_integer a, big_integer const& b);
  friend big_integer operator/(big_integer a, big_integer const& b);
  friend big_integer operator&(big_integer a, big_integer const& b);
  friend big_integer operator|(big_integer a, big_integer const& b);
  friend big_integer operator^(big_integer a, big_integer const& b);

  friend big_integer operator<<(big_integer a, int b);
  friend big_integer operator>>(big_integer a, int b);

  size_t size() const;
  bool eq_zero() const;
  big_integer abs() const;
  void swap(big_integer& other);
  std::vector<uint32_t> mul_uint32_t(uint32_t x) const;
  std::vector<uint32_t> div_uint32_t(uint32_t x) const;
  uint32_t operator[](size_t ind) const;
  bool sgn() const;
  big_integer norm();

private:
  std::vector<uint32_t> data_;
  bool sgn_;

  void delete_leading_zeroes();

  friend big_integer bit_operation(std::function<uint32_t(uint32_t, uint32_t)> const& f, big_integer const& a, big_integer const& b);

  static const std::function<uint32_t(uint32_t, uint32_t)> bit_and;
  static const std::function<uint32_t(uint32_t, uint32_t)> bit_xor;
  static const std::function<uint32_t(uint32_t, uint32_t)> bit_or;
};

std::vector<uint32_t> mul_uint32_t(std::vector<uint32_t> const&, uint32_t x);

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

