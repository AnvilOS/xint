
#ifndef HMAC_DRBG_H
#define HMAC_DRBG_H

struct hmac_drbg_ctx
{
    int state;
};

void hmac_drbg_instantiate(struct hmac_drbg_ctx *ctx);
void hmac_drbg_update(struct hmac_drbg_ctx *ctx);
void hmac_drbg_reseed(struct hmac_drbg_ctx *ctx);
void hmac_drbg_generate(struct hmac_drbg_ctx *ctx);

#endif // HMAC_DRBG_H
