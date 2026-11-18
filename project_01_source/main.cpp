#include <iostream>
#include <chrono>
#include <cstring>
#include "u512.h"
#include "modular_math.h"
#include "dh.h"
#include "prng.h"

int main() {
    std::cout << "===============================================================\n";
    std::cout << "  CHUONG TRINH TRAO DOI KHOA DIFFIE-HELLMAN (SAFE PRIME 512-BIT)\n";
    std::cout << "  Mon hoc: Nhap mon Ma hoa Mat ma - Bo mon Cong nghe Tri thuc   \n";
    std::cout << "===============================================================\n\n";

    // Khởi tạo bộ sinh ngẫu nhiên chuẩn mật mã SHAKE128
    prng_ctx ctx;
    uint8_t seed[32];
    // Thu thập seed ngẫu nhiên từ hệ điều hành
    randombytes(seed, sizeof(seed));
    prng_seed(&ctx, seed, sizeof(seed));

    // 1. Sinh số nguyên tố an toàn p (Safe Prime) và phần tử sinh g
    int bit_size = 512;
    std::cout << "[1] Dang sinh so nguyen to an toan p (" << bit_size << " bit) bang thuat toan Miller-Rabin...\n";
    std::cout << "    (Qua trinh nay co the mat vai giay, vui long doi)...\n";

    auto start_time = std::chrono::high_resolution_clock::now();
    uint512_t p = generate_safe_prime(&ctx, bit_size);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "    -> Sinh thanh cong so nguyen to an toan p trong " << elapsed.count() << " giay!\n";
    std::cout << "    p (Hex) = " << p.to_hex_string() << "\n\n";

    // 2. Sử dụng phần tử sinh cố định g = 2 theo yêu cầu đề bài.
    // generate_safe_prime chỉ trả về p mà tại đó 2 là primitive root.
    uint512_t g(2);
    std::cout << "[2] Phan tu sinh cong khai g = " << g.to_hex_string() << "\n";
    std::cout << "    g = 2 da duoc kiem tra la primitive root cua Z_p*\n\n";

    // 2. Sinh khóa riêng ngẫu nhiên cho Alice và Bob trong khoảng [2, p - 2]
    std::cout << "[3] Dang sinh khoa rieng cho Alice va Bob...\n";
    uint512_t a = generate_private_key(&ctx, p); // Khóa riêng của Alice
    uint512_t b = generate_private_key(&ctx, p); // Khóa riêng của Bob

    std::cout << "    Khoa rieng cua Alice (a) = " << a.to_hex_string() << "\n";
    std::cout << "    Khoa rieng cua Bob   (b) = " << b.to_hex_string() << "\n\n";

    // 3. Tính giá trị công khai của Alice và Bob
    std::cout << "[4] Dang tinh toan gia tri cong khai...\n";
    uint512_t A = modular_exponentiation(g, a, p); // Alice tính A = g^a % p
    uint512_t B = modular_exponentiation(g, b, p); // Bob tính B = g^b % p

    std::cout << "    Gia tri cong khai cua Alice (A = g^a mod p) = " << A.to_hex_string() << "\n";
    std::cout << "    Gia tri cong khai cua Bob   (B = g^b mod p) = " << B.to_hex_string() << "\n\n";

    // 4. Trao đổi khóa và tính bí mật chung
    std::cout << "[5] Alice va Bob trao doi gia tri cong khai va tinh bi mat chung...\n";
    uint512_t alice_shared_secret = modular_exponentiation(B, a, p); // Alice tính s = B^a % p
    uint512_t bob_shared_secret   = modular_exponentiation(A, b, p); // Bob tính s = A^b % p

    std::cout << "    Bi mat chung Alice tinh duoc: " << alice_shared_secret.to_hex_string() << "\n";
    std::cout << "    Bi mat chung Bob tinh duoc  : " << bob_shared_secret.to_hex_string() << "\n\n";

    // 5. Xác minh bí mật chung trùng khớp
    std::cout << "===============================================================\n";
    bool is_matched = (alice_shared_secret == bob_shared_secret);
    std::cout << "  KET QUA XAC MINH:\n";
    std::cout << "  Qua trinh tinh toan dung khong? " << (is_matched ? "1 (DUNG - BI MAT TRUNG KHOP 100%)" : "0 (SAI)") << "\n";
    std::cout << "===============================================================\n";

    return is_matched ? 0 : 1;
}
