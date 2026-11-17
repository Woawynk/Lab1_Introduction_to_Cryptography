#ifndef U512_H
#define U512_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#ifndef uint128_t
#define uint128_t __uint128_t
#endif

using namespace std;

struct uint512_t {
    uint64_t digits[8];

    uint512_t();
    uint512_t(uint64_t val);
    uint512_t(const std::string &hex_str);

    int get_bit(size_t i) const;
    void set_bit(size_t i, int val = 1);
    size_t bit_length() const;

    bool is_zero() const;
    bool is_one() const;
    bool is_even() const;
    bool is_odd() const;

    uint64_t shift_left_1(uint64_t carry_in = 0);
    uint64_t shift_right_1();

    std::string to_hex_string() const;
    void print_hex(const char* label = nullptr) const;
};

bool operator==(const uint512_t &a, const uint512_t &b);
bool operator!=(const uint512_t &a, const uint512_t &b);
bool operator<(const uint512_t &a, const uint512_t &b);
bool operator>(const uint512_t &a, const uint512_t &b);
bool operator<=(const uint512_t &a, const uint512_t &b);
bool operator>=(const uint512_t &a, const uint512_t &b);

uint512_t operator+(const uint512_t &a, const uint512_t &b);
uint512_t operator-(const uint512_t &a, const uint512_t &b);
uint512_t operator<<(const uint512_t &a, size_t shift);
uint512_t operator>>(const uint512_t &a, size_t shift);

std::ostream& operator<<(std::ostream &os, const uint512_t &a);

#endif // U512_H
