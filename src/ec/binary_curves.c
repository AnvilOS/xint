
#include "binary_curves.h"

#include "xint_internal.h"
#include "xint_algorithms.h"
#include "xint_io.h"
#include "xint_bitwise.h"

static void point_add(xint_ecc_point_t r, const xint_ecc_point_t q, const xint_ecc_point_t p, const xint_ecc_curve_t *c);
static void point_double(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_ecc_curve_t *c);
static void point_add_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t *c);
static void point_double_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c);
static void xint_mod_fast_k163(xword_t *w, xword_t *u);
static void scalar_multiply_mont_x_only(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c);

const xword_t k163_p[]  = { 0 };
const xword_t k163_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_Gx[] = { X(0x5c94eee8, 0xde4e6d5e), X(0xaa07d793, 0x7bbc11ac), X(0xfe13c053, 0x02) };
const xword_t k163_Gy[] = { X(0xccdaa3d9, 0x0536d538), X(0x321f2e80, 0x5d38ff58), X(0x89070fb0, 0x02) };
const xword_t k163_n[]  = { X(0x99f8a5ef, 0xa2e0cc0d), X(0x00020108, 0x00000000), X(0x00000000, 0x04) };
const xword_t k163_h[]  = { 0x02 };
const int k163_exp[] = { 7, 6, 3, 0 };
const xword_t k163_x[]  = { X(0x000000C9, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x08) };
const xint_ecc_curve_t k163 =
{
    163,
    CURVE_WORDS(163),
    k163_p,
    k163_a,
    k163_b,
    k163_Gx,
    k163_Gy,
    k163_n,
    k163_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k163_exp,
    4,
    k163_x,
    scalar_multiply_mont_x_only,
};

const xword_t k233_p[]  = { 0 };
const xword_t k233_a[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x0000) };
const xword_t k233_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x0000) };
const xword_t k233_Gx[] = { X(0xefad6126, 0x0a4c9d6e), X(0x19c26bf5, 0x149563a4), X(0x29f22ff4, 0x7e731af1), X(0x32ba853a, 0x0172) };
const xword_t k233_Gy[] = { X(0x56fae6a3, 0x56e0c110), X(0xf18aeb9b, 0x27a8cd9b), X(0x555a67c4, 0x19b7f70f), X(0x537dece8, 0x01db) };
const xword_t k233_n[]  = { X(0xf173abdf, 0x6efb1ad5), X(0xb915bcd4, 0x00069d5b), X(0x00000000, 0x00000000), X(0x00000000, 0x0080) };
const xword_t k233_h[]  = { 0x04 };
const int k233_exp[] = { 74, 0 };
const xword_t k233_x[]  = { X(0x00000001, 0x00000000), X(0x00000400, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x0200) };
const xint_ecc_curve_t k233 =
{
    233,
    CURVE_WORDS(233),
    k233_p,
    k233_a,
    k233_b,
    k233_Gx,
    k233_Gy,
    k233_n,
    k233_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k233_exp,
    2,
    k233_x,
    scalar_multiply_mont_x_only,
};

const xword_t k283_p[]  = { 0 };
const xword_t k283_a[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
const xword_t k283_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
const xword_t k283_Gx[] = { X(0x58492836, 0xb0c2ac24), X(0x16876913, 0x23c1567a), X(0x53cd265f, 0x62f188e5), X(0x3f1a3b81, 0x78ca4488), 0x503213f };
const xword_t k283_Gy[] = { X(0x77dd2259, 0x4e341161), X(0xe4596236, 0xe8184698), X(0xe87e45c0, 0x07e5426f), X(0x8d90f95d, 0x0f1c9e31), 0x1ccda38 };
const xword_t k283_n[]  = { X(0x1e163c61, 0x94451e06), X(0x265dff7f, 0x2ed07577), X(0xffffe9ae, 0xffffffff), X(0xffffffff, 0xffffffff), 0x1ffffff };
const xword_t k283_h[]  = { 0x04 };
const int k283_exp[] = { 12, 7, 5, 0 };
const xword_t k283_x[]  = { X(0x000010a1, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x8000000 };
const xint_ecc_curve_t k283 =
{
    283,
    CURVE_WORDS(283),
    k283_p,
    k283_a,
    k283_b,
    k283_Gx,
    k283_Gy,
    k283_n,
    k283_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k283_exp,
    4,
    k283_x,
    scalar_multiply_mont_x_only,
};

const xword_t k409_p[]  = { 0 };
const xword_t k409_a[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
const xword_t k409_b[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
const xword_t k409_Gx[] = { X(0xe9023746, 0xb35540cf), X(0xee222eb1, 0xb5aaaa62), X(0xc460189e, 0xf9f67cc2), X(0x27accfb8, 0xe307c84c), X(0x0efd0987, 0x0f718421), X(0xad3ab189, 0x658f49c1), 0x060f05f };
const xword_t k409_Gy[] = { X(0xd8e0286b, 0x5863ec48), X(0xaa9ca27a, 0xe9c55215), X(0xda5f6c42, 0xe9ea10e3), X(0xe6325165, 0x918ea427), X(0x3460782f, 0xbf04299c), X(0xacba1dac, 0x0b7c4e42), 0x1e36905 };
const xword_t k409_n[]  = { X(0xe01e5fcf, 0x4b5c83b8), X(0xe3e7ca5b, 0x557d5ed3), X(0x20400ec4, 0x83b2d4ea), X(0xfffffe5f, 0xffffffff), X(0xffffffff, 0xffffffff), X(0xffffffff, 0xffffffff), 0x07fffff };
const xword_t k409_h[]  = { 0x04 };
const int k409_exp[] = { 87, 0 };
const xword_t k409_x[]  = { X(0x00000001, 0x00000000), X(0x00800000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x2000000 };
const xint_ecc_curve_t k409 =
{
    409,
    CURVE_WORDS(409),
    k409_p,
    k409_a,
    k409_b,
    k409_Gx,
    k409_Gy,
    k409_n,
    k409_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k409_exp,
    2,
    k409_x,
    scalar_multiply_mont_x_only,
};

const xword_t k571_p[]  = { 0 };
const xword_t k571_a[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000) };
const xword_t k571_b[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000) };
const xword_t k571_Gx[] = { X(0xa01c8972, 0xe2945283), X(0x4dca88c7, 0x988b4717), X(0x494776fb, 0xbbd1ba39), X(0xb4ceb08c, 0x47da304d), X(0x93b205e6, 0x43709584), X(0x01841ca4, 0x60248048), X(0x0012d5d4, 0xac9ca297), X(0xf8103fe4, 0x82189631), X(0x59923fbc, 0x26eb7a8) };
const xword_t k571_Gy[] = { X(0x3ef1c7a3, 0x01cd4c14), X(0x591984f6, 0x320430c8), X(0x7ba7af1b, 0xb620b01a), X(0xf772aedc, 0x4fbebbb9), X(0xac44aea7, 0x9d4979c0), X(0x006d8a2c, 0xffc61efc), X(0x9f307a54, 0x4dd58cec), X(0x3bca9531, 0x4f4aeade), X(0x7f4fbf37, 0x349dc80) };
const xword_t k571_n[]  = { X(0x637c1001, 0x5cfe778f), X(0x1e91deb4, 0xe5d63938), X(0xb630d84b, 0x917f4138), X(0xb391a8db, 0xf19a63e4), X(0x131850e1, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x2000000) };
const xword_t k571_h[]  = { 0x04 };
const int k571_exp[] = { 10, 5, 2, 0 };
const xword_t k571_x[]  = { X(0x00000425, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x8000000) };
const xint_ecc_curve_t k571 =
{
    571,
    CURVE_WORDS(571),
    k571_p,
    k571_a,
    k571_b,
    k571_Gx,
    k571_Gy,
    k571_n,
    k571_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k571_exp,
    4,
    k571_x,
    scalar_multiply_mont_x_only,
};

const xword_t b163_p[]  = { 0 };
const xword_t b163_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t b163_b[]  = { X(0x4a3205fd, 0x512f7874), X(0x1481eb10, 0xb8c953ca), X(0x0a601907, 0x02) };
const xword_t b163_Gx[] = { X(0xe8343e36, 0xd4994637), X(0xa0991168, 0x86a2d57e), X(0xf0eba162, 0x03) };
const xword_t b163_Gy[] = { X(0x797324f1, 0xb11c5c0c), X(0xa2cdd545, 0x71a0094f), X(0xd51fbc6c, 0x00) };
const xword_t b163_n[]  = { X(0xa4234c33, 0x77e70c12), X(0x000292fe, 0x00000000), X(0x00000000, 0x04) };
const xword_t b163_h[]  = { 0x02 };
const int b163_exp[] = { 7, 6, 3, 0 };
const xword_t b163_x[]  = { X(0x000000C9, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x08) };
const xint_ecc_curve_t b163 =
{
    163,
    CURVE_WORDS(163),
    b163_p,
    b163_a,
    b163_b,
    b163_Gx,
    b163_Gy,
    b163_n,
    b163_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b163_exp,
    4,
    b163_x,
    scalar_multiply_mont_x_only,
};

const xword_t b233_p[]  = { 0 };
const xword_t b233_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x0000) };
const xword_t b233_b[]  = { X(0x7d8f90ad, 0x81fe115f), X(0x20e9ce42, 0x213b333b), X(0x0923bb58, 0x332c7f8c), X(0x647ede6c, 0x0066) };
const xword_t b233_Gx[] = { X(0x71fd558b, 0xf8f8eb73), X(0x391f8b36, 0x5fef65bc), X(0x39f1bb75, 0x8313bb21), X(0xc9dfcbac, 0x00fa) };
const xword_t b233_Gy[] = { X(0x01f81052, 0x36716f7e), X(0xf867a7ca, 0xbf8a0bef), X(0xe58528be, 0x03350678), X(0x6a08a419, 0x0100) };
const xword_t b233_n[]  = { X(0x03cfe0d7, 0x22031d26), X(0xe72f8a69, 0x0013e974), X(0x00000000, 0x00000000), X(0x00000000, 0x0100) };
const xword_t b233_h[]  = { 0x04 };
const int b233_exp[] = { 74, 0 };
const xword_t b233_x[]  = { X(0x00000001, 0x00000000), X(0x00000400, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x0200) };
const xint_ecc_curve_t b233 =
{
    233,
    CURVE_WORDS(233),
    b233_p,
    b233_a,
    b233_b,
    b233_Gx,
    b233_Gy,
    b233_n,
    b233_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b233_exp,
    2,
    b233_x,
    scalar_multiply_mont_x_only,
};

const xword_t b283_p[]  = { 0 };
const xword_t b283_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
const xword_t b283_b[]  = { X(0x3b79a2f5, 0xf6263e31), X(0xa581485a, 0x45309fa2), X(0xca97fd76, 0x19a0303f), X(0xa5a4af8a, 0xc8b8596d), 0x27b680a };
const xword_t b283_Gx[] = { X(0x86b12053, 0xf8cdbecd), X(0x80e2e198, 0x557eac9c), X(0x2eed25b8, 0x70b0dfec), X(0xe1934f8c, 0x8db7dd90), 0x5f93925 };
const xword_t b283_Gy[] = { X(0xbe8112f4, 0x13f0df45), X(0x826779c8, 0x350eddb0), X(0x516ff702, 0xb20d02b4), X(0xb98fe6d4, 0xfe24141c), 0x3676854 };
const xword_t b283_n[]  = { X(0xefadb307, 0x5b042a7c), X(0x938a9016, 0x399660fc), X(0xffffef90, 0xffffffff), X(0xffffffff, 0xffffffff), 0x3ffffff };
const xword_t b283_h[]  = { 0x04 };
const int b283_exp[] = { 12, 7, 5, 0 };
const xword_t b283_x[]  = { X(0x000010a1, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x8000000 };
const xint_ecc_curve_t b283 =
{
    283,
    CURVE_WORDS(283),
    b283_p,
    b283_a,
    b283_b,
    b283_Gx,
    b283_Gy,
    b283_n,
    b283_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b283_exp,
    4,
    b283_x,
    scalar_multiply_mont_x_only,
};

const xword_t b409_p[]  = { 0 };
const xword_t b409_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
const xword_t b409_b[]  = { X(0x7b13545f, 0x4f50ae31), X(0xd57a55aa, 0x72822f6c), X(0xa9a197b2, 0xd6ac27c8), X(0x4761fa99, 0xf1f3dd67), X(0x7fd6422e, 0x3b7b476b), X(0x5c4b9a75, 0xc8ee9feb), 0x021a5c2 };
const xword_t b409_Gx[] = { X(0xbb7996a7, 0x60794e54), X(0x5603aeab, 0x8a118051), X(0xdc255a86, 0x34e59703), X(0xb01ffe5b, 0xf1771d4d), X(0x441cde4a, 0x64756260), X(0x496b0c60, 0xd088ddb3), 0x15d4860 };
const xword_t b409_Gy[] = { X(0x0273c706, 0x81c364ba), X(0xd2181b36, 0xdf4b4f40), X(0x38514f1f, 0x5488d08f), X(0x0158aa4f, 0xa7bd198d), X(0x7636b9c5, 0x24ed106a), X(0x2bbfa783, 0xab6be5f3), 0x061b1cf };
//0x
const xword_t b409_n[]  = { X(0xd9a21173, 0x8164cd37), X(0x9e052f83, 0x5fa47c3c), X(0xf33307be, 0xaad6a612), X(0x000001e2, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x1000000 };
const xword_t b409_h[]  = { 0x04 };
const int b409_exp[] = { 87, 0 };
const xword_t b409_x[]  = { X(0x00000001, 0x00000000), X(0x00800000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x2000000 };
const xint_ecc_curve_t b409 =
{
    409,
    CURVE_WORDS(409),
    b409_p,
    b409_a,
    b409_b,
    b409_Gx,
    b409_Gy,
    b409_n,
    b409_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b409_exp,
    2,
    b409_x,
    scalar_multiply_mont_x_only,
};

const xword_t b571_p[]  = { 0 };
const xword_t b571_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000) };
const xword_t b571_b[]  = { X(0x2955727a, 0x7ffeff7f), X(0x39baca0c, 0x520e4de7), X(0x78ff12aa, 0x4afd185a), X(0x56a66e29, 0x2be7ad67), X(0x8efa5933, 0x84ffabbd), X(0x4a9a18ad, 0xcd6ba8ce), X(0xcb8ceff1, 0x5c6a97ff), X(0xb7f3d62f, 0xde297117), X(0x2221f295, 0x2f40e7e) };
const xword_t b571_Gx[] = { X(0x8eec2d19, 0xe1e7769c), X(0xc850d927, 0x4abfa3b4), X(0x8614f139, 0x99ae6003), X(0x5b67fb14, 0xcdd711a3), X(0xf4c0d293, 0xbde53950), X(0xdb7b2abd, 0xa5f40fc8), X(0x955fa80a, 0x0a93d1d2), X(0x0d3cd775, 0x6c16c0d4), X(0x34b85629, 0x303001d) };
const xword_t b571_Gy[] = { X(0x1b8ac15b, 0x1a4827af), X(0x6e23dd3c, 0x16e2f151), X(0x0485c19b, 0xb3531d2f), X(0x461bb2a8, 0x6291af8f), X(0xbab08a57, 0x84423e43), X(0x3921e8a6, 0x1980f853), X(0x009cbbca, 0x8c6c27a6), X(0xb73d69d7, 0x6dccfffe), X(0x42da639b, 0x37bf273) };
const xword_t b571_n[]  = { X(0x2fe84e47, 0x8382e9bb), X(0x5174d66e, 0x161de93d), X(0xc7dd9ca1, 0x6823851e), X(0x08059b18, 0xff559873), X(0xe661ce18, 0xffffffff), X(0xffffffff, 0xffffffff), X(0xffffffff, 0xffffffff), X(0xffffffff, 0xffffffff), X(0xffffffff, 0x3ffffff) };
const xword_t b571_h[]  = { 0x04 };
const int b571_exp[] = { 10, 5, 2, 0 };
const xword_t b571_x[]  = { X(0x00000425, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x8000000) };
const xint_ecc_curve_t b571 =
{
    571,
    CURVE_WORDS(571),
    b571_p,
    b571_a,
    b571_b,
    b571_Gx,
    b571_Gy,
    b571_n,
    b571_h,
    point_add,
    point_double,
    xint_mod_fast_k163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b571_exp,
    4,
    b571_x,
    scalar_multiply_mont_x_only,
};

static void field_red(xint_t w, const xint_ecc_curve_t *c);

static void field_add(xint_ptr w, const_xint_ptr u, const_xint_ptr v, const xint_ecc_curve_t *c)
{
    // W = U xor V
    int Un = XINT_ABS(u->size);
    int Vn = XINT_ABS(v->size);
    if (Un < Vn)
    {
        XINT_SWAP(const_xint_ptr, u, v);
        XINT_SWAP(int, Un, Vn);
    }
    FAST_RESIZE(w, Un);
    for (int i=0; i<Vn; ++i)
    {
        w->data[i] = u->data[i] ^ v->data[i];
    }
    if (Un > Vn)
    {
        xll_copy(w->data+Vn, u->data+Vn, Un-Vn);
    }
    trim_zeroes(w);
}

static void field_mul(xint_ptr w, const_xint_ptr u, const_xint_ptr v, const xint_ecc_curve_t *c)
{
    // W = U . V
    int Un = XINT_ABS(u->size);
    int Vn = XINT_ABS(v->size);
    if (Un == 0 || Vn == 0)
    {
        w->size = 0;
        return;
    }

    // We need a copy of v because we will be shifting it
    xint_t v_copy = XINT_INIT_VAL;
    xint_copy(v_copy, v);
    
    // We also need a copy of w because it may be common with u
    xint_t w_copy = XINT_INIT_VAL;
    FAST_RESIZE(w_copy, Un + Vn);
    xll_zero(w_copy->data, Un + Vn);

    for (int k=0; k<XWORD_BITS; ++k)
    {
        for (int j=0; j<Un; ++j)
        {
            if (u->data[j] & (1ULL<<k))
            {
                for (int i=0; i<Vn; ++i)
                {
                    w_copy->data[j+i] ^= v_copy->data[i];
                }
            }
        }
        if (k < XWORD_BITS - 1)
        {
            xint_lshift(v_copy, v_copy, 1);
            Vn = XINT_ABS(v_copy->size);
        }
    }
    trim_zeroes(w_copy);
    xint_copy(w, w_copy);
    xint_delete(v_copy);
    xint_delete(w_copy);
    field_red(w, c);
}

static void field_squ(xint_t w, const xint_t u, const xint_ecc_curve_t *c)
{
    field_mul(w, u, u, c);
}

static void field_red(xint_t w, const xint_ecc_curve_t *c)
{
    const int *exp = c->exp;
    int num_exp = c->num_exp;
    int m = c->nbits;
        
    for (int i = xint_highest_bit_num(w); i>=m; --i)
    {
        if (xint_get_bit(w, i))
        {
            for (int k=0; k<num_exp; ++k)
            {
                xint_flip_bit(w, i - m + exp[k]);
            }
            xint_flip_bit(w, i);
        }
    }
    trim_zeroes(w);
}

static void field_inv(xint_t w, const xint_t a, const xint_ecc_curve_t *cve)
{
    xint_t b = XINT_INIT_VAL;
    xint_t c = XINT_INIT_VAL;
    xint_t u = XINT_INIT_VAL;
    xint_t v = XINT_INIT_VAL;
    xint_t TMP = XINT_INIT_VAL;
    xint_assign_long(b, 1);
    xint_assign_zero(c);
    xint_copy(u, a);
    XINT_FROM_XWORDS(v, cve->x, cve->nwords);
    int degv = xint_highest_bit_num(v);
    int degu;
    while (xint_highest_bit_num(v) >= 0)
    {
        degu = xint_highest_bit_num(u);
        degv = xint_highest_bit_num(v);
        int j = degu - degv;
        if (j < 0)
        {
            xint_swap(u, v);
            xint_swap(b, c);
            j = -j;
        }
        xint_lshift(TMP, v, j);
        field_add(u, u, TMP, cve);
        xint_lshift(TMP, c, j);
        field_add(b, b, TMP, cve);
    }
    xint_copy(w, b);
    xint_delete(b);
    xint_delete(c);
    xint_delete(u);
    xint_delete(v);
    xint_delete(TMP);
}

static void point_add(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_ecc_point_t q, const xint_ecc_curve_t *c)
{
    if (p->is_at_infinity)
    {
        xint_point_copy(r, q);
        return;
    }

    if (q->is_at_infinity)
    {
        xint_point_copy(r, p);
        return;
    }

    // lambda = (y1 + y2) / (x1 + x2)
    xint_t lambda = XINT_INIT_VAL;
    
    xint_t y_sum = XINT_INIT_VAL;
    field_add(y_sum, p->y, q->y, c);
    
    xint_t x_sum = XINT_INIT_VAL;
    field_add(x_sum, p->x, q->x, c);
    
    field_inv(lambda, x_sum, c);
    field_mul(lambda, lambda, y_sum, c);
    
    xint_t x3 = XINT_INIT_VAL;
    field_mul(x3, lambda, lambda, c);
    field_add(x3, x3, lambda, c);
    field_add(x3, x3, p->x, c);
    field_add(x3, x3, q->x, c);
    xint_t a = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(a, c->a, c->nwords);
    field_add(x3, x3, a, c);
    
    xint_t y3 = XINT_INIT_VAL;
    field_add(y3, p->x, x3, c);
    field_mul(y3, y3, lambda, c);
    field_add(y3, y3, x3, c);
    field_add(y3, y3, p->y, c);
    xint_copy(r->x, x3);
    xint_copy(r->y, y3);
    xint_delete(lambda);
    xint_delete(y_sum);
    xint_delete(x_sum);
    xint_delete(x3);
    xint_delete(y3);
}

static void point_double(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_ecc_curve_t *c)
{
    if (p->is_at_infinity)
    {
        xint_point_copy(r, p);
        return;
    }
    
    // lambda = (y1 / x1) + x1
    xint_t lambda = XINT_INIT_VAL;
    
    field_inv(lambda, p->x, c);            // 1/x1
    field_mul(lambda, lambda, p->y, c);    // y1/x1
    field_add(lambda, lambda, p->x, c);    // y1/x1 + x1
    
    xint_t x3 = XINT_INIT_VAL;
    field_mul(x3, lambda, lambda, c);
    field_add(x3, x3, lambda, c);
    field_add(x3, x3, p->x, c);
    field_add(x3, x3, p->x, c);
    xint_t a = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(a, c->a, c->nwords);
    field_add(x3, x3, a, c);
    
    xint_t y3 = XINT_INIT_VAL;
    field_add(y3, p->x, x3, c);
    field_mul(y3, y3, lambda, c);
    field_add(y3, y3, x3, c);
    field_add(y3, y3, p->y, c);
    xint_copy(r->x, x3);
    xint_copy(r->y, y3);
    xint_delete(lambda);
    xint_delete(x3);
    xint_delete(y3);
}

static void point_double_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c)
{
    
}

static void point_add_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t *c)
{
    
}

static void xint_mod_fast_k163(xword_t *w, xword_t *u)
{
    //xint_mod_std(w, u, &k163);
}

void m_add(xint_t X1, xint_t Z1, xint_t X2, xint_t Z2, const xint_t px, const xint_ecc_curve_t *c)
{
    xint_t T1 = XINT_INIT_VAL;
    xint_t T2 = XINT_INIT_VAL;
    xint_copy(T1, px);
    field_mul(X1, X1, Z2, c);
    field_mul(Z1, Z1, X2, c);
    field_mul(T2, X1, Z1, c);
    field_add(Z1, Z1, X1, c);
    field_squ(Z1, Z1, c);
    field_mul(X1, Z1, T1, c);
    field_add(X1, X1, T2, c);
    xint_delete(T1);
    xint_delete(T2);
}

void m_dbl(xint_t X2, xint_t Z2, const xint_ecc_curve_t *c)
{
    xint_t T1 = XINT_INIT_VAL;
    xint_t T2 = XINT_INIT_VAL;
    xint_assign_long(T1, 1); // c is 1 for now
    field_squ(X2, X2, c);
    field_squ(Z2, Z2, c);
    field_mul(T1, T1, Z2, c);
    field_add(T1, X2, T1, c);
    field_mul(Z2, X2, Z2, c);
    field_squ(X2, T1, c);
    xint_delete(T1);
    xint_delete(T2);
}

void scalar_multiply_mont_x_only(
            xint_ecc_point_t R,
            const xint_ecc_point_t P,
            const xint_t k,
            const xint_ecc_curve_t *c)
{
    xint_t X1 = XINT_INIT_VAL;
    xint_t Z1 = XINT_INIT_VAL;
    xint_t X2 = XINT_INIT_VAL;
    xint_t Z2 = XINT_INIT_VAL;
    
    if (xint_cmp_ulong(k, 1) <= 0)
    {
        // k is too same
        // return an error
        return;
    }

    // Initial doubling
    xint_copy(X1, P->x);
    xint_assign_ulong(Z1, 1);
    field_squ(Z2, X1, c);
    field_squ(X2, Z2, c);
    xint_t b = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(b, c->b, c->nwords);
    field_add(X2, X2, b, c);

//    xint_print_hex("X1", X1);
//    xint_print_hex("Z1", Z1);
//    xint_print_hex("X2", X2);
//    xint_print_hex("Z2", Z2);

    // We need the top bit set
    int nbits = xint_highest_bit_num(k) + 1;
    for (int i=nbits-2; i >= 0; --i)
    {
        if (xint_get_bit(k, i))
        {
            m_add(X1, Z1, X2, Z2, P->x, c);
            m_dbl(X2, Z2, c);
        }
        else
        {
            m_add(X2, Z2, X1, Z1, P->x, c);
            m_dbl(X1, Z1, c);
        }
//        xint_print_hex("X1", X1);
//        xint_print_hex("Z1", Z1);
//        xint_print_hex("X2", X2);
//        xint_print_hex("Z2", Z2);
    }
    
    xint_t zinv = XINT_INIT_VAL;
    field_inv(zinv, Z1, c);
    field_mul(R->x, X1, zinv, c);
//    xint_print_hex("R->x", R->x);
//    xint_print_hex("k", k);

    xint_delete(X1);
    xint_delete(Z1);
    xint_delete(X2);
    xint_delete(Z2);
}
