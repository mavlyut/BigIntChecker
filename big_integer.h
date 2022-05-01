#pragma once

#include <iosfwd>
#include <string>
#include <vector>
#include <functional>

struct big_integer {
  big_integer();
  big_integer(big_integer const& other);
  explicit big_integer(std::string const& str);

  big_integer(int a);
  big_integer(unsigned int a);
  big_integer(long a);
  big_integer(unsigned long a);
  big_integer(long long a);
  big_integer(unsigned long long a);
  big_integer(std::vector<uint32_t> data, bool sgn);

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

private:
  std::vector<uint32_t> _data;
  bool _sgn;
  static big_integer ZERO;

  size_t length() const;
  void swap(big_integer& other);
  uint32_t operator[](size_t ind) const;
  big_integer& make_bool_op(const std::function<uint32_t(uint32_t, uint32_t)>& f, big_integer const& rhs);
  void reserve(size_t new_capacity);
  big_integer abs() const;
  void shrink_to_fit();
};

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
