
#include "test_ecc.h"
#include "test_harness.h"

#include "xint.h"
#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"
#include "xint_internal.h"
#include "nist_vectors.h"
#include "prime_curves.h"

#include "time_stamp.h"

#include "sha256.h"
#include "hmac.h"

#include <string.h>

static int test_equality(const xint_t x, char *p)
{
    xint_t y = XINT_INIT_VAL;
    xint_assign_str(y, p, 0);
    int ret = xint_cmp(x, y);
    xint_delete(y);
    return ret;
}

static int test_equality_ex(const xword_t *x, int xn, char *p)
{
    xint_t y = XINT_INIT_VAL;
    xint_assign_str(y, p, 0);
    if (xn != y->size)
    {
        return -1;
    }
    int ret = xll_cmp(x, y->data, xn);
    xint_delete(y);
    return ret;
}


TEST_GROUP(ecc);

TEST(ecc, hmac1)
{
    // Test vector from Wikipedia
    uint8_t digest[32];
    const uint8_t expected[32] =
    {
        0xf7, 0xbc, 0x83, 0xf4, 0x30, 0x53, 0x84, 0x24, 0xb1, 0x32, 0x98, 0xe6, 0xaa, 0x6f, 0xb1, 0x43,
        0xef, 0x4d, 0x59, 0xa1, 0x49, 0x46, 0x17, 0x59, 0x97, 0x47, 0x9d, 0xbc, 0x2d, 0x1a, 0x3c, 0xd8
    };
    const char *key = "key";
    const char *msg = "The quick brown fox jumps over the lazy dog";
    hmac_calc(digest, (uint8_t *)key, strlen(key), (uint8_t *)msg, strlen(msg));
    ASSERT_EQ(0, memcmp(digest, expected, 32));

    END_TEST(ecc);
}

TEST(ecc, hmac2)
{
    // Test vector from RFC 4231
    uint8_t digest[32];
    const uint8_t expected[32] =
    {
        0x60, 0xe4, 0x31, 0x59, 0x1e, 0xe0, 0xb6, 0x7f, 0x0d, 0x8a, 0x26, 0xaa, 0xcb, 0xf5, 0xb7, 0x7f,
        0x8e, 0x0b, 0xc6, 0x21, 0x37, 0x28, 0xc5, 0x14, 0x05, 0x46, 0x04, 0x0f, 0x0e, 0xe3, 0x7f, 0x54
    };
    const char *key =
    "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
    "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
    "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
    "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
    "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa";

    const char *msg = "Test Using Larger Than Block-Size Key - Hash Key First";
    hmac_calc(digest, (uint8_t *)key, strlen(key), (uint8_t *)msg, strlen(msg));
    ASSERT_EQ(0, memcmp(digest, expected, 32));

    END_TEST(ecc);
}

TEST(ecc, k_generation)
{
    // From worked example in RFC 6979
    // order:
    // q = 0x4000000000000000000020108A2E0CC0D99F8A5EF
    // qlen = 163 bits
    char *q_str = "0x4000000000000000000020108A2E0CC0D99F8A5EF";
    int qlen = 163;
    xint_t q_int = XINT_INIT_VAL;
    xint_assign_str(q_int, q_str, 0);
   
    // private key:
    // x = 0x09A4D6792295A7F730FC3F2B49CBC0F62E862272F
    char *x = "0x09A4D6792295A7F730FC3F2B49CBC0F62E862272F";
    
    xint_t x_int = XINT_INIT_VAL;
    xint_assign_str(x_int, x, 0);

    // public key:
    // Ux = 0x79AEE090DB05EC252D5CB4452F356BE198A4FF96F
    // Uy = 0x782E29634DDC9A31EF40386E896BAA18B53AFA5A3

    // Input message = "sample"
    char *m = "sample";
    
    xint_t k = XINT_INIT_VAL;
    static const int hlen = 32;
    uint8_t h1[hlen];
    sha256_calc(h1, (uint8_t *)m, strlen(m));
    ecc_gen_deterministic_k(k, h1, hlen, x_int, &k163);

    char *k_str = xint_to_string(k, 16);
    ASSERT_EQ(0, strcasecmp(k_str, "23AF4074C90A02B3FE61D286D5C87F425E6BDD81B"));
    free(k_str);
    
    END_TEST(ecc);
}

TEST(ecc, rfc_6979)
{
    // From test case for P256 in RFC 6979
    char *q = "0xFFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551";
    int qlen = 256;
   
    // private key:
    char *x = "0xC9AFA9D845BA75166B5C215767B1D6934E50C3DB36E89B127B8A622B120F6721";

    // public key:
    char *Ux_exp = "0x60FED4BA255A9D31C961EB74C6356D68C049B8923B61FA6CE669622E60F29FB6";
    char *Uy_exp = "0x7903FE1008B8BC99A41AE9E95628BC64F2F1B20C2D7E9F5177A3C294D4462299";
    
    // Input message = "sample"
    char *m = "sample";
    
    // Expected values
    char *k_exp = "0xA6E3C57DD01ABE90086538398355DD4C3B17AA873382B0F24D6129493D8AAD60";
    char *r_exp = "0xEFD48B2AACB6A8FD1140DD9CD45E81D69D2C877B56AAF991C34D0EA84EAF3716";
    char *s_exp = "0xF7CB1C942D657C41D436C7A1B6E29F65F3E900DBB9AFF4064DC4AB2F843ACDA8";

    // Check the q
    ASSERT_EQ(0, test_equality_ex(p256.n, p256.nwords, q));

    // Check the public key
    xint_t priv = XINT_INIT_VAL;
    xint_assign_str(priv, x, 0);
    xint_ecc_point_t pub;
    xint_point_init(pub);
    STAMP_VARS();
    __disable_irq();
    STAMP_BEFORE();
    xint_ecc_get_public_key(pub, priv, &p256);
    STAMP_AFTER();
    __enable_irq();
    printf("xint_ecc_get_public_key : %u\n", STAMP_DIFF());
    pub->is_at_infinity = 0;
    ASSERT_EQ(0, test_equality(pub->x, Ux_exp));
    ASSERT_EQ(0, test_equality(pub->y, Uy_exp));
    
    // Calc the hash
    static const int hlen = 32;
    uint8_t h1[hlen];
    sha256_calc(h1, (uint8_t *)m, strlen(m));
    
    // Check that we generate the correct k
    xint_t k = XINT_INIT_VAL;
    ecc_gen_deterministic_k(k, h1, 32, priv, &p256);
    ASSERT_EQ(0, test_equality(k, k_exp));

    // Check the sig
    xint_ecc_sig_t signature;
    xint_init(signature->r);
    xint_init(signature->s);
    //STAMP_VARS();
    __disable_irq();
    STAMP_BEFORE();
    xint_ecc_sign_det(signature, h1, hlen, priv, &p256);
    STAMP_AFTER();
    __enable_irq();
    printf("xint_ecc_sign_det : %u\n", STAMP_DIFF());
    printf("Calculated\n");
    xint_print_hex("sigR", signature->r);
    xint_print_hex("sigS", signature->s);
    printf("\n");
    ASSERT_EQ(0, test_equality(signature->r, r_exp));
    ASSERT_EQ(0, test_equality(signature->s, s_exp));
    
    //STAMP_VARS();
    __disable_irq();
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    ASSERT_EQ(1, xint_ecc_verify(signature, h1, hlen, pub, &p256));
    STAMP_AFTER();
    __enable_irq();
    printf("xint_ecc_verify : %u\n", STAMP_DIFF());

    END_TEST(ecc);
}

TEST(ecc, simple_gcd_and_inverse)
{
    xint_t u = XINT_INIT_VAL;
    xint_t v = XINT_INIT_VAL;
    xint_t ud = XINT_INIT_VAL;
    xint_t vd = XINT_INIT_VAL;
    xint_t gcd = XINT_INIT_VAL;

    xint_assign_ulong(u, 40902);
    xint_assign_ulong(v, 24140);

    xint_gcd(gcd, u, v);
    ASSERT_EQ(0, xint_cmp_long(gcd, 34));

    xint_gcd_ext(gcd, ud, vd, u, v);
    ASSERT_EQ(0, xint_cmp_long(gcd, 34));
    ASSERT_EQ(0, xint_cmp_long(ud, 337));
    ASSERT_EQ(0, xint_cmp_long(vd, -571));

    xint_t w = XINT_INIT_VAL;
    xint_assign_ulong(u, 3);
    xint_assign_ulong(v, 11);
    xint_mod_inverse(w, u, v);
    ASSERT_EQ(0, xint_cmp_long(w, 4));

    END_TEST(ecc)
}

TEST(ecc, gen_det_k)
{
    // From worked example in RFC 6979
    // order:
    // q = 0x4000000000000000000020108A2E0CC0D99F8A5EF
    // qlen = 163 bits
    char *q_str = "0x4000000000000000000020108A2E0CC0D99F8A5EF";
    int qlen = 163;
    xint_t q_int = XINT_INIT_VAL;
    xint_assign_str(q_int, q_str, 0);

    // private key:
    // x = 0x09A4D6792295A7F730FC3F2B49CBC0F62E862272F
    char *x = "0x09A4D6792295A7F730FC3F2B49CBC0F62E862272F";

    // public key:
    // Ux = 0x79AEE090DB05EC252D5CB4452F356BE198A4FF96F
    // Uy = 0x782E29634DDC9A31EF40386E896BAA18B53AFA5A3

    // Input message = "sample"
    char *m = "sample";
        

    
    
    END_TEST(ecc)
}

TEST(ecc, curve_p224)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality_ex(p224.p, p224.nwords, "0xffffffffffffffffffffffffffffffff000000000000000000000001"));
    ASSERT_EQ(0, test_equality_ex(p224.a, p224.nwords, "0xfffffffffffffffffffffffffffffffefffffffffffffffffffffffe"));
    ASSERT_EQ(0, test_equality_ex(p224.b, p224.nwords,"0xb4050a850c04b3abf54132565044b0b7d7bfd8ba270b39432355ffb4"));
    ASSERT_EQ(0, test_equality_ex(p224.Gx, p224.nwords, "0xb70e0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21"));
    ASSERT_EQ(0, test_equality_ex(p224.Gy, p224.nwords, "0xbd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34"));
    ASSERT_EQ(0, test_equality_ex(p224.n, p224.nwords, "0xffffffffffffffffffffffffffff16a2e0b8f03e13dd29455c5c2a3d"));
    ASSERT_EQ(0, test_equality_ex(p224.h, 1, "0x1"));
    
    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "A548803B79DF17C40CDE3FF0E36D025143BCBBA146EC32908EB84937", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_point_t G;
    xint_point_init(G);
    CONST_XINT_FROM_XWORDS(G->x, p224.Gx, p224.nwords);
    CONST_XINT_FROM_XWORDS(G->y, p224.Gy, p224.nwords);
    G->is_at_infinity = 0;
    p224.scalar_mul(R, G, x, &p224);
    
    ASSERT_EQ(0, test_equality(R->x, "0xC3A3F5B82712532004C6F6D1DB672F55D931C3409EA1216D0BE77380"));
    ASSERT_EQ(0, test_equality(R->y, "0x9BF4978CA8C8A8DF855A74C6905A5A3947ACFF772FCE436D48341D46"));

    //ASSERT_EQ(1, xint_is_prime(p224.n));

    END_TEST(ecc);
}

TEST(ecc, curve_p256)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality_ex(p256.p, p256.nwords, "0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff"));
    ASSERT_EQ(0, test_equality_ex(p256.a, p256.nwords, "0xffffffff00000001000000000000000000000000fffffffffffffffffffffffc"));
    ASSERT_EQ(0, test_equality_ex(p256.b, p256.nwords, "0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b"));
    ASSERT_EQ(0, test_equality_ex(p256.Gx, p256.nwords, "0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296"));
    ASSERT_EQ(0, test_equality_ex(p256.Gy, p256.nwords, "0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5"));
    ASSERT_EQ(0, test_equality_ex(p256.n, p256.nwords, "0xffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551"));
    ASSERT_EQ(0, test_equality_ex(p256.h, 1, "0x1"));

    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "7A1A7E52797FC8CAAA435D2A4DACE39158504BF204FBE19F14DBB427FAEE50AE", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_point_t G;
    xint_point_init(G);
    CONST_XINT_FROM_XWORDS(G->x, p256.Gx, p256.nwords);
    CONST_XINT_FROM_XWORDS(G->y, p256.Gy, p256.nwords);
    G->is_at_infinity = 0;

    STAMP_VARS();
    __disable_irq();
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    p256.scalar_mul(R, G, x, &p256);
    STAMP_AFTER();
    __enable_irq();
    printf("xint_ecc_mult : %u\n", STAMP_DIFF());
    
    ASSERT_EQ(0, test_equality(R->x, "0x2B42F576D07F4165FF65D1F3B1500F81E44C316F1F0B3EF57325B69ACA46104F"));
    ASSERT_EQ(0, test_equality(R->y, "0x3CE76603264661EA2F602DF7B4510BBC9ED939233C553EA5F42FB3F1338174B5"));

    //ASSERT_EQ(1, xint_is_prime(p256.n));

    END_TEST(ecc);
}

TEST(ecc, curve_p384)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality_ex(p384.p, p384.nwords, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff"));
    ASSERT_EQ(0, test_equality_ex(p384.a, p384.nwords, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc"));
    ASSERT_EQ(0, test_equality_ex(p384.b, p384.nwords, "0xb3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef"));
    ASSERT_EQ(0, test_equality_ex(p384.Gx, p384.nwords, "0xaa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7"));
    ASSERT_EQ(0, test_equality_ex(p384.Gy, p384.nwords, "0x3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f"));
    ASSERT_EQ(0, test_equality_ex(p384.n, p384.nwords, "0xffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973"));
    ASSERT_EQ(0, test_equality_ex(p384.h, 1, "0x1"));

    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "2E44EF1F8C0BEA8394E3DDA81EC6A7842A459B534701749E2ED95F054F0137680878E0749FC43F85EDCAE06CC2F43FEF", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_point_t G;
    xint_point_init(G);
    CONST_XINT_FROM_XWORDS(G->x, p384.Gx, p384.nwords);
    CONST_XINT_FROM_XWORDS(G->y, p384.Gy, p384.nwords);
    G->is_at_infinity = 0;

    p384.scalar_mul(R, G, x, &p384);
    
    ASSERT_EQ(0, test_equality(R->x, "0x30EA514FC0D38D8208756F068113C7CADA9F66A3B40EA3B313D040D9B57DD41A332795D02CC7D507FCEF9FAF01A27088"));
    ASSERT_EQ(0, test_equality(R->y, "0xC04E32465D14C50CBC3BCB88EA20F95B10616663FC62A8DCDB48D3006327EA7CA104F6F9294C66EA2487BD50357010C6"));

    //ASSERT_EQ(1, xint_is_prime(p384.n));

    END_TEST(ecc);
}

TEST(ecc, curve_p521)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality_ex(p521.p, p521.nwords, "0x01ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
    ASSERT_EQ(0, test_equality_ex(p521.a, p521.nwords, "0x01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc"));
    ASSERT_EQ(0, test_equality_ex(p521.b, p521.nwords, "0x0051953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00"));
    ASSERT_EQ(0, test_equality_ex(p521.Gx, p521.nwords, "0x00c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66"));
    ASSERT_EQ(0, test_equality_ex(p521.Gy, p521.nwords, "0x011839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650"));
    ASSERT_EQ(0, test_equality_ex(p521.n, p521.nwords, "0x01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9b8899c47aebb6fb71e91386409"));
    ASSERT_EQ(0, test_equality_ex(p521.h, 1, "0x1"));

    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "C91E2349EF6CA22D2DE39DD51819B6AAD922D3AECDEAB452BA172F7D63E370CECD70575F597C09A174BA76BED05A48E562BE0625336D16B8703147A6A231D6BF", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_point_t G;
    xint_point_init(G);
    CONST_XINT_FROM_XWORDS(G->x, p521.Gx, p521.nwords);
    CONST_XINT_FROM_XWORDS(G->y, p521.Gy, p521.nwords);
    G->is_at_infinity = 0;

    p521.scalar_mul(R, G, x, &p521);
    
    ASSERT_EQ(0, test_equality(R->x, "0x140C8EDCA57108CE3F7E7A240DDD3AD74D81E2DE62451FC1D558FDC79269ADACD1C2526EEEEF32F8C0432A9D56E2B4A8A732891C37C9B96641A9254CCFE5DC3E2BA"));
    ASSERT_EQ(0, test_equality(R->y, "0xCD42A03AD1EB93C532FC8A54683998FF86FEC61F85F8E15B4ACD5B696498F211506D340091019900C918BD8088E0352E9742EA9E2B55983ECAA343E424B8113428"));
    
    //ASSERT_EQ(1, xint_is_prime(p521.n));

    END_TEST(ecc);
}

TEST(ecc, plain_scalar_multiply)
{
    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "7A1A7E52797FC8CAAA435D2A4DACE39158504BF204FBE19F14DBB427FAEE50AE", 16);

    xint_ecc_point_t R;
    xint_point_init(R);

    xint_ecc_point_t G;
    xint_point_init(G);
    G->is_at_infinity = 0;
    G->x->size = p256.nwords;
    G->x->data = p256.Gx;
    G->y->size = p256.nwords;
    G->y->data = p256.Gy;

    STAMP_VARS();
    __disable_irq();
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    xint_ecc_mul_scalar_plain(R, G, x, &p256);
    STAMP_AFTER();
    __enable_irq();
    printf("xint_ecc_mul_scalar_plain : %u\n", STAMP_DIFF());
    
    ASSERT_EQ(0, test_equality(R->x, "0x2B42F576D07F4165FF65D1F3B1500F81E44C316F1F0B3EF57325B69ACA46104F"));
    ASSERT_EQ(0, test_equality(R->y, "0x3CE76603264661EA2F602DF7B4510BBC9ED939233C553EA5F42FB3F1338174B5"));

    END_TEST(ecc);
}

TEST(ecc, jacobian_scalar_multiply)
{
    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "7A1A7E52797FC8CAAA435D2A4DACE39158504BF204FBE19F14DBB427FAEE50AE", 16);

    xint_ecc_point_t R;
    xint_point_init(R);

    xint_ecc_point_t G;
    xint_point_init(G);
    G->is_at_infinity = 0;
    G->x->size = p256.nwords;
    G->x->data = p256.Gx;
    G->y->size = p256.nwords;
    G->y->data = p256.Gy;

    STAMP_VARS();
    __disable_irq();
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    xint_ecc_mul_scalar_jacobian(R, G, x, &p256);
    STAMP_AFTER();
    __enable_irq();
    printf("xint_ecc_mul_scalar_jacobian : %u\n", STAMP_DIFF());
    
    ASSERT_EQ(0, test_equality(R->x, "0x2B42F576D07F4165FF65D1F3B1500F81E44C316F1F0B3EF57325B69ACA46104F"));
    ASSERT_EQ(0, test_equality(R->y, "0x3CE76603264661EA2F602DF7B4510BBC9ED939233C553EA5F42FB3F1338174B5"));

    END_TEST(ecc);
}

TEST(ecc, nist)
{
    xint_t priv = XINT_INIT_VAL;
    xint_ecc_point_t pub;
    xint_point_init(pub);
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_p_192[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &p192);
        int resx = test_equality(pub->x, key_pairs_p_192[i].Qx);
        int resy = test_equality(pub->y, key_pairs_p_192[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_p_224[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &p224);
        int resx = test_equality(pub->x, key_pairs_p_224[i].Qx);
        int resy = test_equality(pub->y, key_pairs_p_224[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_p_256[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &p256);
        int resx = test_equality(pub->x, key_pairs_p_256[i].Qx);
        int resy = test_equality(pub->y, key_pairs_p_256[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_p_384[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &p384);
        int resx = test_equality(pub->x, key_pairs_p_384[i].Qx);
        int resy = test_equality(pub->y, key_pairs_p_384[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_p_521[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &p521);
        int resx = test_equality(pub->x, key_pairs_p_521[i].Qx);
        int resy = test_equality(pub->y, key_pairs_p_521[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_k_163[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &k163);
        int resx = test_equality(pub->x, key_pairs_k_163[i].Qx);
        int resy = test_equality(pub->y, key_pairs_k_163[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_k_233[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &k233);
        int resx = test_equality(pub->x, key_pairs_k_233[i].Qx);
        int resy = test_equality(pub->y, key_pairs_k_233[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_k_283[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &k283);
        int resx = test_equality(pub->x, key_pairs_k_283[i].Qx);
        int resy = test_equality(pub->y, key_pairs_k_283[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_k_409[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &k409);
        int resx = test_equality(pub->x, key_pairs_k_409[i].Qx);
        int resy = test_equality(pub->y, key_pairs_k_409[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_k_571[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &k571);
        int resx = test_equality(pub->x, key_pairs_k_571[i].Qx);
        int resy = test_equality(pub->y, key_pairs_k_571[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_b_163[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &b163);
        int resx = test_equality(pub->x, key_pairs_b_163[i].Qx);
        int resy = test_equality(pub->y, key_pairs_b_163[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_b_233[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &b233);
        int resx = test_equality(pub->x, key_pairs_b_233[i].Qx);
        int resy = test_equality(pub->y, key_pairs_b_233[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_b_283[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &b283);
        int resx = test_equality(pub->x, key_pairs_b_283[i].Qx);
        int resy = test_equality(pub->y, key_pairs_b_283[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_b_409[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &b409);
        int resx = test_equality(pub->x, key_pairs_b_409[i].Qx);
        int resy = test_equality(pub->y, key_pairs_b_409[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    for (int i=0; i<10; ++i)
    {
        xint_assign_str(priv, key_pairs_b_571[i].d, 0);
        xint_ecc_get_public_key(pub, priv, &b571);
        int resx = test_equality(pub->x, key_pairs_b_571[i].Qx);
        int resy = test_equality(pub->y, key_pairs_b_571[i].Qy);
        ASSERT_EQ(0, resx);
        ASSERT_EQ(0, resy);
    }
    xint_delete(priv);
    xint_point_delete(pub);

    END_TEST(ecc);
}

int test_ecc(void)
{
    CALL_TEST(ecc, hmac1);
    CALL_TEST(ecc, hmac2);
    CALL_TEST(ecc, k_generation);
    CALL_TEST(ecc, rfc_6979);
    CALL_TEST(ecc, gen_det_k);
    CALL_TEST(ecc, simple_gcd_and_inverse);
    CALL_TEST(ecc, curve_p224);
    CALL_TEST(ecc, curve_p256);
    CALL_TEST(ecc, curve_p384);
    CALL_TEST(ecc, curve_p521);
    CALL_TEST(ecc, plain_scalar_multiply);
    CALL_TEST(ecc, jacobian_scalar_multiply);
    CALL_TEST(ecc, nist);

    END_TEST_GROUP(ecc);
}
