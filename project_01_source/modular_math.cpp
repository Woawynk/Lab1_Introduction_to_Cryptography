#include "modular_math.h"

uint512_t mod(const uint512_t &x, const uint512_t &y) {
    if (y.is_zero()) {
        std::cerr << "Loi: Chia cho 0 trong ham mod!\n";
        return uint512_t();
    }
    if (x < y) return x;
    if (x == y) return uint512_t();

    uint512_t rem;
    size_t x_len = x.bit_length();

    // Duyệt từ bit cao nhất của x xuống bit 0 (theo quy tắc Horner từ mã giả)
    for (int i = (int)x_len - 1; i >= 0; i--) {
        uint64_t xi = (uint64_t)x.get_bit(i);
        uint64_t carry = rem.shift_left_1(xi);
        if (carry > 0 || rem >= y) {
            rem = rem - y;
        }
    }
    return rem;
}

uint512_t AddMod(const uint512_t &x, const uint512_t &y, const uint512_t &p) {
    uint512_t a = (x >= p) ? mod(x, p) : x;
    uint512_t b = (y >= p) ? mod(y, p) : y;

    // Theo mã giả AddMod trong tài liệu:
    // if x >= p - y: return x - (p - y)
    // return x + y
    uint512_t diff = p - b;
    if (a >= diff) {
        return a - diff;
    }
    return a + b;
}

uint512_t MultMod(const uint512_t &x, const uint512_t &y, const uint512_t &p) {
    uint512_t ret(0);
    uint512_t cur_x = (x >= p) ? mod(x, p) : x;

    // Theo mã giả MultMod trong tài liệu:
    // for i = 0 to n - 1:
    //   if y_i == 1: ret = AddMod(ret, x, p)
    //   x = AddMod(x, x, p)
    size_t y_len = y.bit_length();
    for (size_t i = 0; i < y_len; i++) {
        if (y.get_bit(i) == 1) {
            ret = AddMod(ret, cur_x, p);
        }
        cur_x = AddMod(cur_x, cur_x, p);
    }
    return ret;
}

uint512_t modular_exponentiation(const uint512_t &base, const uint512_t &exponent, const uint512_t &p) {
    if (p.is_zero()) {
        std::cerr << "Loi: Modulo bang 0 trong modular_exponentiation!\n";
        return uint512_t();
    }
    if (p.is_one()) {
        return uint512_t(); // bat ky so nao mod 1 deu bang 0
    }
    if (exponent.is_zero()) {
        return uint512_t(1); // a^0 mod p = 1 voi p > 1
    }

    // Theo mã giả ExpMod trong tài liệu:
    // ret = 1
    // base = x % p
    // for i = 0 to n - 1:
    //   if y_i == 1: ret = MultMod(ret, base, p)
    //   base = MultMod(base, base, p)
    uint512_t ret(1);
    uint512_t cur_base = (base >= p) ? mod(base, p) : base;

    size_t exp_len = exponent.bit_length();
    for (size_t i = 0; i < exp_len; i++) {
        if (exponent.get_bit(i) == 1) {
            ret = MultMod(ret, cur_base, p);
        }
        cur_base = MultMod(cur_base, cur_base, p);
    }
    return ret;
}
