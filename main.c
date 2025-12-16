#include <stdio.h>
#include <string.h>
#include "gf293.h"

int main(void) {
    GF293 A, B, R;
    char hex[75];

    const char* A_hex =
        "6F8C2A91D3B7E4F05A9C1E7D4B20F8A63C9D5712E0A4B6F93D8C2E51A7B4096D3C1F8";

    const char* B_hex =
        "1A4E9C7D0F3B6A258E41C9D52F7086AB3C4D91E7F8A20B65C3D94E2A1F6B0875C9";

    if (!gf_from_hex(A_hex, &A) || !gf_from_hex(B_hex, &B)) {
        printf("error hex\n");
        return 1;
    }

    printf("A (hex)  = %s\n", A_hex);
    printf("A (poly) = ");
    gf_print_poly(&A);
    printf("\n\n");

    printf("B (hex)  = %s\n", B_hex);
    printf("B (poly) = ");
    gf_print_poly(&B);
    printf("\n\n");


    R = gf_add(&A, &B);
    gf_to_hex(&R, hex, sizeof(hex));
    printf("A + B = %s\n\n", hex);

    R = gf_mul(&A, &B);
    gf_to_hex(&R, hex, sizeof(hex));
    printf("A * B = %s\n\n", hex);

    R = gf_square(&A);
    gf_to_hex(&R, hex, sizeof(hex));
    printf("A^2 = %s\n\n", hex);

    R = gf_pow_u293(&A, &B);
    gf_to_hex(&R, hex, sizeof(hex));
    printf("A^B = %s\n\n", hex);


    GF293 invA = gf_inverse(&A);
    GF293 check = gf_mul(&A, &invA);

    printf("inv(A) = %s\n", hex);

    printf("A * inv(A) = %s\n\n", hex);


    printf("Tr(A) = %u\n\n", gf_trace(&A));


    gf_to_hex(&A, hex, sizeof(hex));
    printf("A -> HEX -> A = %s\n\n", hex);



    return 0;
}
