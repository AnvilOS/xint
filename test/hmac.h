
#ifndef HMAC_H
#define HMAC_H

#include "sha256.h"

#include <stddef.h>
#include <stdint.h>

struct hmac_sha256_ctx
{
    struct sha_ctx *sha_ctx;
    size_t block_len;
    uint8_t *blk_len_key;
    uint8_t *o_key_pad;
    uint8_t *i_key_pad;
    uint8_t *int_digest;
};

struct hmac_sha256_ctx *hmac_sha256_new(hashfunc_id id, const uint8_t *key, size_t keylen);
void hmac_sha256_reset(struct hmac_sha256_ctx *ctx, const uint8_t *key, size_t keylen);
void hmac_sha256_append(struct hmac_sha256_ctx *ctx, const uint8_t *msg, size_t n);
void hmac_sha256_append_ch(struct hmac_sha256_ctx *ctx, uint8_t ch);
void hmac_sha256_finalise(struct hmac_sha256_ctx *ctx, uint8_t digest[32]);
void hmac_sha256_delete(struct hmac_sha256_ctx *ctx);

void hmac_calc(uint8_t digest[32], const uint8_t *key, size_t keylen, const uint8_t *msg, size_t msglen);

#endif // HMAC_H
