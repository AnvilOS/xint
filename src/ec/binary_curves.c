
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
static void field_red_163(xint_t w, const xint_t u);
static void field_red_233(xint_t w, const xint_t u);
static void field_red_283(xint_t w, const xint_t u);
static void field_red_409(xint_t w, const xint_t u);
static void field_red_571(xint_t w, const xint_t u);
static int is_valid_point(xint_ecc_point_t R, const xint_ecc_curve_t *c);

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
    field_red_163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k163_exp,
    4,
    k163_x,
    scalar_multiply_mont_x_only,
    k163_b,
    is_valid_point,
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
    field_red_233,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k233_exp,
    2,
    k233_x,
    scalar_multiply_mont_x_only,
    k233_b,
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
    field_red_283,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k283_exp,
    4,
    k283_x,
    scalar_multiply_mont_x_only,
    k283_b,
};

const xword_t k409_p[]  = { 0 };
const xword_t k409_a[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
const xword_t k409_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), 0x0000000 };
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
    field_red_409,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k409_exp,
    2,
    k409_x,
    scalar_multiply_mont_x_only,
    k409_b,
};

const xword_t k571_p[]  = { 0 };
const xword_t k571_a[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000) };
const xword_t k571_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00000000) };
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
    field_red_571,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    k571_exp,
    4,
    k571_x,
    scalar_multiply_mont_x_only,
    k571_b,
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
const xword_t b163_sqrt_b[]  = { X(0x69f34da5, 0xda89c039), X(0x3d21c366, 0xdf892759), X(0xc25b85ba, 0x02) };
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
    field_red_163,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b163_exp,
    4,
    b163_x,
    scalar_multiply_mont_x_only,
    b163_sqrt_b,
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
const xword_t b233_sqrt_b[]  = { X(0x61da9138, 0xe5f946d0), X(0x52f21253, 0x71caaeea), X(0xee31e06d, 0x7874e747), X(0xf85627b9, 0x0187) };
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
    field_red_233,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b233_exp,
    2,
    b233_x,
    scalar_multiply_mont_x_only,
    b233_sqrt_b,
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
const xword_t b283_sqrt_b[]  = { X(0xe9b9b3f6, 0x17442aed), X(0x17c082ae, 0x304424ca), X(0xa2fd220a, 0x9fb6f835), X(0xe8198308, 0x5792b1eb), 0x72bcc9c };
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
    field_red_283,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b283_exp,
    4,
    b283_x,
    scalar_multiply_mont_x_only,
    b283_sqrt_b,
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
const xword_t b409_sqrt_b[]  = { X(0xbc25d5ef, 0x872accf0), X(0x8a48e27b, 0x73326c52), X(0x0cf65767, 0xfde89595), X(0x7c1b2649, 0xd0ad7ce5), X(0x5d93ab2e, 0xa29f53cb), X(0xef22f9b4, 0xe4768ee2), 0x09935f7 };
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
    field_red_409,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b409_exp,
    2,
    b409_x,
    scalar_multiply_mont_x_only,
    b409_sqrt_b,
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
const xword_t b571_sqrt_b[]  = { X(0x3b761c43, 0x699b0844), X(0x0ce39b64, 0x71bedfc1), X(0x3594a7f7, 0x06f0340e), X(0x460cd20c, 0x60536b58), X(0xa874ab0b, 0x362c4800), X(0x255902e6, 0x041d7aa1), X(0x135429eb, 0x68d41c59), X(0x8dffd582, 0xdd739a05), X(0x640c20b5, 0x732d556) };
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
    field_red_571,
    point_add_jacobian,
    point_double_jacobian,
    0xc9,
    b571_exp,
    4,
    b571_x,
    scalar_multiply_mont_x_only,
    b571_sqrt_b,
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
    xword_t wintab[16][40];
    for (int i=0; i<16; ++i)
    {
        xll_zero(wintab[i], c->nwords);
    }
    xll_copy(wintab[1], u->data, u->size);
    for (int i=2; i<16; ++i)
    {
        if ((i % 2) == 0)
        {
            xll_lshift(wintab[i], wintab[i/2], c->nwords, 1);
        }
        else
        {
            int j;
            for (j=0; j<u->size; ++j)
            {
                wintab[i][j] = wintab[i-1][j] ^ u->data[j];
            }
            for (   ; j<c->nwords; ++j)
            {
                wintab[i][j] = wintab[i-1][j];
            }
        }
    }
    xword_t C[2*c->nwords];
    xll_zero(C, 2*c->nwords);
    for (int k=(XWORD_BITS/4-1); k>=0; --k)
    {
        xll_lshift(C, C, 2*c->nwords, 4);
        for (int j=v->size-1; j>=0; --j)
        {
            uint8_t window = (v->data[j] >> (4 * k)) & 0xF;
            if (window)
            {
                for (int i=0; i<c->nwords; i++)
                {
                    C[i + j] ^= wintab[window][i];
                }
            }
        }
    }
    FAST_RESIZE(w, 2*c->nwords);
    xll_copy(w->data, C, 2*c->nwords);
    trim_zeroes(w);
    field_red(w, c);
}

static const unsigned short pre_calc_sq[256] = {
    0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015, 0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055, 0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115, 0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155, 0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415, 0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455, 0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515, 0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555, 0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015, 0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055, 0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115, 0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155, 0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415, 0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455, 0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515, 0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555, 0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015, 0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055, 0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115, 0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155, 0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415, 0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455, 0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515, 0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555, 0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015, 0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055, 0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115, 0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155, 0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415, 0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455, 0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515, 0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555,
};

static void field_squ(xint_t w, const xint_t u, const xint_ecc_curve_t *c)
{
    int Un = XINT_ABS(u->size);
    
    FAST_RESIZE(w, Un * 2);
    for (int i=Un-1; i>=0; --i)
    {
        xword_t uu = u->data[i];
#if XWORD_BITS == 32
        w->data[2*i+1] =  (xword_t)(pre_calc_sq[uu >> 24&0xff]) << 16
                        | (xword_t)(pre_calc_sq[uu >> 16&0xff]);
        w->data[2*i]   =  (xword_t)(pre_calc_sq[uu >>  8&0xff]) << 16
                        | (xword_t)(pre_calc_sq[uu      &0xff]);
#elif XWORD_BITS == 64
        w->data[2*i+1] =  (xword_t)(pre_calc_sq[uu >> 56&0xff]) << 48
                        | (xword_t)(pre_calc_sq[uu >> 48&0xff]) << 32
                        | (xword_t)(pre_calc_sq[uu >> 40&0xff]) << 16
                        | (xword_t)(pre_calc_sq[uu >> 32&0xff]);
        w->data[2*i]   =  (xword_t)(pre_calc_sq[uu >> 24&0xff]) << 48
                        | (xword_t)(pre_calc_sq[uu >> 16&0xff]) << 32
                        | (xword_t)(pre_calc_sq[uu >>  8&0xff]) << 16
                        | (xword_t)(pre_calc_sq[uu      &0xff]);
#else
#error XWORD_BITS must be 32 or 64
#endif
    }
    trim_zeroes(w);
    field_red(w, c);
}

static void field_red_163(xint_t w, const xint_t u)
{
#if XWORD_BITS == 32
    XLL_ASSERT(w->size<=11);
    xword_t T;
    FAST_RESIZE_0(w, 11);
    for (int i=10; i>=6; --i)
    {
        T = w->data[i];
        w->data[i-4] ^= (T >> 28) ^ (T >> 29);
        w->data[i-5] ^= (T << 4) ^ (T << 3) ^ (T) ^ (T >> 3);
        w->data[i-6] ^= (T << 29);
    }
    T = w->data[5] & 0xfffffff8;
    w->data[1] ^= (T >> 28) ^ (T >> 29);
    w->data[0] ^= (T << 4) ^ (T << 3) ^ (T) ^ (T >> 3);
    w->data[5] &= 0x00000007;
    w->size = 6;
    trim_zeroes(w);
#elif XWORD_BITS == 64
    XLL_ASSERT(w->size<=6);
    xword_t T;
    FAST_RESIZE_0(w, 6);
    for (int i=5; i>=3; --i)
    {
        T = w->data[i];
        w->data[i-2] ^= (T >> 35) ^ (T >> 32) ^ (T >> 29) ^ (T >> 28);
        w->data[i-3] ^= (T << 29) ^ (T << 32) ^ (T << 35) ^ (T << 36);
    }
    T = w->data[2] & 0xfffffff800000000ULL;
    w->data[0] ^= (T >> 35) ^ (T >> 32) ^ (T >> 29) ^ (T >> 28);
    w->data[2] &= 0x00000007ffffffffULL;
    w->size = 3;
    trim_zeroes(w);
#else
#error XWORD_BITS must be 32 or 64
#endif
}

static void field_red_233(xint_t w, const xint_t u)
{
#if XWORD_BITS == 32
    XLL_ASSERT(w->size<=15);
    xword_t T;
    FAST_RESIZE_0(w, 15);
    for (int i=14; i>=8; --i)
    {
        T = w->data[i];
        w->data[i-4] ^= (T >> 31);
        w->data[i-5] ^= (T << 1);
        w->data[i-7] ^= (T >> 9);
        w->data[i-8] ^= (T << 23);
    }
    T = w->data[7] & 0xfffffe00;
    w->data[3] ^= (T >> 31);
    w->data[2] ^= (T << 1);
    w->data[0] ^= (T >> 9);
    w->data[7] &= 0x1ff;
    w->size = 8;
    trim_zeroes(w);
#elif XWORD_BITS == 64
    XLL_ASSERT(w->size<=8);
    xword_t T;
    FAST_RESIZE_0(w, 8);
    for (int i=7; i>=4; --i)
    {
        T = w->data[i];
        w->data[i-2] ^= (T >> 31);
        w->data[i-3] ^= (T << 33);
        w->data[i-3] ^= (T >> 41);
        w->data[i-4] ^= (T << 23);
    }
    T = w->data[3] & 0xfffffe0000000000ULL;
    w->data[1] ^= (T >> 31);
    w->data[0] ^= (T << 33);
    w->data[0] ^= (T >> 41);
    w->data[3] &= 0x000001ffffffffffULL;
    w->size = 4;
    trim_zeroes(w);
#else
#error XWORD_BITS must be 32 or 64
#endif
}

static void field_red_283(xint_t w, const xint_t u)
{
#if XWORD_BITS == 32
    XLL_ASSERT(w->size<=18);
    xword_t T;
    FAST_RESIZE_0(w, 18);
    for (int i=17; i>=9; --i)
    {
        T = w->data[i];
        w->data[i-9] ^= (T <<  5) ^ (T << 10) ^ (T << 12) ^ (T << 17);
        w->data[i-8] ^= (T >> 27) ^ (T >> 22) ^ (T >> 20) ^ (T >> 15);
    }
    T = w->data[8] & 0xf8000000;
    w->data[0] ^= (T >> 27) ^ (T >> 22) ^ (T >> 20) ^ (T >> 15);
    w->data[8] &= 0x07ffffff;
    w->size = 9;
    trim_zeroes(w);
#elif XWORD_BITS == 64
    XLL_ASSERT(w->size<=9);
    xword_t T;
    FAST_RESIZE_0(w, 9);
    for (int i=8; i>=5; --i)
    {
        T = w->data[i];
        w->data[i-5] ^= (T << 37) ^ (T << 42) ^ (T << 44) ^ (T << 49);
        w->data[i-4] ^= (T >> 27) ^ (T >> 22) ^ (T >> 20) ^ (T >> 15);
    }
    T = w->data[4] & 0xfffffffff8000000ULL;
    w->data[0] ^= (T >> 27) ^ (T >> 22) ^ (T >> 20) ^ (T >> 15);
    w->data[4] &= 0x0000000007ffffffULL;
    w->size = 5;
    trim_zeroes(w);
#else
#error XWORD_BITS must be 32 or 64
#endif
}

static void field_red_409(xint_t w, const xint_t u)
{
#if XWORD_BITS == 32
    XLL_ASSERT(w->size<=26);
    xword_t T;
    FAST_RESIZE_0(w, 26);
    for (int i=25; i>=13; --i)
    {
        T = w->data[i];
        w->data[i-13] ^= (T <<  7);
        w->data[i-12] ^= (T >> 25);
        w->data[i-11] ^= (T << 30);
        w->data[i-10] ^= (T >>  2);
    }
    T = w->data[12] & 0xfe000000;
    w->data[0] ^= (T >> 25);
    w->data[2] ^= (T >>  2);
    w->data[12] &= 0x01ffffff;
    w->size = 13;
    trim_zeroes(w);
#elif XWORD_BITS == 64
    XLL_ASSERT(w->size<=13);
    xword_t T;
    FAST_RESIZE_0(w, 13);
    for (int i=12; i>=7; --i)
    {
        T = w->data[i];
        w->data[i-7] ^= (T << 39);
        w->data[i-6] ^= (T >> 25);
        w->data[i-6] ^= (T << 62);
        w->data[i-5] ^= (T >>  2);
    }
    T = w->data[6] & 0xfffffffffe000000ULL;
    w->data[0] ^= (T >> 25);
    w->data[1] ^= (T >>  2);
    w->data[6] &= 0x0000000001ffffffULL;
    w->size = 7;
    trim_zeroes(w);
#else
#error XWORD_BITS must be 32 or 64
#endif
}

static void field_red_571(xint_t w, const xint_t u)
{
#if XWORD_BITS == 32
    XLL_ASSERT(w->size<=36);
    xword_t T;
    FAST_RESIZE_0(w, 36);
    for (int i=35; i>=18; --i)
    {
        T = w->data[i];
        w->data[i-18] ^= (T <<  5) ^ (T <<  7) ^ (T << 10) ^ (T << 15);
        w->data[i-17] ^= (T >> 27) ^ (T >> 25) ^ (T >> 22) ^ (T >> 17);
    }
    T = w->data[17] & 0xf8000000;
    w->data[0] ^= (T >> 27) ^ (T >> 25) ^ (T >> 22) ^ (T >> 17);
    w->data[17] &= 0x07ffffff;
    w->size = 18;
    trim_zeroes(w);
#elif XWORD_BITS == 64
    XLL_ASSERT(w->size<=18);
    xword_t T;
    FAST_RESIZE_0(w, 18);
    for (int i=17; i>=9; --i)
    {
        T = w->data[i];
        w->data[i-9] ^= (T <<  5) ^ (T <<  7) ^ (T << 10) ^ (T << 15);
        w->data[i-8] ^= (T >> 59) ^ (T >> 57) ^ (T >> 54) ^ (T >> 49);
    }
    T = w->data[8] & 0xf800000000000000ULL;
    w->data[0] ^= (T >> 59) ^ (T >> 57) ^ (T >> 54) ^ (T >> 49);
    w->data[8] &= 0x07ffffffffffffffULL;
    w->size = 9;
    trim_zeroes(w);
#else
#error XWORD_BITS must be 32 or 64
#endif
}

static void field_red(xint_t w, const xint_ecc_curve_t *c)
{
    if (c->xint_mod_fast)
    {
        c->xint_mod_fast(w, w);
        return;
    }
    
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
    field_red(w, cve);
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

void m_add(xint_t X1, xint_t Z1, const xint_t X2, const xint_t Z2, const xint_t px, const xint_ecc_curve_t *c)
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
    xint_t sqrt_b = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(sqrt_b, c->sqrt_b, c->nwords);
    xint_copy(T1, sqrt_b);
    field_squ(X2, X2, c);
    field_squ(Z2, Z2, c);
    field_mul(T1, T1, Z2, c);
    field_add(T1, X2, T1, c);
    field_mul(Z2, X2, Z2, c);
    field_squ(X2, T1, c);
    xint_delete(T1);
    xint_delete(T2);
}

void scalar_multiply_mont_x_only(xint_ecc_point_t R, const xint_ecc_point_t P, const xint_t k, const xint_ecc_curve_t *c)
{
    xint_t X[2];
    xint_t Z[2];
    
    if (xint_cmp_ulong(k, 1) <= 0)
    {
        // k is too same
        // return an error
        return;
    }
    
    xint_init(X[0]);
    xint_init(X[1]);
    xint_init(Z[0]);
    xint_init(Z[1]);

    // Initial doubling
    xint_copy(X[0], P->x);
    xint_assign_ulong(Z[0], 1);
    field_squ(Z[1], X[0], c);
    field_squ(X[1], Z[1], c);
    xint_t b = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(b, c->b, c->nwords);
    field_add(X[1], X[1], b, c);

    // We need the top bit set
    int nbits = xint_highest_bit_num(k) + 1;
    for (int i=nbits-2; i >= 0; --i)
    {
        int bit = xint_get_bit(k, i);
        m_add(X[1-bit], Z[1-bit], X[bit], Z[bit], P->x, c);
        m_dbl(X[bit], Z[bit], c);
    }
    
    xint_t zinv = XINT_INIT_VAL;
    field_inv(zinv, Z[0], c);
    field_mul(R->x, X[0], zinv, c);
    
    xint_t T1 = XINT_INIT_VAL;
    xint_t T2 = XINT_INIT_VAL;
    field_mul(T1, P->x, Z[0], c);
    field_add(T1, T1, X[0], c);
    field_mul(T2, P->x, Z[1], c);
    field_add(T2, T2, X[1], c);
    field_mul(T1, T1, T2, c);
    
    field_squ(T2, P->x, c);
    field_add(T2, T2, P->y, c);
    field_mul(T2, T2, Z[0], c);
    field_mul(T2, T2, Z[1], c);
    
    field_add(T1, T1, T2, c);
    
    field_mul(T2, X[0], zinv, c);
    field_add(T2, T2, P->x, c);
    
    field_mul(T1, T1, T2, c);
    
    field_mul(T2, P->x, Z[0], c);
    field_mul(T2, T2, Z[1], c);
    field_inv(T2, T2, c);
    
    field_mul(T1, T1, T2, c);
    field_add(R->y, T1, P->y, c);
    
    xint_delete(X[0]);
    xint_delete(Z[0]);
    xint_delete(X[1]);
    xint_delete(Z[1]);
}

int is_valid_point(xint_ecc_point_t P, const xint_ecc_curve_t *c)
{
    // Check that P lies on the curve y^2 + xy = x^3 + ax^2 + b
    xint_t lhs = XINT_INIT_VAL;
    xint_t rhs = XINT_INIT_VAL;
    xint_t tmp = XINT_INIT_VAL;

    xint_t a = XINT_INIT_VAL;
    xint_t b = XINT_INIT_VAL;
    CONST_XINT_FROM_XWORDS(a, c->a, c->nwords);
    CONST_XINT_FROM_XWORDS(b, c->b, c->nwords);

    // y^2 + xy
    field_squ(lhs, P->y, c);
    field_mul(tmp, P->x, P->y, c);
    field_add(lhs, lhs, tmp, c);

    // x^3 + ax^2 + b
    field_squ(tmp, P->x, c);
    field_mul(rhs, tmp, P->x, c);
    if (xint_cmp_ulong(a, 1) == 0)
    {
        field_add(rhs, rhs, tmp, c);
    }
    field_add(rhs, rhs, b, c);

    int on_curve = (xint_cmp(lhs, rhs) == 0);
    
    xint_delete(lhs);
    xint_delete(rhs);
    xint_delete(tmp);

    return on_curve;
}
