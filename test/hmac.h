
#ifndef HMAC_H
#define HMAC_H

#include "sha256.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
class Hmac
{
public:
    Hmac(const uint8_t *key, size_t keylen) { reset(key, keylen); }
    virtual ~Hmac() {}
    
    void reset(const uint8_t *key, size_t keylen);
    void append(const uint8_t *msg, size_t n);
    void append(uint8_t ch);
    void finalise(uint8_t digest[32]);

private:
    Sha256 sha256;
    uint8_t blk_len_key[64];
    uint8_t o_key_pad[64];
    uint8_t i_key_pad[64];
    uint8_t int_digest[32];
};

extern "C"
{
#endif // __cplusplus

void hmac_calc(uint8_t digest[32], const uint8_t *key, size_t keylen, const uint8_t *msg, size_t msglen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HMAC_H
