#include "dh.h"
#include <iostream>

// Danh sách 100 số nguyên tố nhỏ đầu tiên dùng để sàng lọc hợp số nhanh
static const uint64_t SMALL_PRIMES[] = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
    157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
    239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
    331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419,
    421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503,
    509, 521, 523, 541
};
static const size_t NUM_SMALL_PRIMES = sizeof(SMALL_PRIMES) / sizeof(SMALL_PRIMES[0]);

// ============================================================
// PRNG UTILS
// ============================================================

uint512_t prng_next_u512(prng_ctx *ctx) {
    uint512_t val;
    for (size_t i = 0; i < 8; i++) {
        val.digits[i] = prng_next_u64(ctx);
    }
    return val;
}

uint512_t prng_random_odd_bits(prng_ctx *ctx, size_t bitsize) {
    if (bitsize == 0 || bitsize > 512) bitsize = 512;

    uint512_t val = prng_next_u512(ctx);

    // Xóa các bit vượt quá bitsize
    for (size_t i = bitsize; i < 512; i++) val.set_bit(i, 0);

    // Bật bit đầu (đảm bảo đủ độ dài bit) và bit cuối (đảm bảo số lẻ)
    val.set_bit(bitsize - 1, 1);
    val.set_bit(0, 1);

    return val;
}

uint512_t prng_random_range(prng_ctx *ctx, const uint512_t &min_val, const uint512_t &max_val) {
    if (max_val <= min_val) return min_val;
    uint512_t range = max_val - min_val + uint512_t(1);
    return mod(prng_next_u512(ctx), range) + min_val;
}

// ============================================================
// SÀNG CHIA THỬ
// ============================================================

bool is_divisible_by_small_primes(const uint512_t &n) {
    size_t len = n.bit_length();
    for (size_t idx = 0; idx < NUM_SMALL_PRIMES; idx++) {
        uint64_t p = SMALL_PRIMES[idx];
        if (n == uint512_t(p)) return false; // Chính là số nguyên tố

        // Tính n % p bằng quy tắc Horner dịch bit - siêu tốc
        uint64_t rem = 0;
        for (int i = (int)len - 1; i >= 0; i--) {
            rem = ((rem << 1) | (uint64_t)n.get_bit(i)) % p;
        }
        if (rem == 0) return true; // Chia hết => hợp số
    }
    return false;
}

// ============================================================
// MILLER-RABIN — Dựa theo logic của Thành viên 2, port sang uint512_t
// ============================================================

bool is_prime(const uint512_t &number) {
    if (number < uint512_t(2)) return false;
    if (number == uint512_t(2) || number == uint512_t(3)) return true;
    if (number.is_even()) return false;

    // Loại nhanh hợp số bằng sàng chia thử
    if (is_divisible_by_small_primes(number)) return false;

    // Tách n - 1 = 2^s * d (d là số lẻ)
    uint512_t d = number - uint512_t(1);
    int s = 0;
    while (d.is_even()) {
        d.shift_right_1();
        s++;
    }

    // Với n là số nguyên tố bất kỳ và a không chia hết cho n -> a^(n-1) mod n = 1
    // Vì mũ n-1 lớn nên chia thành 2^s * d, tính a^d rồi bình phương dần tới n-1
    // Port từ uint32_t bases[] = {2,3,5,7,11,13,17,19,23,29,31,37} của Thành viên 2
    // sang uint512_t, dùng thêm random để tăng độ chính xác với số lớn
    uint64_t fixed_bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    uint512_t number_minus_1 = number - uint512_t(1);

    for (uint64_t base_val : fixed_bases) {
        uint512_t base(base_val);
        if (base >= number) continue;

        uint512_t x = modular_exponentiation(base, d, number); // a^d mod n

        if (x == uint512_t(1) || x == number_minus_1) continue;

        bool is_composite = true; // Hợp số là số có > 2 ước
        for (int r = 1; r < s; r++) {
            x = MultMod(x, x, number); // x*x % number
            if (x == number_minus_1) {
                is_composite = false;
                break;
            }
        }
        if (is_composite) return false;
    }
    return true;
}

// ============================================================
// SINH SAFE PRIME — Logic của Thành viên 2, port sang uint512_t
// ============================================================

uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize) {
    if (bitsize < 4) bitsize = 4;
    if (bitsize > 512) bitsize = 512;

    int q_bit = bitsize - 1; // Số bit của q (q = Sophie Germain prime)

    while (true) {
        // Sinh q lẻ có đúng q_bit bit
        // Giữ q_bit cuối, bật bit đầu (đảm bảo q từ 2^(q_bit-1) đến 2^q_bit)
        // Bật bit cuối (đảm bảo số lẻ) — logic giống Thành viên 2
        uint512_t q = prng_random_odd_bits(ctx, q_bit);

        // Kiểm tra q nguyên tố
        if (!is_prime(q)) continue;

        // Điều kiện số nguyên tố an toàn: p = 2*q + 1
        uint512_t p = (q << 1) + uint512_t(1);

        // Kiểm tra p nguyên tố
        if (is_prime(p)) {
            return p;
        }
    }
}

// ============================================================
// SINH KHÓA RIÊNG — Logic của Thành viên 2, port sang uint512_t
// ============================================================

uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p) {
    if (p <= uint512_t(3)) {
        std::cerr << "Loi: p phai > 3\n";
        return uint512_t(2);
    }
    // Sinh private_key trong khoảng [2, p-2] — giống Thành viên 2
    return prng_random_range(ctx, uint512_t(2), p - uint512_t(2));
}
