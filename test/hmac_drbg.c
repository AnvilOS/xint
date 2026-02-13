
#include "hmac_drbg.h"

#include <string.h>
#include <stdlib.h>

struct hmac_drbg_ctx *hmac_drbg_instantiate(unsigned char *seed, int seedlen)
{
    struct hmac_drbg_ctx *ctx = (struct hmac_drbg_ctx *)malloc(sizeof(struct hmac_drbg_ctx));
    ctx->hmac_ctx = hmac_new(sha256, seed, seedlen);
    ctx->outlen = 32;
    memset(ctx->K, 0, ctx->outlen);
    memset(ctx->V, 1, ctx->outlen);
    hmac_drbg_update(ctx, seed, seedlen);
    ctx->reseed_counter = 1;
    ctx->prediction_resistance = 0;
    return ctx;
}

void hmac_drbg_update(struct hmac_drbg_ctx *ctx, unsigned char *data, int datalen)
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
    if (data == NULL | datalen == 0)
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

void hmac_drbg_reseed(struct hmac_drbg_ctx *ctx)
{
    
}

void hmac_drbg_generate(struct hmac_drbg_ctx *ctx, int nbits_requested, unsigned char *data, int datalen)
{
    // 1. If reseed_counter > reseed_interval, then return an indication that a reseed is required.
    // 2. If additional_input ≠ Null, then (Key, V) = HMAC_DRBG_Update (additional_input, Key, V).
    // 3. temp = Null.
    
    // 4. While (len (temp) < requested_number_of_bits) do:
    int bits_avail = 0;
    //uint8_t block[32];
    uint8_t temp[130];
    long offs = 0;
    while (bits_avail < nbits_requested)
    {
        // 4.1 V = HMAC (Key, V).
        hmac_reset(ctx->hmac_ctx, ctx->K, ctx->outlen);
        hmac_append(ctx->hmac_ctx, ctx->V, ctx->outlen);
        hmac_finalise(ctx->hmac_ctx, ctx->V);

//        hmac_calc(ctx->V, ctx->K, ctx->outlen, ctx->V, ctx->outlen);
        bits_avail += ctx->outlen*8;

        //memcpy(ctx->V, block, ctx->outlen);
        
        // 4.2 temp = temp || V.
        memcpy(temp+offs, ctx->V, ctx->outlen);
        offs += ctx->outlen;
    }

    
    
    // 5. returned_bits = leftmost (temp, requested_number_of_bits).
    //e528e9abf2dece54d47c7e75e5fe302149f817ea9fb4bee6f4199697d04d5b89d54fbb978a15b5c443c9ec21036d2460b6f73ebad0dc2aba6e624abf07745bc107694bb7547bb0995f70de25d6b29e2d3011bb19d27676c07162c8b5ccde0668961df86803482cb37ed6d5c0bb8d50cf1f50d476aa0458bdaba806f48be9dcb8

    
    // 6. (Key, V) = HMAC_DRBG_Update (additional_input, Key, V).
    hmac_drbg_update(ctx, NULL, 0);

    // 7. reseed_counter = reseed_counter + 1.
    ++ctx->reseed_counter;
    
    // 8. Return (SUCCESS, returned_bits, Key, V, reseed_counter).
}
