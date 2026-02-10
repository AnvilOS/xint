
#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    sha224,
    sha256,
    sha384,
    sha512,
} hashfunc_id;

struct sha256_ctx
{
    uint32_t hv[8];
    uint8_t buf[64];
    uint8_t *pbuf;
    size_t msg_len;
};

struct sha512_ctx
{
    uint64_t hv[8];
    uint8_t buf[128];
    uint8_t *pbuf;
    size_t msg_len;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct sha256_ctx *sha256_new(void);
void sha224_reset(struct sha256_ctx *ctx);
void sha256_reset(struct sha256_ctx *ctx);
void sha256_append(struct sha256_ctx *ctx, const uint8_t *msg, size_t n);
void sha256_append_ch(struct sha256_ctx *ctx, uint8_t ch);
void sha224_finalise(struct sha256_ctx *ctx, uint8_t digest[28]);
void sha256_finalise(struct sha256_ctx *ctx, uint8_t digest[32]);
void sha256_delete(struct sha256_ctx *ctx);

void sha224_calc(uint8_t digest[28], const uint8_t *src, size_t n_bytes);
void sha256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes);

struct sha512_ctx *sha512_new(void);
void sha384_reset(struct sha512_ctx *ctx);
void sha512_reset(struct sha512_ctx *ctx);
void sha512_224_reset(struct sha512_ctx *ctx);
void sha512_256_reset(struct sha512_ctx *ctx);
void sha512_append(struct sha512_ctx *ctx, const uint8_t *msg, size_t n);
void sha512_append_ch(struct sha512_ctx *ctx, uint8_t ch);
void sha384_finalise(struct sha512_ctx *ctx, uint8_t digest[48]);
void sha512_finalise(struct sha512_ctx *ctx, uint8_t digest[64]);
void sha512_224_finalise(struct sha512_ctx *ctx, uint8_t digest[64]);
void sha512_256_finalise(struct sha512_ctx *ctx, uint8_t digest[64]);
void sha512_delete(struct sha512_ctx *ctx);

void sha384_calc(uint8_t digest[48], const uint8_t *src, size_t n_bytes);
void sha512_calc(uint8_t digest[64], const uint8_t *src, size_t n_bytes);
void sha512_224_calc(uint8_t digest[28], const uint8_t *src, size_t n_bytes);
void sha512_256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SHA256_H
