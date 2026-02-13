
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
#include "hmac_drbg.h"

#include "time_stamp.h"

#include "sha256.h"
#include "hmac.h"

#include <string.h>
#include <ctype.h>

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

uint8_t *hex2bin(char *hex_msg)
{
    // Convert msg to a binary array
    long len = strlen(hex_msg);
    uint8_t *bin_msg = (uint8_t *)malloc(len/2);
    for (int i=0; i<len/2; ++i)
    {
        uint8_t c[2];
        c[0] = hex_msg[2*i];
        c[1] = hex_msg[2*i+1];
        for (int j=0; j<2; ++j)
        {
            if ('0' <= c[j] && c[j] <= '9')
            {
                c[j] -= '0';
            }
            else if ('a' <= tolower(c[j]) && tolower(c[j]) <= 'f')
            {
                c[j] = tolower(c[j]) - 'a' + 10;
            }
        }
        bin_msg[i] = c[0] << 4 | c[1];
    }
    return bin_msg;
}

TEST_GROUP(ecc);

TEST(ecc, sha)
{
    static const uint8_t expected_digest_224[28] =
    {
        0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9,
        0x47, 0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4,
        0x15, 0xa2, 0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a,
        0xc5, 0xb3, 0xe4, 0x2f
    };
    
    uint8_t digest[64];
    uint8_t msg[] = { };
    sha224_calc(digest, msg, 0);
    ASSERT_EQ(0, memcmp(digest, expected_digest_224, sizeof(expected_digest_224)/sizeof(expected_digest_224[0])));
    
    static const uint8_t expected_digest_256[32] =
    {
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
        0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
        0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
    };

    sha256_calc(digest, msg, 0);
    ASSERT_EQ(0, memcmp(digest, expected_digest_256, sizeof(expected_digest_256)/sizeof(expected_digest_256[0])));

    static const uint8_t expected_digest_384[48] =
    {
        0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38,
        0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
        0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
        0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
        0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
        0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b
    };
    
    sha384_calc(digest, msg, 0);
    ASSERT_EQ(0, memcmp(digest, expected_digest_384, sizeof(expected_digest_384)/sizeof(expected_digest_384[0])));

    static const uint8_t expected_digest_512[64] =
    {
        0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
        0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
        0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
        0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
        0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
        0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
        0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
        0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e
    };

    sha512_calc(digest, msg, 0);
    ASSERT_EQ(0, memcmp(digest, expected_digest_512, sizeof(expected_digest_512)/sizeof(expected_digest_512[0])));

    static const uint8_t expected_digest_512_224[28] =
    {
        0x6e, 0xd0, 0xdd, 0x02, 0x80, 0x6f, 0xa8, 0x9e,
        0x25, 0xde, 0x06, 0x0c, 0x19, 0xd3, 0xac, 0x86,
        0xca, 0xbb, 0x87, 0xd6, 0xa0, 0xdd, 0xd0, 0x5c,
        0x33, 0x3b, 0x84, 0xf4
    };
    
    sha512_224_calc(digest, msg, 0);
    ASSERT_EQ(0, memcmp(digest, expected_digest_512_224, sizeof(expected_digest_512_224)/sizeof(expected_digest_512_224[0])));

    static const uint8_t expected_digest_512_256[32] =
    {
        0xc6, 0x72, 0xb8, 0xd1, 0xef, 0x56, 0xed, 0x28,
        0xab, 0x87, 0xc3, 0x62, 0x2c, 0x51, 0x14, 0x06,
        0x9b, 0xdd, 0x3a, 0xd7, 0xb8, 0xf9, 0x73, 0x74,
        0x98, 0xd0, 0xc0, 0x1e, 0xce, 0xf0, 0x96, 0x7a
    };
    
    sha512_256_calc(digest, msg, 0);
    ASSERT_EQ(0, memcmp(digest, expected_digest_512_256, sizeof(expected_digest_512_256)/sizeof(expected_digest_512_256[0])));

    END_TEST(ecc);
}

TEST(ecc, sha_monte)
{
    char *seed_hex = "6d1e72ad03ddeb5de891e572e2396f8da015d899ef0e79503152d6010a3fe691";
    long seed_len = strlen(seed_hex)/2;
    unsigned char *seed = hex2bin(seed_hex);
    uint8_t md[3][seed_len];
    uint8_t digest[seed_len];

    for (int j=0; j<100; ++j)
    {
        uint8_t concat[3*seed_len];
        memcpy(md[0], seed, seed_len);
        memcpy(md[1], seed, seed_len);
        memcpy(md[2], seed, seed_len);
        for (int i=3; i<1003; ++i)
        {
            memcpy(concat, md[0], seed_len);
            memcpy(concat+seed_len, md[1], seed_len);
            memcpy(concat+2*seed_len, md[2], seed_len);
            sha256_calc(digest, concat, 3*seed_len);
            memcpy(md[0], md[1], seed_len);
            memcpy(md[1], md[2], seed_len);
            memcpy(md[2], digest, seed_len);
        }
        memcpy(seed, digest, seed_len);
   }
   
    char *expected_hex = "6a912ba4188391a78e6f13d88ed2d14e13afce9db6f7dcbf4a48c24f3db02778";
    
    unsigned char *expected = hex2bin(expected_hex);
    
    ASSERT_EQ(0, memcmp(expected, digest, seed_len));

    END_TEST(ecc);
}

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


TEST(ecc, hmac_drbg)
{
//    [SHA-256]
//    [PredictionResistance = False]
//    [EntropyInputLen = 256]
//    [NonceLen = 128]
//    [PersonalizationStringLen = 0]
//    [AdditionalInputLen = 0]
//    [ReturnedBitsLen = 1024]
//
//    COUNT = 0
//    EntropyInput = ca851911349384bffe89de1cbdc46e6831e44d34a4fb935ee285dd14b71a7488
//    Nonce = 659ba96c601dc69fc902940805ec0ca8
//    PersonalizationString =
//    ** INSTANTIATE:
//        V   = e75855f93b971ac468d200992e211960202d53cf08852ef86772d6490bfb53f9
//        Key = 302a4aba78412ab36940f4be7b940a0c728542b8b81d95b801a57b3797f9dd6e
//    AdditionalInput =
//    ** GENERATE (FIRST CALL):
//        V   = bfbdcf455d5c82acafc59f339ce57126ff70b67aef910fa25db7617818faeafe
//        Key = 911bf7cbda4387a172a1a3daf6c9fa8e17c4bfef69cc7eff1341e7eef88d2811
//    AdditionalInput =
//    ReturnedBits = e528e9abf2dece54d47c7e75e5fe302149f817ea9fb4bee6f4199697d04d5b89d54fbb978a15b5c443c9ec21036d2460b6f73ebad0dc2aba6e624abf07745bc107694bb7547bb0995f70de25d6b29e2d3011bb19d27676c07162c8b5ccde0668961df86803482cb37ed6d5c0bb8d50cf1f50d476aa0458bdaba806f48be9dcb8
//    ** GENERATE (SECOND CALL):
//        V   = 6b94e773e3469353a1ca8face76b238c5919d62a150a7dfc589ffa11c30b5b94
//        Key = 6dd2cd5b1edba4b620d195ce26ad6845b063211d11e591432de37a3ad793f66c

    char entropy_hex[] = "ca851911349384bffe89de1cbdc46e6831e44d34a4fb935ee285dd14b71a7488";
    char nonce_hex[] = "659ba96c601dc69fc902940805ec0ca8";
    int entropy_len = strlen(entropy_hex)/2;
    unsigned char *entropy = hex2bin(entropy_hex);
    int nonce_len = strlen(nonce_hex)/2;
    unsigned char *nonce = hex2bin(nonce_hex);

    unsigned char seed[100];
    memset(seed, 0, sizeof(seed));
    int seedlen = entropy_len + nonce_len;
    memcpy(seed, entropy, entropy_len);
    memcpy(seed+entropy_len, nonce, nonce_len);
    struct hmac_drbg_ctx *drbg = hmac_drbg_instantiate(seed, seedlen);
//    uint8_t block[32];
//    hmac_calc(block, drbg->K, drbg->outlen, drbg->V, drbg->outlen);

    
    hmac_drbg_generate(drbg, 1024, NULL, 0);
    hmac_drbg_generate(drbg, 1024, NULL, 0);
    hmac_drbg_generate(drbg, 1024, NULL, 0);
    hmac_drbg_generate(drbg, 1024, NULL, 0);
//    hmac_drbg_update(drbg, tmp_vec, entropy_len+nonce_len);
    //    xint_to_buf(seed, rolen, v_int);
    //    xint_to_buf(seed + rolen, rolen, h1_int);
    //    int seedlen = rolen + rolen;
    //
    //
    //    // a.  The hash is ready, just convert to an xint

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
    STAMP_BEFORE();
    xint_ecc_get_public_key(pub, priv, &p256);
    STAMP_AFTER();
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
    STAMP_BEFORE();
    xint_ecc_sign_det(signature, h1, hlen, priv, &p256);
    STAMP_AFTER();
    printf("xint_ecc_sign_det : %u\n", STAMP_DIFF());
    printf("Calculated\n");
    xint_print_hex("sigR", signature->r);
    xint_print_hex("sigS", signature->s);
    printf("\n");
    ASSERT_EQ(0, test_equality(signature->r, r_exp));
    ASSERT_EQ(0, test_equality(signature->s, s_exp));
    
    //STAMP_VARS();
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    ASSERT_EQ(1, xint_ecc_verify(signature, h1, hlen, pub, &p256));
    STAMP_AFTER();
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
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    p256.scalar_mul(R, G, x, &p256);
    STAMP_AFTER();
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
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    xint_ecc_mul_scalar_plain(R, G, x, &p256);
    STAMP_AFTER();
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
    STAMP_BEFORE();
    //for (int i=0; i<512; ++i)
    xint_ecc_mul_scalar_jacobian(R, G, x, &p256);
    STAMP_AFTER();
    printf("xint_ecc_mul_scalar_jacobian : %u\n", STAMP_DIFF());
    
    ASSERT_EQ(0, test_equality(R->x, "0x2B42F576D07F4165FF65D1F3B1500F81E44C316F1F0B3EF57325B69ACA46104F"));
    ASSERT_EQ(0, test_equality(R->y, "0x3CE76603264661EA2F602DF7B4510BBC9ED939233C553EA5F42FB3F1338174B5"));

    END_TEST(ecc);
}

TEST(ecc, nist_key_pair)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct key_pair *keys;
    } vec_data[] =
    {
        { "p192", &p192, key_pairs_p_192, },
        { "p224", &p224, key_pairs_p_224, },
        { "p256", &p256, key_pairs_p_256, },
        { "p384", &p384, key_pairs_p_384, },
        { "p521", &p521, key_pairs_p_521, },
        { "k163", &k163, key_pairs_k_163, },
        { "k233", &k233, key_pairs_k_233, },
        { "k283", &k283, key_pairs_k_283, },
        { "k409", &k409, key_pairs_k_409, },
        { "k571", &k571, key_pairs_k_571, },
        { "b163", &b163, key_pairs_b_163, },
        { "b233", &b233, key_pairs_b_233, },
        { "b283", &b283, key_pairs_b_283, },
        { "b409", &b409, key_pairs_b_409, },
        { "b571", &b571, key_pairs_b_571, },
    };
    
    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        STAMP_VARS();
        xint_t priv = XINT_INIT_VAL;
        xint_ecc_point_t pub;
        xint_point_init(pub);
        STAMP_BEFORE();
        for (int i=0; i<10; ++i)
        {
            xint_assign_str(priv, vec_data[j].keys[i].d, 0);
            xint_ecc_get_public_key(pub, priv, vec_data[j].curve);
            int resx = test_equality(pub->x, vec_data[j].keys[i].Qx);
            int resy = test_equality(pub->y, vec_data[j].keys[i].Qy);
            ASSERT_EQ(0, resx);
            ASSERT_EQ(0, resy);
        }
        STAMP_AFTER();
        xint_delete(priv);
        xint_point_delete(pub);
    }

    END_TEST(ecc);
}

TEST(ecc, nist_pkv)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct pkv *data;
    } vec_data[] =
    {
        { "p192", &p192, pkv_p_192, },
        { "p224", &p224, pkv_p_224, },
        { "p256", &p256, pkv_p_256, },
        { "p384", &p384, pkv_p_384, },
        { "p521", &p521, pkv_p_521, },
        { "k163", &k163, pkv_k_163, },
        { "k233", &k233, pkv_k_233, },
        { "k283", &k283, pkv_k_283, },
        { "k409", &k409, pkv_k_409, },
        { "k571", &k571, pkv_k_571, },
        { "b163", &b163, pkv_b_163, },
        { "b233", &b233, pkv_b_233, },
        { "b283", &b283, pkv_b_283, },
        { "b409", &b409, pkv_b_409, },
        { "b571", &b571, pkv_b_571, },
    };

    xint_t priv = XINT_INIT_VAL;
    xint_ecc_point_t pub;
    xint_point_init(pub);

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        STAMP_VARS();
        STAMP_BEFORE();
        for (int i=0; i<10; ++i)
        {
            xint_assign_str(pub->x, vec_data[j].data[i].Qx, 0);
            xint_assign_str(pub->y, vec_data[j].data[i].Qy, 0);
            int good_point = xint_ecc_verify_public_key(pub, vec_data[j].curve);
            if (vec_data[j].data[i].result[0] == 'F')
            {
                ASSERT_EQ(0, good_point);
            }
            else
            {
                ASSERT_EQ(1, good_point);
            }
        }
        STAMP_AFTER();
        //printf("xint_ecc_verify_public_key %s : %lu\n", vec_data[j].name, STAMP_DIFF());
    }
    xint_delete(priv);
    xint_point_delete(pub);
    
    END_TEST(ecc);
}

TEST(ecc, nist_gen_224)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_gen *data;
    } vec_data[] =
    {
        { "p224", &p224, sig_gen_p_224_sha_224, },
        { "p256", &p256, sig_gen_p_256_sha_224, },
        { "p384", &p384, sig_gen_p_384_sha_224, },
        { "p521", &p521, sig_gen_p_521_sha_224, },
        { "k233", &k233, sig_gen_k_233_sha_224, },
        { "k283", &k283, sig_gen_k_283_sha_224, },
        { "k409", &k409, sig_gen_k_409_sha_224, },
        { "k571", &k571, sig_gen_k_571_sha_224, },
        { "b233", &b233, sig_gen_b_233_sha_224, },
        { "b283", &b283, sig_gen_b_283_sha_224, },
        { "b409", &b409, sig_gen_b_409_sha_224, },
        { "b571", &b571, sig_gen_b_571_sha_224, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        xint_t priv = XINT_INIT_VAL;
        xint_t k = XINT_INIT_VAL;
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(priv, vec_data[j].data[i].d, 0);
            xint_assign_str(k, vec_data[j].data[i].k, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[32];
            sha224_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            STAMP_BEFORE();
            xint_ecc_sign(sig, h1, 28, priv, k, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_sign %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            int resx = test_equality(sig->r, vec_data[j].data[i].R);
            int resy = test_equality(sig->s, vec_data[j].data[i].S);
            ASSERT_EQ(0, resx);
            ASSERT_EQ(0, resy);
        }
        xint_delete(sig->r);
        xint_delete(sig->s);
    }
    
    END_TEST(ecc);
}


TEST(ecc, nist_gen_256)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_gen *data;
    } vec_data[] =
    {
        { "p224", &p224, sig_gen_p_224_sha_256, },
        { "p256", &p256, sig_gen_p_256_sha_256, },
        { "p384", &p384, sig_gen_p_384_sha_256, },
        { "p521", &p521, sig_gen_p_521_sha_256, },
        { "k233", &k233, sig_gen_k_233_sha_256, },
        { "k283", &k283, sig_gen_k_283_sha_256, },
        { "k409", &k409, sig_gen_k_409_sha_256, },
        { "k571", &k571, sig_gen_k_571_sha_256, },
        { "b233", &b233, sig_gen_b_233_sha_256, },
        { "b283", &b283, sig_gen_b_283_sha_256, },
        { "b409", &b409, sig_gen_b_409_sha_256, },
        { "b571", &b571, sig_gen_b_571_sha_256, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        xint_t priv = XINT_INIT_VAL;
        xint_t k = XINT_INIT_VAL;
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(priv, vec_data[j].data[i].d, 0);
            xint_assign_str(k, vec_data[j].data[i].k, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[32];
            sha256_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            STAMP_BEFORE();
            xint_ecc_sign(sig, h1, 32, priv, k, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_sign %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            int resx = test_equality(sig->r, vec_data[j].data[i].R);
            int resy = test_equality(sig->s, vec_data[j].data[i].S);
            ASSERT_EQ(0, resx);
            ASSERT_EQ(0, resy);
        }
        xint_delete(sig->r);
        xint_delete(sig->s);
    }
    
    END_TEST(ecc);
}

    
TEST(ecc, nist_gen_384)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_gen *data;
    } vec_data[] =
    {
        { "p224", &p224, sig_gen_p_224_sha_384, },
        { "p256", &p256, sig_gen_p_256_sha_384, },
        { "p384", &p384, sig_gen_p_384_sha_384, },
        { "p521", &p521, sig_gen_p_521_sha_384, },
        { "k233", &k233, sig_gen_k_233_sha_384, },
        { "k283", &k283, sig_gen_k_283_sha_384, },
        { "k409", &k409, sig_gen_k_409_sha_384, },
        { "k571", &k571, sig_gen_k_571_sha_384, },
        { "b233", &b233, sig_gen_b_233_sha_384, },
        { "b283", &b283, sig_gen_b_283_sha_384, },
        { "b409", &b409, sig_gen_b_409_sha_384, },
        { "b571", &b571, sig_gen_b_571_sha_384, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        xint_t priv = XINT_INIT_VAL;
        xint_t k = XINT_INIT_VAL;
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(priv, vec_data[j].data[i].d, 0);
            xint_assign_str(k, vec_data[j].data[i].k, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[48];
            sha384_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            STAMP_BEFORE();
            xint_ecc_sign(sig, h1, 48, priv, k, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_sign %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            int resx = test_equality(sig->r, vec_data[j].data[i].R);
            int resy = test_equality(sig->s, vec_data[j].data[i].S);
            ASSERT_EQ(0, resx);
            ASSERT_EQ(0, resy);
        }
        xint_delete(sig->r);
        xint_delete(sig->s);
    }
    
    END_TEST(ecc);
}

    
TEST(ecc, nist_gen_512)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_gen *data;
    } vec_data[] =
    {
        { "p224", &p224, sig_gen_p_224_sha_512, },
        { "p256", &p256, sig_gen_p_256_sha_512, },
        { "p384", &p384, sig_gen_p_384_sha_512, },
        { "p521", &p521, sig_gen_p_521_sha_512, },
        { "k233", &k233, sig_gen_k_233_sha_512, },
        { "k283", &k283, sig_gen_k_283_sha_512, },
        { "k409", &k409, sig_gen_k_409_sha_512, },
        { "k571", &k571, sig_gen_k_571_sha_512, },
        { "b233", &b233, sig_gen_b_233_sha_512, },
        { "b283", &b283, sig_gen_b_283_sha_512, },
        { "b409", &b409, sig_gen_b_409_sha_512, },
        { "b571", &b571, sig_gen_b_571_sha_512, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        xint_t priv = XINT_INIT_VAL;
        xint_t k = XINT_INIT_VAL;
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(priv, vec_data[j].data[i].d, 0);
            xint_assign_str(k, vec_data[j].data[i].k, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[64];
            sha512_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            STAMP_BEFORE();
            xint_ecc_sign(sig, h1, 64, priv, k, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_sign %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            int resx = test_equality(sig->r, vec_data[j].data[i].R);
            int resy = test_equality(sig->s, vec_data[j].data[i].S);
            ASSERT_EQ(0, resx);
            ASSERT_EQ(0, resy);
        }
        xint_delete(sig->r);
        xint_delete(sig->s);
    }
    
    END_TEST(ecc);
}

    
TEST(ecc, nist_ver_224)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_ver *data;
    } vec_data[] =
    {
        { "p192", &p192, sig_ver_p_192_sha_224, },
        { "p224", &p224, sig_ver_p_224_sha_224, },
        { "p256", &p256, sig_ver_p_256_sha_224, },
        { "p384", &p384, sig_ver_p_384_sha_224, },
        { "p521", &p521, sig_ver_p_521_sha_224, },
        { "k163", &k163, sig_ver_k_163_sha_224, },
        { "k233", &k233, sig_ver_k_233_sha_224, },
        { "k283", &k283, sig_ver_k_283_sha_224, },
        { "k409", &k409, sig_ver_k_409_sha_224, },
        { "k571", &k571, sig_ver_k_571_sha_224, },
        { "b163", &b163, sig_ver_b_163_sha_224, },
        { "b233", &b233, sig_ver_b_233_sha_224, },
        { "b283", &b283, sig_ver_b_283_sha_224, },
        { "b409", &b409, sig_ver_b_409_sha_224, },
        { "b571", &b571, sig_ver_b_571_sha_224, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_point_t pub;
        xint_point_init(pub);
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(pub->x, vec_data[j].data[i].Qx, 0);
            xint_assign_str(pub->y, vec_data[j].data[i].Qy, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[32];
            sha224_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            xint_assign_str(sig->r, vec_data[j].data[i].R, 0);
            xint_assign_str(sig->s, vec_data[j].data[i].S, 0);
            STAMP_BEFORE();
            int verified = xint_ecc_verify(sig, h1, 28, pub, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_verify %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            if (vec_data[j].data[i].result[0] == 'F')
            {
                ASSERT_EQ(0, verified);
            }
            else
            {
                ASSERT_EQ(1, verified);
            }
        }
        xint_point_delete(pub);
        xint_delete(sig->r);
        xint_delete(sig->s);
    }

    END_TEST(ecc);
}

TEST(ecc, nist_ver_256)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_ver *data;
    } vec_data[] =
    {
        { "p192", &p192, sig_ver_p_192_sha_256, },
        { "p224", &p224, sig_ver_p_224_sha_256, },
        { "p256", &p256, sig_ver_p_256_sha_256, },
        { "p384", &p384, sig_ver_p_384_sha_256, },
        { "p521", &p521, sig_ver_p_521_sha_256, },
        { "k163", &k163, sig_ver_k_163_sha_256, },
        { "k233", &k233, sig_ver_k_233_sha_256, },
        { "k283", &k283, sig_ver_k_283_sha_256, },
        { "k409", &k409, sig_ver_k_409_sha_256, },
        { "k571", &k571, sig_ver_k_571_sha_256, },
        { "b163", &b163, sig_ver_b_163_sha_256, },
        { "b233", &b233, sig_ver_b_233_sha_256, },
        { "b283", &b283, sig_ver_b_283_sha_256, },
        { "b409", &b409, sig_ver_b_409_sha_256, },
        { "b571", &b571, sig_ver_b_571_sha_256, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_point_t pub;
        xint_point_init(pub);
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(pub->x, vec_data[j].data[i].Qx, 0);
            xint_assign_str(pub->y, vec_data[j].data[i].Qy, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[32];
            sha256_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            xint_assign_str(sig->r, vec_data[j].data[i].R, 0);
            xint_assign_str(sig->s, vec_data[j].data[i].S, 0);
            STAMP_BEFORE();
            int verified = xint_ecc_verify(sig, h1, 32, pub, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_verify %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            if (vec_data[j].data[i].result[0] == 'F')
            {
                ASSERT_EQ(0, verified);
            }
            else
            {
                ASSERT_EQ(1, verified);
            }
        }
        xint_point_delete(pub);
        xint_delete(sig->r);
        xint_delete(sig->s);
    }

    END_TEST(ecc);
}

TEST(ecc, nist_ver_384)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_ver *data;
    } vec_data[] =
    {
        { "p192", &p192, sig_ver_p_192_sha_384, },
        { "p224", &p224, sig_ver_p_224_sha_384, },
        { "p256", &p256, sig_ver_p_256_sha_384, },
        { "p384", &p384, sig_ver_p_384_sha_384, },
        { "p521", &p521, sig_ver_p_521_sha_384, },
        { "k163", &k163, sig_ver_k_163_sha_384, },
        { "k233", &k233, sig_ver_k_233_sha_384, },
        { "k283", &k283, sig_ver_k_283_sha_384, },
        { "k409", &k409, sig_ver_k_409_sha_384, },
        { "k571", &k571, sig_ver_k_571_sha_384, },
        { "b163", &b163, sig_ver_b_163_sha_384, },
        { "b233", &b233, sig_ver_b_233_sha_384, },
        { "b283", &b283, sig_ver_b_283_sha_384, },
        { "b409", &b409, sig_ver_b_409_sha_384, },
        { "b571", &b571, sig_ver_b_571_sha_384, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_point_t pub;
        xint_point_init(pub);
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(pub->x, vec_data[j].data[i].Qx, 0);
            xint_assign_str(pub->y, vec_data[j].data[i].Qy, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[48];
            sha384_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            xint_assign_str(sig->r, vec_data[j].data[i].R, 0);
            xint_assign_str(sig->s, vec_data[j].data[i].S, 0);
            STAMP_BEFORE();
            int verified = xint_ecc_verify(sig, h1, 48, pub, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_verify %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            if (vec_data[j].data[i].result[0] == 'F')
            {
                ASSERT_EQ(0, verified);
            }
            else
            {
                ASSERT_EQ(1, verified);
            }
        }
        xint_point_delete(pub);
        xint_delete(sig->r);
        xint_delete(sig->s);
    }

    END_TEST(ecc);
}

TEST(ecc, nist_ver_512)
{
    static const struct vec
    {
        char *name;
        const xint_ecc_curve_t *curve;
        const struct sig_ver *data;
    } vec_data[] =
    {
        { "p192", &p192, sig_ver_p_192_sha_512, },
        { "p224", &p224, sig_ver_p_224_sha_512, },
        { "p256", &p256, sig_ver_p_256_sha_512, },
        { "p384", &p384, sig_ver_p_384_sha_512, },
        { "p521", &p521, sig_ver_p_521_sha_512, },
        { "k163", &k163, sig_ver_k_163_sha_512, },
        { "k233", &k233, sig_ver_k_233_sha_512, },
        { "k283", &k283, sig_ver_k_283_sha_512, },
        { "k409", &k409, sig_ver_k_409_sha_512, },
        { "k571", &k571, sig_ver_k_571_sha_512, },
        { "b163", &b163, sig_ver_b_163_sha_512, },
        { "b233", &b233, sig_ver_b_233_sha_512, },
        { "b283", &b283, sig_ver_b_283_sha_512, },
        { "b409", &b409, sig_ver_b_409_sha_512, },
        { "b571", &b571, sig_ver_b_571_sha_512, },
    };

    for (int j=0; j<sizeof(vec_data)/sizeof(vec_data[0]); ++j)
    {
        xint_ecc_point_t pub;
        xint_point_init(pub);
        xint_ecc_sig_t sig;
        xint_init(sig->r);
        xint_init(sig->s);
        STAMP_VARS();
        for (int i=0; i<15; ++i)
        {
            xint_assign_str(pub->x, vec_data[j].data[i].Qx, 0);
            xint_assign_str(pub->y, vec_data[j].data[i].Qy, 0);
            uint8_t *msg = hex2bin(vec_data[j].data[i].msg+2);
            uint8_t h1[64];
            sha512_calc(h1, msg, (strlen(vec_data[j].data[i].msg)-2)/2);
            free(msg);
            xint_assign_str(sig->r, vec_data[j].data[i].R, 0);
            xint_assign_str(sig->s, vec_data[j].data[i].S, 0);
            STAMP_BEFORE();
            int verified = xint_ecc_verify(sig, h1, 64, pub, vec_data[j].curve);
            STAMP_AFTER();
            //printf("xint_ecc_verify %s : %lu\n", vec_data[j].name, STAMP_DIFF());
            if (vec_data[j].data[i].result[0] == 'F')
            {
                ASSERT_EQ(0, verified);
            }
            else
            {
                ASSERT_EQ(1, verified);
            }
        }
        xint_point_delete(pub);
        xint_delete(sig->r);
        xint_delete(sig->s);
    }

    END_TEST(ecc);
}

int test_ecc(void)
{
    CALL_TEST(ecc, sha);
    CALL_TEST(ecc, sha_monte);
    CALL_TEST(ecc, hmac1);
    CALL_TEST(ecc, hmac2);
    CALL_TEST(ecc, hmac_drbg);
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
    CALL_TEST(ecc, nist_key_pair);
    CALL_TEST(ecc, nist_pkv);
    CALL_TEST(ecc, nist_gen_224);
    CALL_TEST(ecc, nist_gen_256);
    CALL_TEST(ecc, nist_gen_384);
    CALL_TEST(ecc, nist_gen_512);
    CALL_TEST(ecc, nist_ver_224);
    CALL_TEST(ecc, nist_ver_256);
    CALL_TEST(ecc, nist_ver_384);
    CALL_TEST(ecc, nist_ver_512);

    END_TEST_GROUP(ecc);
}
