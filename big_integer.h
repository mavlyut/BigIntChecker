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

  void feel(uint64_t a);
  size_t length() const;
  uint32_t operator[](size_t ind) const;
  big_integer& make_bool_op(const std::function<uint32_t(uint32_t, uint32_t)>& f, big_integer const& rhs);
  void reserve(size_t new_capacity);
  void shrink_to_fit();
  uint32_t _default() const;

  static uint32_t trial(big_integer const& a, big_integer const& b, size_t i, size_t j);
  static bool smaller(big_integer const& a, big_integer const& b, size_t pos, size_t m);
  big_integer& difference(big_integer const& rhs, size_t offset, size_t m);
  big_integer& mul(uint32_t rhs);
  big_integer& div(uint32_t rhs);
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

