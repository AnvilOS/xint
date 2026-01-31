
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
};

const xword_t k283_p[]  = { 0 };
const xword_t k283_a[]  = { X(0x00000000, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x0000000000), X(0x00000000, 0x0000000000), 0x0000 };
const xword_t k283_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x0000000000), X(0x00000000, 0x0000000000), 0x0000 };
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
};

const xword_t k409_p[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k409_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k409_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k409_Gx[] = { X(0x5c94eee8, 0xde4e6d5e), X(0xaa07d793, 0x7bbc11ac), X(0xfe13c053, 0x02) };
const xword_t k409_Gy[] = { X(0xccdaa3d9, 0x0536d538), X(0x321f2e80, 0x5d38ff58), X(0x89070fb0, 0x02) };
const xword_t k409_n[]  = { X(0x99f8a5ef, 0xa2e0cc0d), X(0x00020108, 0x00000000), X(0x00000000, 0x04) };
const xword_t k409_h[]  = { 0x02 };
const int k409_exp[] = { 7, 6, 3, 0 };
const xword_t k409_x[]  = { X(0x000000C9, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x08) };
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
    4,
    k409_x,
};

const xword_t k571_p[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k571_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k571_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k571_Gx[] = { X(0x5c94eee8, 0xde4e6d5e), X(0xaa07d793, 0x7bbc11ac), X(0xfe13c053, 0x02) };
const xword_t k571_Gy[] = { X(0xccdaa3d9, 0x0536d538), X(0x321f2e80, 0x5d38ff58), X(0x89070fb0, 0x02) };
const xword_t k571_n[]  = { X(0x99f8a5ef, 0xa2e0cc0d), X(0x00020108, 0x00000000), X(0x00000000, 0x04) };
const xword_t k571_h[]  = { 0x02 };
const int k571_exp[] = { 7, 6, 3, 0 };
const xword_t k571_x[]  = { X(0x000000C9, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x08) };
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
        xll_move(w->data+Vn, u->data+Vn, Un-Vn);
    }
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
    while (degv >= 0)
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
}

static void point_double(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_ecc_curve_t *c)
{
//    xint_t IN = XINT_INIT_VAL;
//    xint_t OUT = XINT_INIT_VAL;
//    xint_t PROD = XINT_INIT_VAL;
//    xint_assign_str(IN, "0x5", 0);
//    field_inv(OUT, IN);
//    xint_print_hex("INV", OUT);
//    xint_print_hex("IN ", IN);
//
//    field_mul(PROD, OUT, IN);
//    xint_print_hex("PROD", PROD);
//    field_red(PROD);
//    xint_print_hex("PROD", PROD);
//
//    field_mul(PROD, IN, OUT);
//    xint_print_hex("PROD", PROD);
//
//    xint_print_hex("PROD", PROD);
//    field_red(PROD);
//    xint_print_hex("OUT", OUT);
//    xint_print_hex("IN ", IN);
//    xint_print_hex("PROD", PROD);

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

