
#include "hmac_drbg.h"

#include <string.h>
#include <stdlib.h>

static void hmac_drbg_update(struct hmac_drbg_ctx *ctx, unsigned char *data, int datalen);

struct hmac_drbg_ctx *hmac_drbg_instantiate(hashfunc_id id, unsigned char *seed, int seedlen)
{
    struct hmac_ctx *hmac_ctx = hmac_new(id, seed, seedlen);
    int outlen = hmac_ctx->sha_ctx->digest_len;
    struct hmac_drbg_ctx *ctx = (struct hmac_drbg_ctx *)malloc(sizeof(struct hmac_drbg_ctx) + 2 * outlen);
    ctx->hmac_ctx = hmac_ctx;
    ctx->outlen = outlen;
    // 1. seed_material = entropy_input || nonce || personalization_string.
    // 2. Key = 0x00 00...00.
    ctx->K = (unsigned char *)(ctx + 1);
    memset(ctx->K, 0, ctx->outlen);
    // 3. V = 0x01 01...01.
    ctx->V = ctx->K + ctx->outlen;
    memset(ctx->V, 1, ctx->outlen);
    // 4. (Key, V) = HMAC_DRBG_Update (seed_material, Key, V).
    hmac_drbg_update(ctx, seed, seedlen);
    // 5. reseed_counter = 1.
    ctx->reseed_counter = 1;
    
    ctx->prediction_resistance = 0;
    return ctx;
}

void hmac_drbg_reseed(struct hmac_drbg_ctx *ctx, unsigned char *seed, int seedlen)
{
    // 1. seed_material = entropy_input || additional_input.
    
    // 2. (Key, V) = HMAC_DRBG_Update (seed_material, Key, V).
    hmac_drbg_update(ctx, seed, seedlen);

    // 3. reseed_counter = 1.
    ctx->reseed_counter = 1;

    // 4. Return (V, Key, reseed_counter).
}

int hmac_drbg_generate(struct hmac_drbg_ctx *ctx, unsigned char *bits, int nbits_requested, unsigned char *data, int datalen)
{
    // 1. If reseed_counter > reseed_interval, then return an indication that a reseed is required.
    
    // 2. If additional_input ≠ Null, then (Key, V) = HMAC_DRBG_Update (additional_input, Key, V).
    if (data != NULL && datalen != 0)
    {
        hmac_drbg_update(ctx, data, datalen);
    }
    
    // 3. temp = Null.
    int bits_avail = 0;
    long offs = 0;
    int bytes_requested = (nbits_requested - 1) / 8 + 1;

    // 4. While (len (temp) < requested_number_of_bits) do:
    while (bits_avail < nbits_requested)
    {
        // 4.1 V = HMAC (Key, V).
        hmac_reset(ctx->hmac_ctx, ctx->K, ctx->outlen);
        hmac_append(ctx->hmac_ctx, ctx->V, ctx->outlen);
        hmac_finalise(ctx->hmac_ctx, ctx->V);

        long remaining_space = bytes_requested - offs;
        bits_avail += ctx->outlen*8;
        
        // 4.2 temp = temp || V.
        if (remaining_space >= ctx->outlen)
        {
            memcpy(bits+offs, ctx->V, ctx->outlen);
            offs += ctx->outlen;
        }
        else
        {
            memcpy(bits+offs, ctx->V, remaining_space);
            offs += remaining_space;
        }
            
    }
   
    // 5. returned_bits = leftmost (temp, requested_number_of_bits).
    
    // 6. (Key, V) = HMAC_DRBG_Update (additional_input, Key, V).
    hmac_drbg_update(ctx, NULL, 0);

    // 7. reseed_counter = reseed_counter + 1.
    ++ctx->reseed_counter;
    
    // 8. Return (SUCCESS, returned_bits, Key, V, reseed_counter).
    return 0;
}

void hmac_drbg_uninstantiate(struct hmac_drbg_ctx *ctx)
{
    hmac_delete(ctx->hmac_ctx);
    memset(ctx->V, 0, ctx->outlen);
    memset(ctx->K, 0, ctx->outlen);
    free(ctx);
}

static void hmac_drbg_update(struct hmac_drbg_ctx *ctx, unsigned char *data, int datalen)
{
    // 1. K = HMAC (K, V || 0x00 || provided_data).
    hmac_reset(ctx->hmac_ctx, ctx->K, ctx->outlen);
    hmac_append(ctx->hmac_ctx, ctx->V, ctx->outlen);
    hmac_append_ch(ctx->hmac_ctx, 0);
    hmac_append(ctx->hmac_ctx, data, datalen);
    hmac_finalise(ctx->hmac_ctx, ctx->K);

    // 2. V = HMAC (K, V).
    hmac_reset(ctx->hmac_ctx, ctx->K, ctx->outlen);
    hmac_append(ctx->hmac_ctx, ctx->V, ctx->outlen);
    hmac_finalise(ctx->hmac_ctx, ctx->V);

    // 3. If (provided_data = Null), then return K and V.
    if (data == NULL || datalen == 0)
    {
        return;
    }

    // 4. K = HMAC (K, V || 0x01 || provided_data).
    hmac_reset(ctx->hmac_ctx, ctx->K, ctx->outlen);
    hmac_append(ctx->hmac_ctx, ctx->V, ctx->outlen);
    hmac_append_ch(ctx->hmac_ctx, 1);
    hmac_append(ctx->hmac_ctx, data, datalen);
    hmac_finalise(ctx->hmac_ctx, ctx->K);

    // 5. V = HMAC(K,V).
    hmac_reset(ctx->hmac_ctx, ctx->K, ctx->outlen);
    hmac_append(ctx->hmac_ctx, ctx->V, ctx->outlen);
    hmac_finalise(ctx->hmac_ctx, ctx->V);
    
    // 6. Return (K, V).
}
