#include "dh.h"
#include <iostream>

// ============================================================
// PRNG UTILS (SHAKE-128)
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
    for (size_t i = bitsize; i < 512; i++) {
        val.set_bit(i, 0);
    }

    // Bật bit cao nhất (đảm bảo đủ độ dài bitsize) và bit 0 (đảm bảo là số lẻ)
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
// TẦNG 1: LỌC NHANH BẰNG THỬ CHIA SỐ NGUYÊN TỐ NHỎ
// ============================================================

// Chỉ dùng 15 số nguyên tố đầu tiên: đủ loại ~75% hợp số lẻ ngẫu nhiên,
// chi phí tối thiểu (15 phép mod số lớn thay vì 100+).
// Lý do KHÔNG dùng nhiều hơn: với số 512-bit, mỗi lần mod() phải duyệt 512
// bước dịch bit (long division theo bit) => 100 số nguyên tố = 51.200 thao
// tác số lớn, chậm hơn 1 lần Miller-Rabin và chỉ bổ sung thêm ~3% lợi ích.
static const uint32_t SMALL_PRIMES[] = {
    3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53
};
static const size_t NUM_SMALL_PRIMES = sizeof(SMALL_PRIMES) / sizeof(SMALL_PRIMES[0]);

// Trả về true nếu number chia hết cho bất kỳ số nguyên tố nhỏ nào
// (tức là chắc chắn là hợp số), false nếu vượt qua bộ lọc.
bool is_divisible_by_small_primes(const uint512_t &number) {
    for (size_t i = 0; i < NUM_SMALL_PRIMES; i++) {
        uint512_t small_p(SMALL_PRIMES[i]);
        if (number == small_p) return false;       // Chính nó là số nguyên tố nhỏ
        if (mod(number, small_p) == uint512_t(0))  // Chia hết => hợp số
            return true;
    }
    return false;
}

// ============================================================
// TẦNG 2: KIỂM TRA NGUYÊN TỐ MILLER-RABIN
// ============================================================

// Chiến lược 2 tầng (Two-Phase Primality Testing):
//   Tầng 1 - Trial Division (is_divisible_by_small_primes):
//     Lọc nhanh ~75% hợp số lẻ ngẫu nhiên chỉ với 15 phép mod đơn giản.
//     Chi phí thấp, hiệu quả cao cho số được sinh ngẫu nhiên.
//   Tầng 2 - Miller-Rabin với cơ sở {2} + 40 cơ sở ngẫu nhiên từ [2, n-2]:
//     - Cơ sở cố định a=2 luôn là cơ sở đầu tiên: loại >99.9% hợp số ngay
//       vòng đầu, không tốn chi phí PRNG.
//     - 40 cơ sở ngẫu nhiên từ PRNG (SHAKE-128): đảm bảo không có số nào
//       "may mắn" qua tất cả cơ sở cố định. Xác suất sai <= (1/4)^40 ≈ 8e-25.
bool is_prime(prng_ctx *ctx, const uint512_t &number) {
    if (number < uint512_t(2)) return false;
    if (number == uint512_t(2) || number == uint512_t(3)) return true;
    if (number.is_even()) return false;

    // --- Tầng 1: Lọc nhanh bằng thử chia số nguyên tố nhỏ ---
    // Loại ngay ~75% hợp số lẻ trước khi tốn chi phí Miller-Rabin
    if (is_divisible_by_small_primes(number)) return false;

    // --- Tầng 2: Miller-Rabin ---
    // Tách n - 1 = 2^s * d (với d là số lẻ)
    uint512_t d = number - uint512_t(1);
    int s = 0;
    while (d.is_even()) {
        d.shift_right_1();
        s++;
    }

    uint512_t number_minus_1 = number - uint512_t(1);
    uint512_t number_minus_3 = number - uint512_t(3); // để sinh a trong [2, n-2]

    // Xây dựng danh sách cơ sở: {2} + 40 cơ sở ngẫu nhiên từ [2, n-2]
    // Tổng 41 cơ sở => xác suất sai <= (1/4)^41 ≈ 2×10^-25 (an toàn tuyệt đối)
    const int NUM_RANDOM_BASES = 40;
    uint512_t all_bases[1 + NUM_RANDOM_BASES];
    all_bases[0] = uint512_t(2); // cơ sở cố định đầu tiên
    for (int i = 0; i < NUM_RANDOM_BASES; i++) {
        // Sinh a ngẫu nhiên trong [0, n-4], rồi cộng 2 => a trong [2, n-2]
        all_bases[1 + i] = mod(prng_next_u512(ctx), number_minus_3) + uint512_t(2);
    }

    for (int idx = 0; idx < 1 + NUM_RANDOM_BASES; idx++) {
        uint512_t base = all_bases[idx];
        if (base >= number) continue;

        // Tính x = a^d mod n (Square-and-Multiply)
        uint512_t x = modular_exponentiation(base, d, number);

        // a^d ≡ 1 (mod n) hoặc a^d ≡ n-1 (mod n) => vượt qua với cơ sở này
        if (x == uint512_t(1) || x == number_minus_1) {
            continue;
        }

        // Bình phương liên tiếp s-1 lần: tìm x^(2^r) ≡ n-1 (mod n)
        bool is_composite = true;
        for (int r = 1; r < s; r++) {
            x = MultMod(x, x, number);
            if (x == number_minus_1) {
                is_composite = false;
                break;
            }
        }

        // Không bao giờ đạt n-1 => chắc chắn là hợp số (theo định lý Miller)
        if (is_composite) {
            return false;
        }
    }

    return true; // Vượt qua cả 2 tầng => là số nguyên tố
}

// ============================================================
// SINH SAFE PRIME: p = 2q + 1
// ============================================================

uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize) {
    if (bitsize < 4) bitsize = 4;
    if (bitsize > 512) bitsize = 512;

    int q_bit = bitsize - 1; // Số bit của số nguyên tố Sophie Germain q

    while (true) {
        // 1. Sinh số lẻ ngẫu nhiên q có đúng q_bit bit
        uint512_t q = prng_random_odd_bits(ctx, q_bit);

        // 2. Kiểm tra q có phải số nguyên tố bằng Miller-Rabin không
        if (!is_prime(ctx, q)) {
            continue;
        }

        // 3. Tính p = 2*q + 1
        uint512_t p = (q << 1) + uint512_t(1);

        // 4. Kiểm tra p có phải số nguyên tố bằng Miller-Rabin không
        if (is_prime(ctx, p)) {
            return p; // Tìm thấy Safe Prime hợp lệ!
        }
    }
}

// ============================================================
// SINH KHÓA RIÊNG TRONG KHOẢNG [2, p - 2]
// ============================================================

uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p) {
    if (p <= uint512_t(3)) {
        std::cerr << "Loi: p phai > 3 de sinh khoa rieng!\n";
        return uint512_t(2);
    }
    // Sinh khóa riêng trong khoảng [2, p - 2]
    return prng_random_range(ctx, uint512_t(2), p - uint512_t(2));
}
