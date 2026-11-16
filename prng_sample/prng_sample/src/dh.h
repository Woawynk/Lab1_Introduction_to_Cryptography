#ifndef DH1_H
#define DH1_H

#include <stdint.h>
#include "prng.h"

bool is_prime(uint32_t number);

uint32_t modular_exponentiation(uint32_t base, uint32_t exp, const uint32_t &mod);
uint32_t generate_safe_prime(prng_ctx *ctx, size_t bitsize = 32);
uint32_t generate_private_key(prng_ctx *ctx, uint32_t p);

#endif
