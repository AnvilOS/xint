
#include "hmac.h"

#include <string.h>
#include <stdlib.h>

struct hmac_sha256_ctx *hmac_sha256_new(hashfunc_id id, const uint8_t *key, size_t keylen)
{
    size_t sha_mem_req = sha_mem_required(id);
    size_t blk_len;
    
    switch (id)
    {
        case sha224:
        case sha256:
            blk_len = 64;
            break;
        case sha384:
        case sha512:
        case sha512_224:
        case sha512_256:
            blk_len = 128;
            break;
    }

    // Allocate memory for the HMAC and sha in one go
    struct hmac_sha256_ctx *ctx = (struct hmac_sha256_ctx *)malloc(sizeof(struct hmac_sha256_ctx) + sha_mem_req + 4 * blk_len);
    ctx->block_len = blk_len;
    ctx->sha_ctx = (struct sha_ctx *)(ctx + 1);
    ctx->i_key_pad = (uint8_t *)ctx->sha_ctx + sha_mem_req;
    ctx->o_key_pad = ctx->i_key_pad + blk_len;
    ctx->blk_len_key = ctx->o_key_pad + blk_len;
    ctx->int_digest = ctx->blk_len_key + blk_len;
    sha_ctx_init(ctx->sha_ctx, id);
    hmac_sha256_reset(ctx, key, keylen);
    return ctx;
}

void hmac_sha256_reset(struct hmac_sha256_ctx *ctx, const uint8_t *key, size_t keylen)
{
    if (keylen > ctx->block_len)
    {
        sha_reset(ctx->sha_ctx);
        sha_append(ctx->sha_ctx, key, keylen);
        sha_finalise(ctx->sha_ctx, ctx->blk_len_key);
    }
    else
    {
        memcpy(ctx->blk_len_key, key, keylen);
    }
    
    for (int i=0; i<ctx->block_len; ++i)
    {
        ctx->o_key_pad[i] = ctx->blk_len_key[i] ^ 0x5c;
        ctx->i_key_pad[i] = ctx->blk_len_key[i] ^ 0x36;
    }
    
    sha_reset(ctx->sha_ctx);
    sha_append(ctx->sha_ctx, ctx->i_key_pad, ctx->block_len);
}

void hmac_sha256_append(struct hmac_sha256_ctx *ctx, const uint8_t *msg, size_t n)
{
    sha_append(ctx->sha_ctx, msg, n);
}

void hmac_sha256_append_ch(struct hmac_sha256_ctx *ctx, uint8_t ch)
{
    sha_append_ch(ctx->sha_ctx, ch);
}

void hmac_sha256_finalise(struct hmac_sha256_ctx *ctx, uint8_t digest[32])
{
    sha_finalise(ctx->sha_ctx, ctx->int_digest);
    sha_reset(ctx->sha_ctx);
    sha_append(ctx->sha_ctx, ctx->o_key_pad, 64);
    sha_append(ctx->sha_ctx, ctx->int_digest, 32);
    sha_finalise(ctx->sha_ctx, digest);
}

void hmac_sha256_delete(struct hmac_sha256_ctx *ctx)
{
    free(ctx);
}

void hmac_calc(uint8_t digest[32], const uint8_t *key, size_t keylen, const uint8_t *msg, size_t msglen)
{
    struct hmac_sha256_ctx *ctx = hmac_sha256_new(sha256, key, keylen);
    hmac_sha256_reset(ctx, key, keylen);
    hmac_sha256_append(ctx, msg, msglen);
    hmac_sha256_finalise(ctx, digest);
    hmac_sha256_delete(ctx);
}
