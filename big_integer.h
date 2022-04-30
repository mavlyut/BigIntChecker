#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>
#include <functional>


const uint32_t max_int32 = ((1ll << 32) - 1);
const uint32_t log_int32 = 32;
const uint64_t base = (uint64_t) max_int32 + 1ll;
const uint32_t base_mod_ten = (uint32_t) (base % 10);


class big_integer
{
private:
    
    std::vector<uint32_t> data;
    
    short signum;
    
    void negate();
    void mul(uint32_t val);
    void add(uint32_t val);
    uint32_t div(uint32_t val);
    uint32_t div10();
    uint32_t mod(uint32_t val);
    void correct_size(size_t);
    void add(big_integer const&);
    void subtract(big_integer const&);
    void mul_max_int32();
    void div_max_int32();
    void invert();
    void to_add_code();
    void to_simple_code();
    
    uint32_t& operator[](size_t);
    const uint32_t& operator[](size_t) const;
    size_t size() const;
    uint32_t& back();
    const uint32_t& back() const;
    void pop_back();
    void push_back(uint32_t);
    bool empty() const;
    
public:
    
    big_integer();
    big_integer(int);
    big_integer(uint32_t);
    big_integer(uint64_t);
    explicit big_integer(const std::string&);
    big_integer(std::vector<uint32_t>&, short);
    big_integer(big_integer const&);
    
    std::string to_string() const;
    std::vector<uint32_t> get_digits() const;
    
    bool is_deg2() const;
    
    big_integer& operator = (const big_integer&);
    
    friend bool operator == (big_integer const&, big_integer const&);
    friend bool operator != (big_integer const&, big_integer const&);
    friend bool operator < (big_integer const&, big_integer const&);
    friend bool operator <= (big_integer const&, big_integer const&);
    friend bool operator > (big_integer const&, big_integer const&);
    friend bool operator >= (big_integer const&, big_integer const&);
    
    friend std::ostream& operator<<(std::ostream&, big_integer const&);
    friend std::istream& operator>>(std::istream&, big_integer const&);
    
    friend big_integer operator-(big_integer const&);
    friend big_integer operator+(big_integer const&);
    
    friend big_integer operator+(big_integer const&, big_integer const&);
    friend big_integer operator-(big_integer const&, big_integer const&);
    friend big_integer operator*(big_integer const&, big_integer const&);
    friend big_integer operator/(big_integer const&, big_integer const&);
    friend big_integer operator%(big_integer const&, big_integer const&);
    
    
    friend big_integer& operator+=(big_integer&, big_integer const&);
    friend big_integer& operator-=(big_integer&, big_integer const&);
    friend big_integer& operator/=(big_integer&, big_integer const&);
    friend big_integer& operator*=(big_integer&, big_integer const&);
    friend big_integer& operator%=(big_integer&, big_integer const&);
    friend big_integer& operator^=(big_integer&, big_integer const&);
    friend big_integer& operator|=(big_integer&, big_integer const&);
    friend big_integer& operator&=(big_integer&, big_integer const&);
    friend big_integer& operator<<=(big_integer&, int);
    friend big_integer& operator>>=(big_integer&, int);
    
    
    static big_integer any_binary(big_integer const&, big_integer const&, std::function<uint32_t(uint32_t, uint32_t)>);
    friend big_integer operator^(big_integer const&, big_integer const&);
    friend big_integer operator|(big_integer const&, big_integer const&);
    friend big_integer operator&(big_integer const&, big_integer const&);
    friend big_integer operator<<(big_integer const&, int);
    friend big_integer operator>>(big_integer const&, int);
    friend big_integer operator~(big_integer const&);
    friend bool operator!(big_integer const&);
    
    big_integer pow(int);
    big_integer absolute() const;
    
    inline static big_integer base_deg(size_t n);
    
    ~big_integer();
    
};

std::string to_string(big_integer const& b);
