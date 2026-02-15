
#ifndef HMAC_DRBG_H
#define HMAC_DRBG_H

#include "hmac.h"

struct hmac_drbg_ctx
{
    struct hmac_ctx *hmac_ctx;
    int outlen;
    int reseed_counter;
    int security_strength;
    int prediction_resistance;
    unsigned char *V;
    unsigned char *K;
};

struct hmac_drbg_ctx *hmac_drbg_instantiate(hashfunc_id id, unsigned char *seed, int seedlen);
void hmac_drbg_reseed(struct hmac_drbg_ctx *ctx, unsigned char *seed, int seedlen);
int hmac_drbg_generate(struct hmac_drbg_ctx *ctx, unsigned char *bits, int nbits_requested, unsigned char *data, int datalen);
void hmac_drbg_uninstantiate(struct hmac_drbg_ctx *ctx);

#endif // HMAC_DRBG_H
