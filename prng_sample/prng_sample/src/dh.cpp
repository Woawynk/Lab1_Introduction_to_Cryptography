#include "dh.h"

bool is_prime(uint32_t number){
    if (number < 2)
        return false;
    if (number == 2 or number == 3)
        return true;
    if ((number & 1) == 0)
        return false;
    uint32_t d = number - 1;    //Tách n - 1 = 2^s x d
    int s = 0;
    while ((d & 1) == 0) {
        d = d >> 1;
        s++;
    }
    //Với n là số nguyên tố bất kì và a không chia hết cho n -> a^(n-1) mod n = 1
    //Vì mũ n-1 lớn nên chia thành 2^s x d. Tính a^d rồi bình phương dần tới n-1
    uint32_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (uint32_t base : bases) {
        if (base % number == 0)
            continue;
        uint32_t x = modular_exponentiation(base, d, number);   //a^d mod n
        if (x == 1 || x == number - 1)
            continue;
        bool is_composite = true;   //Hợp số là số có > 2 ước
        for (int r = 1; r < s; r++) {
            x = modular_exponentiation(x, 2, number); //x*x % number
            if (x == number - 1) {
                is_composite = false;
                break;
            }
        }
        if (is_composite)
            return false;
    }
    return true;
}


uint32_t modular_exponentiation(uint32_t base, uint32_t exp, const uint32_t &mod){
    //Giả sử base = 2, exp = 13, mod = 10. Hàm sẽ tách 2^13 thành 2^12x2 -> (2^2)^6x2
    //Khi số mũ lẻ thì nhân vào result ngay nên còn (4^6) *axb mod n = a mod n x b mod n
    //4^6 chẵn -> (4^2)^3 -> nhân 4^2 vào result rồi tính tiếp (4^2)^2
    //exp cuối luôn là 1 nên result luôn nhân đủ
    uint32_t result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        //Tăng số (bên trong ngoặc)
        base = (uint64_t)(base * base) % mod;
        //Giảm mũ
        exp = exp >> 1;
    }
    return result;
}
uint32_t generate_safe_prime(prng_ctx *ctx, size_t bitsize){
    if (bitsize < 2 || bitsize > 32) {
        throw std::invalid_argument("bitsize must be between 2 and 32");
    }

    int q_bit = bitsize - 1;
    uint32_t mask = (1U << q_bit) - 1;
    while (true) {
        uint32_t q = prng_next_u32(ctx) & mask;    //Giữ q_bit cuối
        q |= (1U << (q_bit - 1));   //Bật bit đầu (Đảm bảo q là số từ q_bit - 1 đến q_bit). VD: 2^6->2^7
        q |= 1U;    //Bật bit cuối (Đảm bảo số lẻ)
        if (!is_prime(q)) {
            continue;
        }
        uint32_t p = 2 * q + 1; //Điều kiện số nto an toàn
        if (is_prime(p)) {
            return p;
        }
    }
}
uint32_t generate_private_key(prng_ctx *ctx, uint32_t p){
    if (p <= 3){
        throw std::invalid_argument("p must > 3")
    }
    uint32_t range = p - 3;
    uint32_t private_key;
    do{
        private_key = 2 + (prng_next_u32(ctx) % range)
    } while (private_key < 2 || private_key > p - 2);
    return private_key;
}