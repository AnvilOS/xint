
#ifndef HMAC_DRBG_H
#define HMAC_DRBG_H

#include "hmac.h"

struct hmac_drbg_ctx
{
    unsigned char V[100];
    unsigned char K[100];
    int outlen;
    int reseed_counter;
    int security_strength;
    int prediction_resistance;
    struct hmac_sha256_ctx *hmac_ctx;
};

struct hmac_drbg_ctx *hmac_drbg_instantiate(unsigned char *seed, int seedlen);
void hmac_drbg_update(struct hmac_drbg_ctx *ctx, unsigned char *data, int datalen);
void hmac_drbg_reseed(struct hmac_drbg_ctx *ctx);
void hmac_drbg_generate(struct hmac_drbg_ctx *ctx, int nbits, unsigned char *data, int datalen);

#endif // HMAC_DRBG_H
