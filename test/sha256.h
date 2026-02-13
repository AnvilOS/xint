
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
    sha512_224,
    sha512_256,
} hashfunc_id;

// XXX: instead of having the id in the ctx we should have
// parms like digest_len, h reload values etc.
struct sha_ctx
{
    hashfunc_id id;
    int digest_len;
    uint8_t *pbuf;
    int buf_len;
    const void *phv_reload;
    void (*process_chunk)(struct sha_ctx *ctx, const uint8_t *pbuf);
    unsigned long msg_len;
    int size_size;
    uint8_t *buf;
    void *phv;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

size_t sha_mem_required(hashfunc_id id);
void sha_ctx_init(struct sha_ctx *ctx, hashfunc_id id);
struct sha_ctx *sha_new(hashfunc_id id);
void sha_reset(struct sha_ctx *ctx);
void sha_append(struct sha_ctx *ctx, const uint8_t *msg, size_t n);
void sha_append_ch(struct sha_ctx *ctx, uint8_t ch);
void sha_finalise(struct sha_ctx *ctx, uint8_t digest[32]);
void sha_delete(struct sha_ctx *ctx);

void sha224_calc(uint8_t digest[28], const uint8_t *src, size_t n_bytes);
void sha256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes);
void sha384_calc(uint8_t digest[48], const uint8_t *src, size_t n_bytes);
void sha512_calc(uint8_t digest[64], const uint8_t *src, size_t n_bytes);
void sha512_224_calc(uint8_t digest[28], const uint8_t *src, size_t n_bytes);
void sha512_256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SHA256_H
