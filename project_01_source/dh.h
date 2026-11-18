#ifndef DH_H
#define DH_H

#include <cstdint>
#include <cstddef>
#include "u512.h"
#include "modular_math.h"
#include "prng.h"

// Sinh số ngẫu nhiên uint512_t từ bộ sinh PRNG SHAKE-128
uint512_t prng_next_u512(prng_ctx *ctx);

// Sinh số ngẫu nhiên lẻ có đúng bitsize bit (bật MSB và LSB)
uint512_t prng_random_odd_bits(prng_ctx *ctx, size_t bitsize);

// Sinh số ngẫu nhiên trong khoảng [min_val, max_val]
uint512_t prng_random_range(prng_ctx *ctx, const uint512_t &min_val, const uint512_t &max_val);

// Tầng 1: Lọc nhanh - trả về true nếu number chia hết cho số nguyên tố nhỏ nào đó
// (tức là chắc chắn là hợp số). Chỉ thử 15 số đầu để tránh chi phí số lớn quá cao.
bool is_divisible_by_small_primes(const uint512_t &number);

// Tầng 2: Kiểm tra số nguyên tố Miller-Rabin với cơ sở {2} + 40 cơ sở ngẫu nhiên.
// Tích hợp 2 tầng: Trial Division (15 số nhỏ) + Miller-Rabin (41 cơ sở).
// Xác suất sai <= (1/4)^41 ≈ 2×10^-25.
bool is_prime(prng_ctx *ctx, const uint512_t &number);

// Sinh số nguyên tố an toàn p = 2q + 1 (trong đó cả p và q đều là số nguyên tố)
uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize = 512);

// Sinh khóa riêng ngẫu nhiên trong khoảng [2, p - 2]
uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p);

#endif // DH_H
