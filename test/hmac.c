
#include "hmac.h"

#include <string.h>
#include <stdlib.h>

struct hmac_sha256_ctx *hmac_sha256_new(const uint8_t *key, size_t keylen)
{
    struct hmac_sha256_ctx *ctx = (struct hmac_sha256_ctx *)malloc(sizeof(struct hmac_sha256_ctx));
    ctx->sha256_ctx = sha256_new();
    hmac_sha256_reset(ctx, key, keylen);
    return ctx;
}

void hmac_sha256_reset(struct hmac_sha256_ctx *ctx, const uint8_t *key, size_t keylen)
{
    memset(ctx->blk_len_key, 0, 64);
    
    if (keylen > 64)
    {
        sha256_reset(ctx->sha256_ctx);
        sha256_append(ctx->sha256_ctx, key, keylen);
        sha256_finalise(ctx->sha256_ctx, ctx->blk_len_key);
    }
    else if (keylen < 64)
    {
        memset(ctx->blk_len_key, 0, 64);
        memcpy(ctx->blk_len_key, key, keylen);
    }
    
    for (int i=0; i<64; ++i)
    {
        ctx->o_key_pad[i] = ctx->blk_len_key[i] ^ 0x5c;
        ctx->i_key_pad[i] = ctx->blk_len_key[i] ^ 0x36;
    }
    
    sha256_reset(ctx->sha256_ctx);
    sha256_append(ctx->sha256_ctx, ctx->i_key_pad, 64);
}

void hmac_sha256_append(struct hmac_sha256_ctx *ctx, const uint8_t *msg, size_t n)
{
    sha256_append(ctx->sha256_ctx, msg, n);
}

void hmac_sha256_append_ch(struct hmac_sha256_ctx *ctx, uint8_t ch)
{
    sha256_append_ch(ctx->sha256_ctx, ch);
}

void hmac_sha256_finalise(struct hmac_sha256_ctx *ctx, uint8_t digest[32])
{
    sha256_finalise(ctx->sha256_ctx, ctx->int_digest);
    sha256_reset(ctx->sha256_ctx);
    sha256_append(ctx->sha256_ctx, ctx->o_key_pad, 64);
    sha256_append(ctx->sha256_ctx, ctx->int_digest, 32);
    sha256_finalise(ctx->sha256_ctx, digest);
}

void hmac_sha256_delete(struct hmac_sha256_ctx *ctx)
{
    sha256_delete(ctx->sha256_ctx);
    free(ctx);
}

void hmac_calc(uint8_t digest[32], const uint8_t *key, size_t keylen, const uint8_t *msg, size_t msglen)
{
    struct hmac_sha256_ctx *ctx = hmac_sha256_new(key, keylen);
    hmac_sha256_reset(ctx, key, keylen);
    hmac_sha256_append(ctx, msg, msglen);
    hmac_sha256_finalise(ctx, digest);
    hmac_sha256_delete(ctx);
}
