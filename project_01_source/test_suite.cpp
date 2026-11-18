#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdint>
#include "u512.h"
#include "modular_math.h"
#include "dh.h"
#include "prng.h"

void test_basic_arithmetic() {
    std::cout << "[TEST 1] Kiem tra phep toan co ban (+, -, <, ==, shift)...\n";
    
    uint512_t a(100);
    uint512_t b(40);
    
    // So sánh
    assert(b < a);
    assert(a > b);
    assert(a != b);
    assert(a == uint512_t(100));

    // Cộng trừ
    uint512_t c = a + b;
    assert(c == uint512_t(140));
    
    uint512_t d = a - b;
    assert(d == uint512_t(60));

    // Dịch bit
    uint512_t one(1);
    uint512_t shifted = one << 64;
    assert(shifted.digits[1] == 1 && shifted.digits[0] == 0);
    assert(shifted.bit_length() == 65);

    uint512_t back = shifted >> 64;
    assert(back == one);

    // Chuỗi Hex
    uint512_t hex_val("0x123456789abcdef0");
    assert(hex_val.digits[0] == 0x123456789abcdef0ULL);

    uint512_t max_value("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    assert(max_value + uint512_t(1) == uint512_t(0));
    assert(uint512_t(0) - uint512_t(1) == max_value);

    std::cout << "  -> PASS!\n";
}

void test_modular_operations() {
    std::cout << "[TEST 2] Kiem tra mod, AddMod, MultMod...\n";

    uint512_t p(1000000007); // So nguyen to 10^9 + 7
    uint512_t x(987654321);
    uint512_t y(123456789);

    // Modulo
    uint512_t rem = mod(x, p);
    assert(rem == x);

    uint512_t big_val = p + uint512_t(50);
    assert(mod(big_val, p) == uint512_t(50));

    // AddMod: (987654321 + 123456789) % 1000000007 = 111111103
    uint512_t sum_mod = AddMod(x, y, p);
    std::cout << "  AddMod(x, y, p) = " << sum_mod.digits[0] << "\n";
    assert(sum_mod == uint512_t(111111103));

    // MultMod: (987654321 * 123456789) % 1000000007 = 259106859
    uint512_t mul_mod = MultMod(x, y, p);
    std::cout << "  MultMod(x, y, p) = " << mul_mod.digits[0] << "\n";
    assert(mul_mod == uint512_t(259106859));

    std::cout << "  -> PASS!\n";
}

void test_modular_exponentiation() {
    std::cout << "[TEST 3] Kiem tra modular_exponentiation (Square-and-Multiply)...\n";

    // Test theo Dinh ly Fermat nho: a^(p-1) = 1 (mod p) voi p nguyen to
    uint512_t p(1000000007);
    uint512_t a(3);
    uint512_t exp = p - uint512_t(1); // exp = p - 1

    uint512_t res = modular_exponentiation(a, exp, p);
    std::cout << "  Fermat test: 3^(p-1) mod p = " << res.digits[0] << "\n";
    assert(res == uint512_t(1));

    // Test luy thua nho: 2^10 % 1000 = 1024 % 1000 = 24
    uint512_t res2 = modular_exponentiation(uint512_t(2), uint512_t(10), uint512_t(1000));
    std::cout << "  Small exp test: 2^10 mod 1000 = " << res2.digits[0] << "\n";
    assert(res2 == uint512_t(24));

    // Test luy thua voi so 512-bit
    uint512_t p512("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C605295386E176C");
    uint512_t base512("0x2");
    uint512_t exp512("0x10001"); // 65537

    auto start = std::chrono::high_resolution_clock::now();
    uint512_t res512 = modular_exponentiation(base512, exp512, p512);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "  512-bit exp test: 2^65537 mod p512 = " << res512.to_hex_string() << "\n";
    std::cout << "  Thoi gian thuc thi: " << elapsed.count() << " ms\n";
    assert(!res512.is_zero());

    std::cout << "  -> PASS!\n";
}

void test_primality_and_dh() {
    std::cout << "[TEST 4] Kiem tra Miller-Rabin, g = 2 va DH...\n";

    uint8_t seed[32] = {};
    prng_ctx ctx;
    prng_seed(&ctx, seed, sizeof(seed));

    assert(is_prime(&ctx, uint512_t(101)));
    assert(!is_prime(&ctx, uint512_t(561)));
    assert(is_divisible_by_3_5_7(uint512_t(15)));
    assert(is_divisible_by_3_5_7(uint512_t(25)));
    assert(is_divisible_by_3_5_7(uint512_t(21)));
    assert(is_divisible_by_3_5_7(uint512_t(49)));
    assert(!is_divisible_by_3_5_7(uint512_t(29)));

    uint512_t p = generate_safe_prime(&ctx, 16);
    uint512_t q = (p - uint512_t(1)) >> 1;
    assert(is_prime(&ctx, p));
    assert(is_prime(&ctx, q));

    uint512_t g(2);
    assert(is_generator_two(p, q));
    assert(MultMod(g, g, p) != uint512_t(1));
    assert(modular_exponentiation(g, q, p) == p - uint512_t(1));

    uint512_t a(7);
    uint512_t b(11);
    uint512_t public_a = modular_exponentiation(g, a, p);
    uint512_t public_b = modular_exponentiation(g, b, p);
    assert(modular_exponentiation(public_b, a, p) ==
           modular_exponentiation(public_a, b, p));

    std::cout << "  -> PASS!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "  KIEM THU CAC MO-DUN U512 & MODULAR     \n";
    std::cout << "========================================\n";

    test_basic_arithmetic();
    test_modular_operations();
    test_modular_exponentiation();
    test_primality_and_dh();

    std::cout << "\n>>> TAT CA CAC TEST DA VUOT QUA THANH CONG! <<<\n";
    return 0;
}
