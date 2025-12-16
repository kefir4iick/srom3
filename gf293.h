#ifndef GF293_H
#define GF293_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define GF_M 293
#define GF_WORDS ((GF_M + 31) / 32)

typedef struct {
    uint32_t words[GF_WORDS];
} GF293;


GF293 gf_zero(void);
GF293 gf_one(void);

bool gf_is_zero(const GF293* a);
bool gf_is_one(const GF293* a);
void gf_set_bit(GF293* a, int pos);
int  gf_degree(const GF293* a);

GF293 gf_add(const GF293* a, const GF293* b);
GF293 gf_mul(const GF293* a, const GF293* b);
GF293 gf_square(const GF293* a);

GF293 gf_pow_u293(const GF293* a, const GF293* exp);

GF293  gf_inverse(const GF293* a);
uint8_t gf_trace(const GF293* a);

bool gf_to_bitstr(const GF293* a, char* out, size_t out_len);
bool gf_from_bitstr(const char* in, GF293* out);

bool gf_from_hex(const char* hex, GF293* out);
bool gf_to_hex(const GF293* a, char* out, size_t out_len);

void gf_print_poly(const GF293* a);

#endif
