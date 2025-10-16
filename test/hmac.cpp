
#include "hmac.h"

#include <string.h>

void Hmac::reset(const uint8_t *key, size_t keylen)
{
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
}

void Hmac::append(const uint8_t *msg, size_t n)
{
    sha256.append(msg, n);
}

void Hmac::append(uint8_t ch)
{
    append(&ch, 1);
}

void Hmac::finalise(uint8_t digest[32])
{
    sha256.finalise(int_digest);
    sha256.reset();
    sha256.append(o_key_pad, 64);
    sha256.append(int_digest, 32);
    sha256.finalise(digest);
}

void hmac_calc(uint8_t digest[32], const uint8_t *key, size_t keylen, const uint8_t *msg, size_t msglen)
{
    Hmac hmac(key, keylen);
    hmac.append(msg, msglen);
    hmac.finalise(digest);
}
