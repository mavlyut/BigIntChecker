#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <vector>

struct big_integer
{
  big_integer();
  big_integer(std::vector<uint32_t> const& data, bool const& sign);
  big_integer(big_integer const& other);
  big_integer(int a);
  explicit big_integer(std::string const& str);
  big_integer& operator=(big_integer const& other);
  ~big_integer();

  big_integer abs() const;
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
  big_integer& operator--();

  friend bool operator==(big_integer const& a, big_integer const& b);
  friend bool operator!=(big_integer const& a, big_integer const& b);
  friend bool operator<(big_integer const& a, big_integer const& b);
  friend bool operator>(big_integer const& a, big_integer const& b);
  friend bool operator<=(big_integer const& a, big_integer const& b);
  friend bool operator>=(big_integer const& a, big_integer const& b);

  friend big_integer operator+(big_integer const& a, big_integer const& b);
  friend big_integer operator-(big_integer const& a, big_integer const& b);
  friend big_integer operator*(big_integer const& a, big_integer const& b);
  friend big_integer operator/(big_integer const& a, big_integer const& b);
  friend big_integer operator%(big_integer const& a, big_integer const& b);

  friend big_integer operator&(big_integer const& a, big_integer const& b);
  friend big_integer operator|(big_integer const& a, big_integer const& b);
  friend big_integer operator^(big_integer const& a, big_integer const& b);

  friend big_integer operator<<(big_integer const& a, int b);
  friend big_integer operator>>(big_integer const& a, int b);

  friend std::string to_string(big_integer const& a);
  friend std::ostream& operator<<(std::ostream& s, big_integer const& a);
private:
  std::vector<uint32_t> data;
  bool sign;
  size_t len() const;
  uint32_t value(size_t id) const;
  void to_size(size_t nlen = 0);
};

big_integer binpow(int k);

#endif

