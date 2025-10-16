
#include "test_ecc.h"
#include "test_harness.h"

#include "xint.h"
#include "xint_ecc.h"
#include "xint_io.h"

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

TEST(ecc, simple)
{
    ASSERT_EQ(1, 1);

    END_TEST(ecc);
}

int test_ecc(void)
{
    CALL_TEST(ecc, hmac);
    CALL_TEST(ecc, simple);
    
    END_TEST_GROUP(ecc);
}
