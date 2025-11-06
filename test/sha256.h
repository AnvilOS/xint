
#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

struct sha256_ctx
{
    uint32_t hv[8];
    uint8_t buf[64];
    uint8_t *pbuf;
    size_t msg_len;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sha256_ctx *sha256_new(void);
void sha256_reset(struct sha256_ctx *ctx);
void sha256_append(struct sha256_ctx *ctx, const uint8_t *msg, size_t n);
void sha256_append_ch(struct sha256_ctx *ctx, uint8_t ch);
void sha256_finalise(struct sha256_ctx *ctx, uint8_t digest[32]);
void sha256_delete(struct sha256_ctx *ctx);

void sha256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SHA256_H
