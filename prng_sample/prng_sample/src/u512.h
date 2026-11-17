#ifndef U512_H
#define U512_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#if defined(__SIZEOF_INT128__)
#ifndef uint128_t
#define uint128_t __uint128_t
#endif
#endif

/**
 * @brief Cấu trúc biểu diễn số nguyên không dấu 512-bit (uint512_t).
 * Gồm mảng 8 phần tử uint64_t theo thứ tự Little-endian:
 * digits[0]: 64 bit thấp nhất (LSB).
 * digits[7]: 64 bit cao nhất (MSB).
 */
struct uint512_t {
    uint64_t digits[8];

    // Constructors
    uint512_t();
    uint512_t(uint64_t val);
    uint512_t(const std::string &hex_str);

    // Truy xuất và thao tác trên bit
    int get_bit(size_t i) const;
    void set_bit(size_t i, int val = 1);
    size_t bit_length() const;

    // Kiểm tra tính chất số
    bool is_zero() const;
    bool is_one() const;
    bool is_even() const;
    bool is_odd() const;

    // Phép dịch bit 1 vị trí (dành riêng cho các giải thuật mô-đun)
    // shift_left_1 trả về bit 512 bị đẩy ra ngoài (carry: 0 hoặc 1)
    uint64_t shift_left_1(uint64_t carry_in = 0);
    // shift_right_1 trả về bit 0 bị đẩy ra ngoài
    uint64_t shift_right_1();

    // Hiển thị dạng Hex
    std::string to_hex_string() const;
    void print_hex(const char* label = nullptr) const;
};

// Các toán tử so sánh
bool operator==(const uint512_t &a, const uint512_t &b);
bool operator!=(const uint512_t &a, const uint512_t &b);
bool operator<(const uint512_t &a, const uint512_t &b);
bool operator>(const uint512_t &a, const uint512_t &b);
bool operator<=(const uint512_t &a, const uint512_t &b);
bool operator>=(const uint512_t &a, const uint512_t &b);

// Các toán tử số học cơ bản
uint512_t operator+(const uint512_t &a, const uint512_t &b);
uint512_t operator-(const uint512_t &a, const uint512_t &b);
uint512_t operator<<(const uint512_t &a, size_t shift);
uint512_t operator>>(const uint512_t &a, size_t shift);

std::ostream& operator<<(std::ostream &os, const uint512_t &a);

#endif // U512_H
