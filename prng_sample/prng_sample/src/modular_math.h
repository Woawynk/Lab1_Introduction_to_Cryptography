#ifndef MODULAR_MATH_H
#define MODULAR_MATH_H

#include "u512.h"

/**
 * @brief Tính x % y bằng thuật toán dịch bit theo mã giả tài liệu.
 * Đảm bảo các biến trung gian luôn nằm trong phạm vi 512-bit.
 */
uint512_t mod(const uint512_t &x, const uint512_t &y);

/**
 * @brief Tính (x + y) % p không tràn số.
 */
uint512_t AddMod(const uint512_t &x, const uint512_t &y, const uint512_t &p);

/**
 * @brief Tính (x * y) % p bằng thuật toán Double-and-Add theo mã giả tài liệu.
 */
uint512_t MultMod(const uint512_t &x, const uint512_t &y, const uint512_t &p);

/**
 * @brief Tính (base^exponent) % p bằng thuật toán Square-and-Multiply theo mã giả tài liệu.
 * Đây chính là hàm modular_exponentiation yêu cầu trong Lab 01.
 */
uint512_t modular_exponentiation(const uint512_t &base, const uint512_t &exponent, const uint512_t &p);

#endif // MODULAR_MATH_H
