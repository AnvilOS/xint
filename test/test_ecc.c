
#include "test_ecc.h"
#include "test_harness.h"

#include "xint.h"
#include "xint_ecc.h"
#include "xint_io.h"
#include "xint_bitwise.h"
#include "xint_algorithms.h"

#include "sha256.h"
#include "hmac.h"

#include <string.h>

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

    // public key:
    // Ux = 0x79AEE090DB05EC252D5CB4452F356BE198A4FF96F
    // Uy = 0x782E29634DDC9A31EF40386E896BAA18B53AFA5A3

    // Input message = "sample"
    char *m = "sample";
        
    // a.  Process m through the hash function H
    int hlen = 32;
    uint8_t h1[hlen];
    sha256_calc(h1, (uint8_t *)m, strlen(m));

    xint_t h1_int = XINT_INIT_VAL;
    xint_from_bin(h1_int, h1, 32);
    xint_rshift(h1_int, h1_int, xint_size(h1_int) * XWORD_BITS - 163);

    xint_t v_int = XINT_INIT_VAL;
    xint_assign_str(v_int, x, 0);

    xint_t z2_int = XINT_INIT_VAL;
    xint_mod(z2_int, h1_int, q_int);
    
    int rolen = (qlen + 7) >> 3;
    int rlen = rolen * 8;
        
    // b. V = 0x01 0x01 0x01 ... 0x01 vlen = 8*ceil(hlen/8)
    uint8_t V[(hlen / 8 ) * 8];
    memset(V, 0x01, sizeof(V));
        
    // c. K = 0x00 0x00 0x00 ... 0x00
    uint8_t K[(hlen / 8 ) * 8];
    memset(K, 0x00, sizeof(K));

    // d. K = HMAC_K(V || 0x00 || int2octets(x) || bits2octets(h1))
    uint8_t tmp_vec[100];
    memset(tmp_vec, 0, sizeof(tmp_vec));
    memcpy(tmp_vec, V, sizeof(V));
    tmp_vec[32] = 0x00;
    xint_to_buf(tmp_vec + 33, 21, v_int);
    xint_to_buf(tmp_vec + 54, 21, z2_int);
    hmac_calc(K, K, sizeof(K), tmp_vec, 75);

    // e. V = HMAC_K(V)
    hmac_calc(V, K, sizeof(K), V, sizeof(V));

    // f. K = HMAC_K(V || 0x01 || int2octets(x) || bits2octets(h1))
    memset(tmp_vec, 0, sizeof(tmp_vec));
    memcpy(tmp_vec, V, 32);
    tmp_vec[32] = 0x01;
    xint_to_buf(tmp_vec + 33, 21, v_int);
    xint_to_buf(tmp_vec + 54, 21, z2_int);
    hmac_calc(K, K, sizeof(K), tmp_vec, 75);

    // g. V = HMAC_K(V)
    hmac_calc(V, K, sizeof(K), V, sizeof(V));

    // h.
    xint_t k = XINT_INIT_VAL;
    while (1)
    {
        // 1.  Set T to the empty sequence length tlen
        uint8_t T[100];
        int tlen = 0;
        // 2.  While tlen < qlen, do the following:
        while (tlen < qlen)
        {
            // V = HMAC_K(V)
            // T = T || V
            hmac_calc(V, K, sizeof(K), V, sizeof(V));
            memcpy(T + tlen, V, sizeof(V));
            tlen += sizeof(V) * 8;
        }
            
        // 3.  Compute:
        // k = bits2int(T)
        xint_from_bin(k, T, 32);
        xint_rshift(k, k, xint_size(k) * XWORD_BITS - 163);
        
        // If 1 <= k <= q-1 done
        if (xint_cmp_ulong(k, 1) >= 0 && xint_cmp(k, q_int) < 0)
        {
            break;
        }
        else
        {
            // else compute:
            // K = HMAC_K(V || 0x00)
            // V = HMAC_K(V)
            uint8_t tmpV[33];
            memcpy(tmpV, V, sizeof(V));
            tmpV[32] = 0x00;
            hmac_calc(K, K, sizeof(K), tmpV, sizeof(tmpV));
            hmac_calc(V, K, sizeof(K), V, sizeof(V));
        }
    }
    
    char *k_str = xint_to_string(k, 16);
    ASSERT_EQ(0, strcasecmp(k_str, "23AF4074C90A02B3FE61D286D5C87F425E6BDD81B"));
    free(k_str);
    
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

int test_equality(const xint_t x, char *p)
{
    xint_t y = XINT_INIT_VAL;
    xint_assign_str(y, p, 0);
    int ret = xint_cmp(x, y);
    xint_delete(y);
    return ret;
}

TEST(ecc, curve_p224)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality(p224.p, "0xffffffffffffffffffffffffffffffff000000000000000000000001"));
    ASSERT_EQ(0, test_equality(p224.a, "0xfffffffffffffffffffffffffffffffefffffffffffffffffffffffe"));
    ASSERT_EQ(0, test_equality(p224.b, "0xb4050a850c04b3abf54132565044b0b7d7bfd8ba270b39432355ffb4"));
    ASSERT_EQ(0, test_equality(p224.Gx, "0xb70e0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21"));
    ASSERT_EQ(0, test_equality(p224.Gy, "0xbd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34"));
    ASSERT_EQ(0, test_equality(p224.n, "0xffffffffffffffffffffffffffff16a2e0b8f03e13dd29455c5c2a3d"));
    ASSERT_EQ(0, test_equality(p224.h, "0x1"));
    
    xint_ecc_point_t G;
    xint_point_init(G);
    G->is_at_infinity = 0;
    xint_copy(G->x, p224.Gx);
    xint_copy(G->y, p224.Gy);

    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "A548803B79DF17C40CDE3FF0E36D025143BCBBA146EC32908EB84937", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_mul_scalar_mont_ladder(R, G, x, p224);
    
    xint_t Rx_exp = XINT_INIT_VAL;
    xint_assign_str(Rx_exp, "C3A3F5B82712532004C6F6D1DB672F55D931C3409EA1216D0BE77380", 16);
    
    xint_t Ry_exp = XINT_INIT_VAL;
    xint_assign_str(Ry_exp, "9BF4978CA8C8A8DF855A74C6905A5A3947ACFF772FCE436D48341D46", 16);

    ASSERT_EQ(0, xint_cmp(Rx_exp, R->x));
    ASSERT_EQ(0, xint_cmp(Ry_exp, R->y));

    END_TEST(ecc);
}

TEST(ecc, curve_p256)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality(p256.p, "0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff"));
    ASSERT_EQ(0, test_equality(p256.a, "0xffffffff00000001000000000000000000000000fffffffffffffffffffffffc"));
    ASSERT_EQ(0, test_equality(p256.b, "0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b"));
    ASSERT_EQ(0, test_equality(p256.Gx, "0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296"));
    ASSERT_EQ(0, test_equality(p256.Gy, "0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5"));
    ASSERT_EQ(0, test_equality(p256.n, "0xffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551"));
    ASSERT_EQ(0, test_equality(p256.h, "0x1"));

    xint_ecc_point_t G;
    xint_point_init(G);
    G->is_at_infinity = 0;
    xint_copy(G->x, p256.Gx);
    xint_copy(G->y, p256.Gy);

    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "7A1A7E52797FC8CAAA435D2A4DACE39158504BF204FBE19F14DBB427FAEE50AE", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_mul_scalar_mont_ladder(R, G, x, p256);
    
    xint_t Rx_exp = XINT_INIT_VAL;
    xint_assign_str(Rx_exp, "2B42F576D07F4165FF65D1F3B1500F81E44C316F1F0B3EF57325B69ACA46104F", 16);
    
    xint_t Ry_exp = XINT_INIT_VAL;
    xint_assign_str(Ry_exp, "3CE76603264661EA2F602DF7B4510BBC9ED939233C553EA5F42FB3F1338174B5", 16);

    ASSERT_EQ(0, xint_cmp(Rx_exp, R->x));
    ASSERT_EQ(0, xint_cmp(Ry_exp, R->y));

    END_TEST(ecc);
}

TEST(ecc, curve_p384)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality(p384.p, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff"));
    ASSERT_EQ(0, test_equality(p384.a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc"));
    ASSERT_EQ(0, test_equality(p384.b, "0xb3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef"));
    ASSERT_EQ(0, test_equality(p384.Gx, "0xaa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7"));
    ASSERT_EQ(0, test_equality(p384.Gy, "0x3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f"));
    ASSERT_EQ(0, test_equality(p384.n, "0xffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973"));
    ASSERT_EQ(0, test_equality(p384.h, "0x1"));

    xint_ecc_point_t G;
    xint_point_init(G);
    G->is_at_infinity = 0;
    xint_copy(G->x, p384.Gx);
    xint_copy(G->y, p384.Gy);

    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "2E44EF1F8C0BEA8394E3DDA81EC6A7842A459B534701749E2ED95F054F0137680878E0749FC43F85EDCAE06CC2F43FEF", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_mul_scalar_mont_ladder(R, G, x, p384);
    
    xint_t Rx_exp = XINT_INIT_VAL;
    xint_assign_str(Rx_exp, "30EA514FC0D38D8208756F068113C7CADA9F66A3B40EA3B313D040D9B57DD41A332795D02CC7D507FCEF9FAF01A27088", 16);
    
    xint_t Ry_exp = XINT_INIT_VAL;
    xint_assign_str(Ry_exp, "C04E32465D14C50CBC3BCB88EA20F95B10616663FC62A8DCDB48D3006327EA7CA104F6F9294C66EA2487BD50357010C6", 16);

    ASSERT_EQ(0, xint_cmp(Rx_exp, R->x));
    ASSERT_EQ(0, xint_cmp(Ry_exp, R->y));

    END_TEST(ecc);
}

TEST(ecc, curve_p521)
{
    // Check the curve parameters
    ASSERT_EQ(0, test_equality(p521.p, "0x01ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
    ASSERT_EQ(0, test_equality(p521.a, "0x01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc"));
    ASSERT_EQ(0, test_equality(p521.b, "0x0051953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00"));
    ASSERT_EQ(0, test_equality(p521.Gx, "0x00c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66"));
    ASSERT_EQ(0, test_equality(p521.Gy, "0x011839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650"));
    ASSERT_EQ(0, test_equality(p521.n, "0x01fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9b8899c47aebb6fb71e91386409"));
    ASSERT_EQ(0, test_equality(p521.h, "0x1"));

    xint_ecc_point_t G;
    xint_point_init(G);
    G->is_at_infinity = 0;
    xint_copy(G->x, p521.Gx);
    xint_copy(G->y, p521.Gy);

    xint_t x = XINT_INIT_VAL;
    xint_assign_str(x, "C91E2349EF6CA22D2DE39DD51819B6AAD922D3AECDEAB452BA172F7D63E370CECD70575F597C09A174BA76BED05A48E562BE0625336D16B8703147A6A231D6BF", 16);

    xint_ecc_point_t R;
    xint_point_init(R);
    
    xint_ecc_mul_scalar_mont_ladder(R, G, x, p521);
    
    xint_t Rx_exp = XINT_INIT_VAL;
    xint_assign_str(Rx_exp, "140C8EDCA57108CE3F7E7A240DDD3AD74D81E2DE62451FC1D558FDC79269ADACD1C2526EEEEF32F8C0432A9D56E2B4A8A732891C37C9B96641A9254CCFE5DC3E2BA", 16);
    
    xint_t Ry_exp = XINT_INIT_VAL;
    xint_assign_str(Ry_exp, "CD42A03AD1EB93C532FC8A54683998FF86FEC61F85F8E15B4ACD5B696498F211506D340091019900C918BD8088E0352E9742EA9E2B55983ECAA343E424B8113428", 16);

    ASSERT_EQ(0, xint_cmp(Rx_exp, R->x));
    ASSERT_EQ(0, xint_cmp(Ry_exp, R->y));

    END_TEST(ecc);
}

int test_ecc(void)
{
    CALL_TEST(ecc, hmac1);
    CALL_TEST(ecc, hmac2);
    CALL_TEST(ecc, k_generation);
    CALL_TEST(ecc, simple_gcd_and_inverse);
    CALL_TEST(ecc, curve_p224);
    CALL_TEST(ecc, curve_p256);
    CALL_TEST(ecc, curve_p384);
    CALL_TEST(ecc, curve_p521);

    END_TEST_GROUP(ecc);
}
