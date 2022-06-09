#include "big_integer.h"
#include <stdexcept>
#include <algorithm>

typedef unsigned long long ull;

void big_integer::to_size(size_t NEEDED_LENGTH) {
	while (data.size() > NEEDED_LENGTH && sign && data.back() == UINT32_MAX) data.pop_back();
	while (data.size() > NEEDED_LENGTH && !sign && data.back() == 0) data.pop_back();
	while (data.size() < NEEDED_LENGTH) data.push_back(this->value(data.size()));
}

big_integer::big_integer() : sign(false) {}

big_integer::big_integer(uint_vector const& _data, bool const& _sign) : data(_data), sign(_sign) {
    to_size();
}

big_integer::big_integer(int o) {
	data.push_back(uint(o));
	sign = (o < 0);
    to_size();
}

size_t big_integer::len() const {
	return this->data.size();
}

big_integer::big_integer(big_integer const& o) : data(o.data), sign(o.sign) {}

big_integer::big_integer(std::string const& s) {
    bool ok = true;
    for (size_t i = 1; i < s.size(); i++) ok |= ('0' <= s[i] && s[i] <= '9');
    ok |= (('0' <= s[0] && s[0] <= '9') || (s[0] == '-'));
    if (!ok) throw std::runtime_error("Incorrect string format");
    big_integer res;
    for (char ch : s) {
		if (ch == '-') {
			continue;
		} else {
			res = res * 10 + (ch - '0');
		}
	}
    if (s[0] == '-') {
		res = -res;
	}
    sign = (s[0] == '-');
	(*this) = res;
    to_size();
}

uint big_integer::value(size_t id) const {
	if (id < data.size()) {
		return data[id];
	}
	if (sign) return UINT32_MAX; else return 0;
}

big_integer::~big_integer() {}

big_integer& big_integer::operator=(big_integer const& o) {
	big_integer res(o);
	std::swap(res.sign, this->sign);
	std::swap(res.data, this->data);
	return (*this);
}

big_integer big_integer::abs() const {
	return (sign) ? -(*this) : (*this);
}

big_integer big_integer::operator - () const {
	if ((*this) == 0) return (*this);
	return (~(*this)) + 1;
}

big_integer big_integer::operator + () const {
	return (*this);
}

big_integer big_integer::operator ~ () const {
    uint_vector res;
	for (size_t i = 0; i < data.size(); i++) res.push_back(~data[i]);
	return big_integer(res, !sign);
}

big_integer& big_integer::operator ++ () {
	return (*this) += 1;
}

big_integer& big_integer::operator--() {
	return (*this) -= 1;
}

big_integer operator + (big_integer const& a, big_integer const& b) {
	uint carry = 0;
    uint_vector res(std::max(a.len(), b.len()) + 1);//extra qword
	for (size_t i = 0; i < res.size(); i++) {
		ull cur = ull(a.value(i)) + b.value(i) + carry;
		carry = cur >> 32;
		res[i] = uint(cur); //default casting with %= 2^32
	}
	return big_integer(res, (res.back() & (1 << 31)));
}

big_integer operator - (big_integer const& a, big_integer const& b)  {
	return (a + (-b));
}

big_integer operator * (big_integer const& a, big_integer const& b) {
	if ((a.data.empty() && !a.sign) || (b.data.empty() && !b.sign)) return big_integer(0);
	big_integer p = a.abs(), q = b.abs();
	size_t sz = p.len() + q.len() + 1;
    uint_vector res(sz);
	for (size_t i = 0; i < p.len(); i++) {
		uint carry = 0;
		for (size_t j = 0; j < q.len() || carry; j++) {
			size_t at = i + j;
			ull cur = ull(p.value(i)) * q.value(j) + res[at] + carry;
			res[at] = uint(cur); //default casting with %= 2^32
			carry = cur >> 32;
		}
	}
	big_integer ans = big_integer(res, 0);
	if (a.sign ^ b.sign) ans = -ans;
	return ans;
}

uint trial(uint a, uint b, uint c) {
	ull res = a;
	res <<= 32;
	res += b;
	res /= c;
	res = std::min(res, ull(UINT32_MAX));
	return uint(res);
}

big_integer operator / (big_integer const& a, big_integer const& b) {
	big_integer p = a.abs(), q = b.abs();
    if (q == 0) throw std::runtime_error("Division by zero");
	if (p.len() < q.len()) return 0;
	uint f = (ull(UINT32_MAX) + 1) / (ull(q.data.back()) + 1);
	big_integer bf = f;
	bf = bf.abs();
	p *= bf;
	q *= bf;
	size_t n = p.len(), m = q.len();
	size_t sz = n - m + 1;
    uint_vector res(sz);
	uint divisor = q.data.back();
    uint_vector divident_pref(m + 1);
	for (size_t i = 0; i <= m; i++) divident_pref[i] = p.value(n - m + i);
	for (size_t i = 0; i < sz; i++) {
		divident_pref[0] = p.value(n - m - i);
		//raw data multiplication
        big_integer cur = trial(divident_pref[m], divident_pref[m - 1], divisor);
		cur.sign = false;
        cur.to_size(1);
        big_integer divideable = q * cur;
        divideable.to_size(m + 1);
        big_integer divident = big_integer(divident_pref, 0);
        divident.to_size(m + 1);

		while (cur > 0 && divident < divideable) {
            --cur;
            cur.to_size(1);
			cur.sign = false;
            divideable = q * cur;
            divideable.to_size(m + 1);
        }

		divident -= divideable;
        divident.to_size(m + 1);
		for (size_t j = 0; j <= m; j++) divident_pref[j] = divident.value(j);
		for (size_t j = m; j > 0; j--) divident_pref[j] = divident_pref[j - 1];
		res[sz - i - 1] = cur.data[0];
	}
	big_integer ans(res, 0);
	if (a.sign ^ b.sign) ans = -ans;
	return ans;
}

big_integer operator % (big_integer const& a, big_integer const& b) {
	return a - (a / b) * b;
}

big_integer binpow(int k) {
	if (!k) return big_integer(1);
	if (k % 2) return binpow(k - 1) * 2; else {
		big_integer cur = binpow(k / 2);
		return cur * cur;
	}
}

big_integer operator >> (big_integer const& a, int b) {
	big_integer res = a / binpow(b);
	if (a < 0) --res;
	return res;
}

big_integer operator << (big_integer const& a, int b) {
	return a * binpow(b);
}

big_integer operator & (big_integer const& a, big_integer const& b) {
    uint_vector res(std::max(a.len(), b.len()));
	for (size_t i = 0; i < res.size(); i++) res[i] = a.value(i) & b.value(i);
	return big_integer(res, a.sign & b.sign);

}

big_integer operator | (big_integer const& a, big_integer const& b) {
    uint_vector res(std::max(a.len(), b.len()));
	for (size_t i = 0; i < res.size(); i++) res[i] = a.value(i) | b.value(i);
	return big_integer(res, a.sign | b.sign);

}

big_integer operator ^ (big_integer const& a, big_integer const& b) {
    uint_vector res(std::max(a.len(), b.len()));
	for (size_t i = 0; i < res.size(); i++) res[i] = a.value(i) ^ b.value(i);
	return big_integer(res, a.sign ^ b.sign);
}

big_integer& big_integer::operator += (big_integer const& rhs) {
	(*this) = (*this) + rhs;
	return (*this);
}
big_integer& big_integer::operator -= (big_integer const& rhs) {
	(*this) = (*this) - rhs;
	return (*this);
}
big_integer& big_integer::operator *= (big_integer const& rhs) {
	(*this) = (*this) * rhs;
	return (*this);
}
big_integer& big_integer::operator /= (big_integer const& rhs) {
	(*this) = (*this) / rhs;
	return (*this);
}
big_integer& big_integer::operator %= (big_integer const& rhs) {
	(*this) = (*this) % rhs;
	return (*this);
}
big_integer& big_integer::operator &= (big_integer const& rhs) {
	(*this) = (*this) & rhs;
	return (*this);
}
big_integer& big_integer::operator |= (big_integer const& rhs) {
	(*this) = (*this) | rhs;
	return (*this);
}
big_integer& big_integer::operator ^= (big_integer const& rhs) {
	(*this) = (*this) ^ rhs;
	return (*this);
}

bool operator == (big_integer const& p, big_integer const& q) {
	big_integer a = p, b = q;
	if (a.sign != b.sign) return false;
	if (a.len() != b.len()) return false;
	for (size_t i = 0; i < a.len(); i++) if (a.data[i] != b.data[i]) return false;
	return true;
}
bool operator != (big_integer const& a, big_integer const& b) {
	return !(a == b);
}
bool operator < (big_integer const& p, big_integer const& q) {
	big_integer a = p, b = q;
	if (a.sign != b.sign) return a.sign;
	if (a.len() < b.len()) return !a.sign;
	if (a.len() > b.len()) return a.sign;
	for (size_t i = a.len(); i > 0; i--) {
		if (a.data[i - 1] < b.data[i - 1]) return !a.sign;
		if (a.data[i - 1] > b.data[i - 1]) return a.sign;
	}
	return false;
}
bool operator > (big_integer const& a, big_integer const& b) {
	return (b < a);
}
bool operator <= (big_integer const& a, big_integer const& b) {
	return !(a > b);
}
bool operator >= (big_integer const& a, big_integer const& b) {
	return !(a < b);
}

big_integer& big_integer::operator <<= (int rhs) {
	(*this) = (*this) << rhs;
	return (*this);
}
big_integer& big_integer::operator >>= (int rhs) {
	(*this) = (*this) >> rhs;
	return (*this);
}

std::string to_string(big_integer const & a)
{
	std::string res = "";
	if (a.data.empty()) {
		return (a.sign) ? "-1" : "0";
	}
	big_integer ac = a.abs();
	while (!ac.data.empty()) {
		big_integer rem = ac % 10;
		res += char(rem.value(0) + '0');
		ac = ac / 10;
	}
	if (a.sign) res.push_back('-');
	reverse(res.begin(), res.end());
	return res;
}

std::ostream & operator<<(std::ostream & out, big_integer const & a)
{
	return (out << to_string(a));
}
/*
#include <iostream>

int main() {
	big_integer a({ 4, 0 }, 1);
	big_integer b = a.abs();
	assert(b >= 0);
	return 0;
}
*/
