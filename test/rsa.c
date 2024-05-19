
#include "rsa.h"

#include "sha256.h"
#include "xint.h"
#include "xint_io.h"
#include "xint_exp.h"
#include "test_harness.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// DER encodings
// MD2:            (0x)30 20 30 0c 06 08 2a 86 48 86 f7 0d 02 02 05 00 04 10 || H.
// MD5:            (0x)30 20 30 0c 06 08 2a 86 48 86 f7 0d 02 05 05 00 04 10 || H.
// SHA-1:          (0x)30 21 30 09 06 05 2b 0e 03 02 1a 05 00 04 14 || H.
// SHA-224:        (0x)30 2d 30 0d 06 09 60 86 48 01 65 03 04 02 04 05 00 04 1c || H.
// SHA-256:        (0x)30 31 30 0d 06 09 60 86 48 01 65 03 04 02 01 05 00 04 20 || H.
// SHA-384:        (0x)30 41 30 0d 06 09 60 86 48 01 65 03 04 02 02 05 00 04 30 || H.
// SHA-512:        (0x)30 51 30 0d 06 09 60 86 48 01 65 03 04 02 03 05 00 04 40 || H.
// SHA-512/224:    (0x)30 2d 30 0d 06 09 60 86 48 01 65 03 04 02 05 05 00 04 1c || H.
// SHA-512/256:    (0x)30 31 30 0d 06 09 60 86 48 01 65 03 04 02 06 05 00 04 20 || H.

const unsigned char der_256[] = { 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };

TEST_GROUP(rsa)

TEST(rsa, simple)
{
    xint_t n = XINT_INIT_VAL;
    xint_from_string(n, "cea80475324c1dc8347827818da58bac069d3419c614a6ea1ac6a3b510dcd72cc516954905e9fef908d45e13006adf27d467a7d83c111d1a5df15ef293771aefb920032a5bb989f8e4f5e1b05093d3f130f984c07a772a3683f4dc6fb28a96815b32123ccdd13954f19d5b8b24a103e771a34c328755c65ed64e1924ffd04d30b2142cc262f6e0048fef6dbc652f21479ea1c4b1d66d28f4d46ef7185e390cbfa2e02380582f3188bb94ebbf05d31487a09aff01fcbb4cd4bfd1f0a833b38c11813c84360bb53c7d4481031c40bad8713bb6b835cb08098ed15ba31ee4ba728a8c8e10f7294e1b4163b7aee57277bfd881a6f9d43e02c6925aa3a043fb7fb78d");
    
    xint_t e = XINT_INIT_VAL;
    xint_from_string(e, "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000260445");
    
    xint_t d = XINT_INIT_VAL;
    xint_from_string(d, "0997634c477c1a039d44c810b2aaa3c7862b0b88d3708272e1e15f66fc9389709f8a11f3ea6a5af7effa2d01c189c50f0d5bcbe3fa272e56cfc4a4e1d388a9dcd65df8628902556c8b6bb6a641709b5a35dd2622c73d4640bfa1359d0e76e1f219f8e33eb9bd0b59ec198eb2fccaae0346bd8b401e12e3c67cb629569c185a2e0f35a2f741644c1cca5ebb139d77a89a2953fc5e30048c0e619f07c8d21d1e56b8af07193d0fdf3f49cd49f2ef3138b5138862f1470bd2d16e34a2b9e7777a6c8c8d4cb94b4e8b5d616cd5393753e7b0f31cc7da559ba8e98d888914e334773baf498ad88d9631eb5fe32e53a4145bf0ba548bf2b0a50c63f67b14e398a34b0d");

    const char *msg = "5af283b1b76ab2a695d794c23b35ca7371fc779e92ebf589e304c7f923d8cf976304c19818fcd89d6f07c8d8e08bf371068bdf28ae6ee83b2e02328af8c0e2f96e528e16f852f1fc5455e4772e288a68f159ca6bdcf902b858a1f94789b3163823e2d0717ff56689eec7d0e54d93f520d96e1eb04515abc70ae90578ff38d31b";
    
    // Convert msg to a binary array
    long len = strlen(msg);
    uint8_t *bin_msg = (uint8_t *)malloc(len);
    for (int i=0; i<len/2; ++i)
    {
        uint8_t c[2];
        c[0] = msg[2*i];
        c[1] = msg[2*i+1];
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

    // Calculate the sha256
    uint8_t digest[32];
    sha256_calc(digest, bin_msg, len/2);
    free(bin_msg);

    // Add a pkcs#1 header
    int emlen = 256;
    int hlen = len / 2;
    uint8_t em[256];
    int tlen = sizeof(der_256) + 32;
    int pos = 0;
    em[pos++] = 0x00;
    em[pos++] = 0x01;
    int nff = emlen - tlen - 3;
    for(int i=0; i<nff; ++i)
    {
        em[pos++] = 0xff;
    }
    em[pos++] = 0x00;
    for (int i=0; i<sizeof(der_256); ++i)
    {
        em[pos++] = der_256[i];
    }
    for (int i=0; i<32; ++i)
    {
        em[pos++] = digest[i];
    }
    
    // Put em into an xint
    xint_t m = XINT_INIT_VAL;
    xint_from_bin(m, em, 256);

    xint_t c_calc = XINT_INIT_VAL;
    // c_calc = m ^ d mod n
    xint_mod_exp(c_calc, m, d, n);

    xint_t S = XINT_INIT_VAL;
    xint_from_string(S, "6b8be97d9e518a2ede746ff4a7d91a84a1fc665b52f154a927650db6e7348c69f8c8881f7bcf9b1a6d3366eed30c3aed4e93c203c43f5528a45de791895747ade9c5fa5eee81427edee02082147aa311712a6ad5fb1732e93b3d6cd23ffd46a0b3caf62a8b69957cc68ae39f9993c1a779599cdda949bdaababb77f248fcfeaa44059be5459fb9b899278e929528ee130facd53372ecbc42f3e8de2998425860406440f248d817432de687112e504d734028e6c5620fa282ca07647006cf0a2ff83e19a916554cc61810c2e855305db4e5cf893a6a96767365794556ff033359084d7e38a8456e68e21155b76151314a29875feee09557161cbc654541e89e42");
    
    
    ASSERT_EQ(0, xint_cmp(c_calc, S));

    END_TEST(rsa);
}

int test_rsa(void)
{
    CALL_TEST(rsa, simple);
    END_TEST_GROUP(rsa);
}



