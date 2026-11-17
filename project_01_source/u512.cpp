#include "u512.h"

uint512_t::uint512_t() {
    for (size_t i = 0; i < 8; i++) {
        digits[i] = 0;
    }
}

uint512_t::uint512_t(uint64_t val) {
    digits[0] = val;
    for (size_t i = 1; i < 8; i++) {
        digits[i] = 0;
    }
}

uint512_t::uint512_t(const std::string &hex_str) {
    for (size_t i = 0; i < 8; i++) {
        digits[i] = 0;
    }

    std::string s = hex_str;
    if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) {
        s = s.substr(2);
    }

    if (s.empty()) return;

    int digit_idx = 0;
    int len = (int)s.length();
    for (int i = len; i > 0; i -= 16) {
        if (digit_idx >= 8) break;
        int start = std::max(0, i - 16);
        int chunk_len = i - start;
        std::string chunk = s.substr(start, chunk_len);
        digits[digit_idx++] = std::stoull(chunk, nullptr, 16);
    }
}

int uint512_t::get_bit(size_t i) const {
    if (i >= 512) return 0;
    size_t word = i / 64;
    size_t bit = i % 64;
    return (int)((digits[word] >> bit) & 1ULL);
}

void uint512_t::set_bit(size_t i, int val) {
    if (i >= 512) return;
    size_t word = i / 64;
    size_t bit = i % 64;
    if (val) {
        digits[word] |= (1ULL << bit);
    } else {
        digits[word] &= ~(1ULL << bit);
    }
}

size_t uint512_t::bit_length() const {
    for (int i = 7; i >= 0; i--) {
        if (digits[i] != 0) {
            uint64_t w = digits[i];
            size_t bits = (i + 1) * 64;
            while ((w & (1ULL << 63)) == 0) {
                w <<= 1;
                bits--;
            }
            return bits;
        }
    }
    return 0;
}

bool uint512_t::is_zero() const {
    for (size_t i = 0; i < 8; i++) {
        if (digits[i] != 0) return false;
    }
    return true;
}

bool uint512_t::is_one() const {
    if (digits[0] != 1) return false;
    for (size_t i = 1; i < 8; i++) {
        if (digits[i] != 0) return false;
    }
    return true;
}

bool uint512_t::is_even() const {
    return (digits[0] & 1ULL) == 0;
}

bool uint512_t::is_odd() const {
    return (digits[0] & 1ULL) == 1;
}

uint64_t uint512_t::shift_left_1(uint64_t carry_in) {
    uint64_t carry = carry_in & 1ULL;
    for (size_t i = 0; i < 8; i++) {
        uint64_t next_carry = (digits[i] >> 63) & 1ULL;
        digits[i] = (digits[i] << 1) | carry;
        carry = next_carry;
    }
    return carry;
}

uint64_t uint512_t::shift_right_1() {
    uint64_t carry = 0;
    for (int i = 7; i >= 0; i--) {
        uint64_t next_carry = (digits[i] & 1ULL) << 63;
        digits[i] = (digits[i] >> 1) | carry;
        carry = next_carry;
    }
    return (carry >> 63);
}

std::string uint512_t::to_hex_string() const {
    if (is_zero()) return "0x0";

    std::stringstream ss;
    ss << "0x";
    int start_word = 7;
    while (start_word >= 0 && digits[start_word] == 0) {
        start_word--;
    }

    ss << std::hex << digits[start_word];
    for (int i = start_word - 1; i >= 0; i--) {
        ss << std::setfill('0') << std::setw(16) << std::hex << digits[i];
    }
    return ss.str();
}

void uint512_t::print_hex(const char* label) const {
    if (label) {
        std::cout << label << ": ";
    }
    std::cout << to_hex_string() << "\n";
}

bool operator==(const uint512_t &a, const uint512_t &b) {
    for (size_t i = 0; i < 8; i++) {
        if (a.digits[i] != b.digits[i]) return false;
    }
    return true;
}

bool operator!=(const uint512_t &a, const uint512_t &b) {
    return !(a == b);
}

bool operator<(const uint512_t &a, const uint512_t &b) {
    for (int i = 7; i >= 0; i--) {
        if (a.digits[i] < b.digits[i]) return true;
        if (a.digits[i] > b.digits[i]) return false;
    }
    return false;
}

bool operator>(const uint512_t &a, const uint512_t &b) {
    return b < a;
}

bool operator<=(const uint512_t &a, const uint512_t &b) {
    return !(b < a);
}

bool operator>=(const uint512_t &a, const uint512_t &b) {
    return !(a < b);
}

uint512_t operator+(const uint512_t &a, const uint512_t &b) {
    uint512_t res;
#if defined(__SIZEOF_INT128__)
    uint128_t carry = 0;
    for (size_t i = 0; i < 8; i++) {
        uint128_t sum = (uint128_t)a.digits[i] + b.digits[i] + carry;
        res.digits[i] = (uint64_t)sum;
        carry = sum >> 64;
    }
#else
    uint64_t carry = 0;
    for (size_t i = 0; i < 8; i++) {
        uint64_t u1 = a.digits[i];
        uint64_t u2 = b.digits[i];
        uint64_t sum = u1 + u2 + carry;
        carry = (sum < u1) || (carry && (sum == u1));
        res.digits[i] = sum;
    }
#endif
    return res;
}

uint512_t operator-(const uint512_t &a, const uint512_t &b) {
    uint512_t res;
#if defined(__SIZEOF_INT128__)
    uint128_t borrow = 0;
    for (size_t i = 0; i < 8; i++) {
        uint128_t a_val = a.digits[i];
        uint128_t b_val = (uint128_t)b.digits[i] + borrow;
        if (a_val >= b_val) {
            res.digits[i] = (uint64_t)(a_val - b_val);
            borrow = 0;
        } else {
            res.digits[i] = (uint64_t)(((uint128_t)1 << 64) + a_val - b_val);
            borrow = 1;
        }
    }
#else
    uint64_t borrow = 0;
    for (size_t i = 0; i < 8; i++) {
        uint64_t u1 = a.digits[i];
        uint64_t u2 = b.digits[i];
        uint64_t diff = u1 - u2 - borrow;
        borrow = (u1 < u2) || (borrow && (u1 == u2));
        res.digits[i] = diff;
    }
#endif
    return res;
}

uint512_t operator<<(const uint512_t &a, size_t shift) {
    if (shift == 0) return a;
    if (shift >= 512) return uint512_t();

    uint512_t res;
    size_t word_shift = shift / 64;
    size_t bit_shift = shift % 64;

    for (size_t i = 0; i < 8; i++) {
        if (i >= word_shift) {
            size_t src = i - word_shift;
            res.digits[i] |= (a.digits[src] << bit_shift);
            if (bit_shift > 0 && src > 0) {
                res.digits[i] |= (a.digits[src - 1] >> (64 - bit_shift));
            }
        }
    }
    return res;
}

uint512_t operator>>(const uint512_t &a, size_t shift) {
    if (shift == 0) return a;
    if (shift >= 512) return uint512_t();

    uint512_t res;
    size_t word_shift = shift / 64;
    size_t bit_shift = shift % 64;

    for (size_t i = 0; i < 8; i++) {
        if (i + word_shift < 8) {
            size_t src = i + word_shift;
            res.digits[i] |= (a.digits[src] >> bit_shift);
            if (bit_shift > 0 && src + 1 < 8) {
                res.digits[i] |= (a.digits[src + 1] << (64 - bit_shift));
            }
        }
    }
    return res;
}

std::ostream& operator<<(std::ostream &os, const uint512_t &a) {
    os << a.to_hex_string();
    return os;
}
