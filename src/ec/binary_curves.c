
#include "binary_curves.h"

#include "xint_internal.h"
#include "xint_algorithms.h"
#include "xint_io.h"
#include "xint_bitwise.h"

static void point_add(xint_ecc_point_t r, const xint_ecc_point_t q, const xint_ecc_point_t p, const xint_t m);
static void point_double(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_t a, const xint_t m);
static void point_add_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_point_jacobian_t Qj, const xint_ecc_curve_t *c);
static void point_double_jacobian(xint_ecc_point_jacobian_t Rjx, const xint_ecc_point_jacobian_t Pj, const xint_ecc_curve_t *c);
static void xint_mod_fast_k163(xword_t *w, xword_t *u);

const xword_t k163_p[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_a[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_b[]  = { X(0x00000001, 0x00000000), X(0x00000000, 0x00000000), X(0x00000000, 0x00) };
const xword_t k163_Gx[] = { X(0x5c94eee8, 0xde4e6d5e), X(0xaa07d793, 0x7bbc11ac), X(0xfe13c053, 0x02) };
const xword_t k163_Gy[] = { X(0xccdaa3d9, 0x0536d538), X(0x321f2e80, 0x5d38ff58), X(0x89070fb0, 0x02) };
const xword_t k163_n[]  = { X(0x99f8a5ef, 0xa2e0cc0d), X(0x00020108, 0x00000000), X(0x00000000, 0x04) };
const xword_t k163_h[]  = { 0x02 };
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
};

static void field_red(xint_t w);

static void field_add(xint_ptr w, const_xint_ptr u, const_xint_ptr v)
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

static void field_mul(xint_ptr w, const_xint_ptr u, const_xint_ptr v)
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
}

static void field_squ(xint_t w, const xint_t u)
{
    
}

static void field_red(xint_t w)
{
}

static void field_inv(xint_t w, const xint_t a)
{
    char *fx_str = "0x0800000000000000000000000000000000000000C9";
    xint_t b = XINT_INIT_VAL;
    xint_t c = XINT_INIT_VAL;
    xint_t u = XINT_INIT_VAL;
    xint_t v = XINT_INIT_VAL;
    xint_t TMP = XINT_INIT_VAL;
    xint_assign_long(b, 1);
    xint_assign_zero(c);
    xint_copy(u, a);
    xint_assign_str(v, fx_str, 0);
    while (1)
    {
        int degu = xint_highest_bit_num(u);
        int degv = xint_highest_bit_num(v);
        if (degu == 0)
        {
            break;
        }
        int j = degu - degv;
        if (j < 0)
        {
            xint_swap(u, v);
            xint_swap(b, c);
            j = -j;
        }
        xint_copy(TMP, v);
        xint_lshift(TMP, TMP, j);
        field_add(u, u, TMP);
        xint_copy(TMP, c);
        xint_lshift(TMP, TMP, j);
        field_add(b, b, TMP);
    }
    xint_copy(w, b);
}

static void point_add(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_ecc_point_t q, const xint_t m)
{
    // lambda = (y1 + y2) / (x1 + x2)
    xint_t lambda = XINT_INIT_VAL;
    
    xint_t y_sum = XINT_INIT_VAL;
    field_add(y_sum, p->y, q->y);
    
    xint_t x_sum = XINT_INIT_VAL;
    field_add(y_sum, p->x, q->x);
    
    field_inv(lambda, x_sum);
    field_mul(lambda, lambda, y_sum);
}

static void point_double(xint_ecc_point_t r, const xint_ecc_point_t p, const xint_t a, const xint_t m)
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

    // lambda = (y1 / x1) + x1
    xint_t lambda = XINT_INIT_VAL;
    
    field_inv(lambda, p->x);      // 1/x1
    field_mul(lambda, lambda, p->y);         // y1/x1
    field_red(lambda);
    field_add(lambda, lambda, p->x);         // y1/x1 + x1
    
    xint_t x3 = XINT_INIT_VAL;
    field_mul(x3, lambda, lambda);
    field_red(x3);
    field_add(x3, x3, lambda);
    field_add(x3, x3, p->x);
    field_add(x3, x3, p->x);
    field_add(x3, x3, a);
    
    xint_t y3 = XINT_INIT_VAL;
    field_add(y3, p->x, x3);
    field_mul(y3, y3, lambda);
    field_red(y3);
    field_add(y3, y3, x3);
    field_add(y3, y3, p->y);
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

