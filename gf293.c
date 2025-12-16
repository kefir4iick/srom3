#include "gf293.h"
#include <stdio.h>
#include <string.h>



static inline void gf_xtime(GF293* a) {
    uint32_t carry =
        (a->words[(GF_M - 1) / 32] >> ((GF_M - 1) % 32)) & 1u;

    uint32_t prev = 0;
    for (int i = 0; i < GF_WORDS; i++) {
        uint32_t cur = a->words[i];
        a->words[i] = (cur << 1) | prev;
        prev = cur >> 31;
    }

    int last_bits = GF_M % 32;
    if (last_bits) {
        uint32_t mask = (1u << last_bits) - 1u;
        a->words[GF_WORDS - 1] &= mask;
    }

    if (carry) {
        a->words[0] ^= (1u << 0);
        a->words[0] ^= (1u << 1);
        a->words[0] ^= (1u << 6);
        a->words[0] ^= (1u << 11);
    }
}




GF293 gf_zero(void) {
    GF293 z = {0};
    return z;
}

GF293 gf_one(void) {
    GF293 r = {0};
    r.words[0] = 1u;
    return r;
}




bool gf_is_zero(const GF293* a) {
    for (int i = 0; i < GF_WORDS; i++)
        if (a->words[i]) return false;
    return true;
}

bool gf_is_one(const GF293* a) {
    if (a->words[0] != 1u) return false;
    for (int i = 1; i < GF_WORDS; i++)
        if (a->words[i]) return false;
    return true;
}




void gf_set_bit(GF293* a, int pos) {
    if (pos < 0 || pos >= GF_M) return;
    a->words[pos / 32] |= (1u << (pos % 32));
}

static inline uint32_t gf_get_bit(const GF293* a, int pos) {
    return (a->words[pos / 32] >> (pos % 32)) & 1u;
}

int gf_degree(const GF293* a) {
    for (int i = GF_WORDS - 1; i >= 0; i--) {
        uint32_t w = a->words[i];
        if (!w) continue;
        for (int j = 31; j >= 0; j--) {
            if ((w >> j) & 1u)
                return i * 32 + j;
        }
    }
    return -1;
}




GF293 gf_add(const GF293* a, const GF293* b) {
    GF293 r;
    for (int i = 0; i < GF_WORDS; i++)
        r.words[i] = a->words[i] ^ b->words[i];
    return r;
}

GF293 gf_mul(const GF293* a, const GF293* b) {
    GF293 res = gf_zero();
    GF293 acc = *a;

    for (int i = 0; i < GF_M; i++) {
        if (gf_get_bit(b, i))
            res = gf_add(&res, &acc);
        gf_xtime(&acc);
    }
    return res;
}

GF293 gf_square(const GF293* a) {
    return gf_mul(a, a);
}




GF293 gf_pow_u293(const GF293* a, const GF293* exp) {
    if (!a || !exp) return gf_zero();

    GF293 r = gf_one();

    for (int i = GF_M - 1; i >= 0; --i) {
        r = gf_square(&r);
        if (gf_get_bit(exp, i))
            r = gf_mul(&r, a);
    }
    return r;
}




GF293 gf_inverse(const GF293* a) {
    if (!a || gf_is_zero(a)) return gf_zero();

    GF293 t = *a;
    GF293 inv = gf_one();

    for (int i = 1; i < GF_M; i++) {
        t = gf_square(&t);
        inv = gf_mul(&inv, &t);
    }
    return inv;
}

uint8_t gf_trace(const GF293* a) {
    GF293 t = *a;
    GF293 s = *a;

    for (int i = 1; i < GF_M; i++) {
        t = gf_square(&t);
        s = gf_add(&s, &t);
    }
    return (uint8_t)(s.words[0] & 1u);
}




bool gf_to_bitstr(const GF293* a, char* out, size_t out_len) {
    if (!a || !out) return false;
    if (out_len < GF_M + 1) return false;

    for (int i = 0; i < GF_M; i++) {
        int deg = (GF_M - 1) - i;
        uint32_t bit = gf_get_bit(a, deg);
        out[i] = bit ? '1' : '0';
    }
    out[GF_M] = '\0';
    return true;
}

bool gf_from_bitstr(const char* in, GF293* out) {
    if (!in || !out) return false;

    for (int i = 0; i < GF_M; i++)
        if (in[i] != '0' && in[i] != '1')
            return false;
    if (in[GF_M] != '\0') return false;

    GF293 r = gf_zero();

    for (int i = 0; i < GF_M; i++) {
        if (in[i] == '1') {
            int deg = (GF_M - 1) - i;
            r.words[deg / 32] ^= (1u << (deg % 32));
        }
    }

    *out = r;
    return true;
}

void gf_print_poly(const GF293* a) {
    bool first = true;
    for (int i = GF_M - 1; i >= 0; i--) {
        if (gf_get_bit(a, i)) {
            if (!first) printf(" + ");
            if (i == 0) printf("1");
            else if (i == 1) printf("x");
            else printf("x^%d", i);
            first = false;
        }
    }
    if (first) printf("0");
}



static int hex_val(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool gf_from_hex(const char* hex, GF293* out) {
    if (!hex || !out) return false;

    size_t len = strlen(hex);
    const size_t hex_len = (GF_M + 3) / 4;
    const size_t hex_bits = hex_len * 4;
    const size_t pad = hex_bits - GF_M;

    if (len == 0 || len > hex_len) return false;

    char full[75];
    size_t z = hex_len - len;
    for (size_t i = 0; i < z; i++) full[i] = '0';
    memcpy(full + z, hex, len);
    full[hex_len] = '\0';

    GF293 r = gf_zero();

    for (size_t i = 0; i < hex_len; i++) {
        int v = hex_val(full[i]);
        if (v < 0) return false;

        for (int o = 0; o < 4; o++) {
            size_t global = i * 4 + (size_t)o;
            if (global < pad) continue;

            size_t idx = global - pad;
            int deg = (GF_M - 1) - (int)idx;

            if ((v >> (3 - o)) & 1)
                r.words[deg / 32] ^= (1u << (deg % 32));
        }
    }

    *out = r;
    return true;
}

bool gf_to_hex(const GF293* a, char* out, size_t out_len) {
    if (!a || !out) return false;

    const size_t hex_len = (GF_M + 3) / 4;
    const size_t hex_bits = hex_len * 4;
    const size_t pad = hex_bits - GF_M;

    if (out_len < hex_len + 1) return false;

    static const char* H = "0123456789ABCDEF";
    char tmp[75];

    for (size_t i = 0; i < hex_len; i++) {
        int v = 0;
        for (int o = 0; o < 4; o++) {
            size_t global = i * 4 + (size_t)o;
            uint32_t bit = 0;

            if (global >= pad) {
                size_t idx = global - pad;
                int deg = (GF_M - 1) - (int)idx;
                bit = (a->words[deg / 32] >> (deg % 32)) & 1u;
            }
            v |= bit << (3 - o);
        }
        tmp[i] = H[v];
    }
    tmp[hex_len] = '\0';

    size_t start = 0;
    while (start + 1 < hex_len && tmp[start] == '0')
        start++;

    size_t n = hex_len - start;
    memcpy(out, tmp + start, n);
    out[n] = '\0';
    return true;
}
