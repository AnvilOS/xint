
#include "hmac.h"

#include "sha256.h"

#include <string.h>

void Hmac::reset()
{
}

void Hmac::process_chunk()
{
}

void Hmac::append(const uint8_t *msg, size_t n)
{
}

void Hmac::append(uint8_t ch)
{
}

void Hmac::finalise(uint8_t digest[32])
{
}

void hmac_calc(uint8_t digest[32], const uint8_t *key, size_t keylen, const uint8_t *msg, size_t msglen)
{
    Sha256 sha256;
    uint8_t blk_len_key[64];
    uint8_t o_key_pad[64];
    uint8_t i_key_pad[64];

    memset(blk_len_key, 0, 64);
    
    if (keylen > 64)
    {
        sha256.reset();
        sha256.append(key, keylen);
        sha256.finalise(blk_len_key);
    }
    else if (keylen < 64)
    {
        memset(blk_len_key, 0, 64);
        memcpy(blk_len_key, key, keylen);
    }
    
    for (int i=0; i<64; ++i)
    {
        o_key_pad[i] = blk_len_key[i] ^ 0x5c;
        i_key_pad[i] = blk_len_key[i] ^ 0x36;
    }
    
    sha256.reset();
    sha256.append(i_key_pad, 64);
    sha256.append(msg, msglen);
    sha256.finalise(digest);

    sha256.reset();
    sha256.append(o_key_pad, 64);
    sha256.append(digest, 32);
    sha256.finalise(digest);
}
