
#include "sha256.h"

#include <string.h>
#include <stdlib.h>

struct sha256_ctx *sha256_new(void)
{
    struct sha256_ctx *ctx = (struct sha256_ctx *)malloc(sizeof(struct sha256_ctx));
    sha256_reset(ctx);
    return ctx;
}

void sha224_reset(struct sha256_ctx *ctx)
{
    // Initialise hash values
    ctx->hv[0] = 0xc1059ed8;
    ctx->hv[1] = 0x367cd507;
    ctx->hv[2] = 0x3070dd17;
    ctx->hv[3] = 0xf70e5939;
    ctx->hv[4] = 0xffc00b31;
    ctx->hv[5] = 0x68581511;
    ctx->hv[6] = 0x64f98fa7;
    ctx->hv[7] = 0xbefa4fa4;
    // Initialise buffer variables
    ctx->pbuf = ctx->buf;
    ctx->msg_len = 0;
}

void sha256_reset(struct sha256_ctx *ctx)
{
    // Initialise hash values
    ctx->hv[0] = 0x6a09e667;
    ctx->hv[1] = 0xbb67ae85;
    ctx->hv[2] = 0x3c6ef372;
    ctx->hv[3] = 0xa54ff53a;
    ctx->hv[4] = 0x510e527f;
    ctx->hv[5] = 0x9b05688c;
    ctx->hv[6] = 0x1f83d9ab;
    ctx->hv[7] = 0x5be0cd19;
    // Initialise buffer variables
    ctx->pbuf = ctx->buf;
    ctx->msg_len = 0;
}

// The array of round constants is fixed
static const uint32_t k32[] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#define byte_swap32(x) ((x)[0]<< 24|(x)[1]<<16|(x)[2]<<8|(x)[3])
#define rightrotate32(x, n) (((x)>>(n))|((x)<<(32-(n))))

void sha256_process_chunk(struct sha256_ctx *ctx)
{
    // Instead of pre-calculating the full 64 message schedule array we calculate
    // each entry as required. Also we only keep the most recent 16 in a
    // cirular fashion and use mod arithmetic to access them
    uint32_t ww[16];
#define w(a) ww[(a)&0xf]
    
    // Initialise working variables to current hash values
    uint32_t a = ctx->hv[0];
    uint32_t b = ctx->hv[1];
    uint32_t c = ctx->hv[2];
    uint32_t d = ctx->hv[3];
    uint32_t e = ctx->hv[4];
    uint32_t f = ctx->hv[5];
    uint32_t g = ctx->hv[6];
    uint32_t h = ctx->hv[7];
    
    for (int i=0; i<64; ++i)
    {
        if (i < 16)
        {
            w(i) = byte_swap32(&ctx->buf[4*i]);
        }
        else
        {
            uint32_t s0 = rightrotate32(w(i-15), 7) ^ rightrotate32(w(i-15), 18) ^ (w(i-15) >> 3);
            uint32_t s1 = rightrotate32(w(i-2), 17) ^ rightrotate32(w(i-2), 19) ^ (w(i-2) >> 10);
            w(i) = w(i-16) + s0 + w(i-7) + s1;
        }
        uint32_t S1 = rightrotate32(e, 6) ^ rightrotate32(e, 11) ^ rightrotate32(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = h + S1 + ch + k32[i] + w(i);
        uint32_t S0 = rightrotate32(a, 2) ^ rightrotate32(a, 13) ^ rightrotate32(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    ctx->hv[0] += a;
    ctx->hv[1] += b;
    ctx->hv[2] += c;
    ctx->hv[3] += d;
    ctx->hv[4] += e;
    ctx->hv[5] += f;
    ctx->hv[6] += g;
    ctx->hv[7] += h;
}

void sha256_append(struct sha256_ctx *ctx, const uint8_t *msg, size_t n)
{
    const uint8_t *p = msg;
    while (n--)
    {
        *ctx->pbuf++ = *p++;
        ++ctx->msg_len;
        if ((ctx->msg_len % 64) == 0)
        {
            sha256_process_chunk(ctx);
            ctx->pbuf = ctx->buf;
        }
    }
}

void sha256_append_ch(struct sha256_ctx *ctx, uint8_t ch)
{
    sha256_append(ctx, &ch, 1);
}

void sha224_finalise(struct sha256_ctx *ctx, uint8_t digest[28])
{
    uint8_t *ptr = (uint8_t*)digest;
    
    uint64_t L = ctx->msg_len * 8;

    // append a single 1 bit
    sha256_append_ch(ctx, 0x80);

    // append k '0' bits where k takes us up to 8 octets before the buf is full
    while ((ctx->msg_len % 64) != 56)
    {
        sha256_append_ch(ctx, 0x00);
    }
    
    // append L - the message size in bits
    for (int i=0; i<8; ++i)
    {
        sha256_append_ch(ctx, L >> 56);
        L <<= 8;
    }
    
    // Produce the digest
    for (int i=0; i<7; ++i)
    {
        *ptr++ = ctx->hv[i] >> 24 & 0xff;
        *ptr++ = ctx->hv[i] >> 16 & 0xff;
        *ptr++ = ctx->hv[i] >> 8 & 0xff;
        *ptr++ = ctx->hv[i] >> 0 & 0xff;
    }
}

void sha256_finalise(struct sha256_ctx *ctx, uint8_t digest[32])
{
    uint8_t *ptr = (uint8_t*)digest;
    
    uint64_t L = ctx->msg_len * 8;

    // append a single 1 bit
    sha256_append_ch(ctx, 0x80);

    // append k '0' bits where k takes us up to 8 octets before the buf is full
    while ((ctx->msg_len % 64) != 56)
    {
        sha256_append_ch(ctx, 0x00);
    }
    
    // append L - the message size in bits
    for (int i=0; i<8; ++i)
    {
        sha256_append_ch(ctx, L >> 56);
        L <<= 8;
    }
    
    // Produce the digest
    for (int i=0; i<8; ++i)
    {
        *ptr++ = ctx->hv[i] >> 24 & 0xff;
        *ptr++ = ctx->hv[i] >> 16 & 0xff;
        *ptr++ = ctx->hv[i] >> 8 & 0xff;
        *ptr++ = ctx->hv[i] >> 0 & 0xff;
    }
}

void sha256_delete(struct sha256_ctx *ctx)
{
    free(ctx);
}

void sha224_calc(uint8_t digest[28], const uint8_t *src, size_t n_bytes)
{
    struct sha256_ctx *ctx = sha256_new();
    sha224_reset(ctx);
    sha256_append(ctx, (const uint8_t*)src, n_bytes);
    sha224_finalise(ctx, digest);
    sha256_delete(ctx);
}

void sha256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes)
{
    struct sha256_ctx *ctx = sha256_new();
    sha256_reset(ctx);
    sha256_append(ctx, (const uint8_t*)src, n_bytes);
    sha256_finalise(ctx, digest);
    sha256_delete(ctx);
}

struct sha512_ctx *sha512_new(void)
{
    struct sha512_ctx *ctx = (struct sha512_ctx *)malloc(sizeof(struct sha512_ctx));
    sha512_reset(ctx);
    return ctx;
}

void sha384_reset(struct sha512_ctx *ctx)
{
    // Initialise hash values
    ctx->hv[0] = 0xcbbb9d5dc1059ed8;
    ctx->hv[1] = 0x629a292a367cd507;
    ctx->hv[2] = 0x9159015a3070dd17;
    ctx->hv[3] = 0x152fecd8f70e5939;
    ctx->hv[4] = 0x67332667ffc00b31;
    ctx->hv[5] = 0x8eb44a8768581511;
    ctx->hv[6] = 0xdb0c2e0d64f98fa7;
    ctx->hv[7] = 0x47b5481dbefa4fa4;
    // Initialise buffer variables
    ctx->pbuf = ctx->buf;
    ctx->msg_len = 0;
}

void sha512_reset(struct sha512_ctx *ctx)
{
    // Initialise hash values
    ctx->hv[0] = 0x6a09e667f3bcc908;
    ctx->hv[1] = 0xbb67ae8584caa73b;
    ctx->hv[2] = 0x3c6ef372fe94f82b;
    ctx->hv[3] = 0xa54ff53a5f1d36f1;
    ctx->hv[4] = 0x510e527fade682d1;
    ctx->hv[5] = 0x9b05688c2b3e6c1f;
    ctx->hv[6] = 0x1f83d9abfb41bd6b;
    ctx->hv[7] = 0x5be0cd19137e2179;
    // Initialise buffer variables
    ctx->pbuf = ctx->buf;
    ctx->msg_len = 0;
}

void sha512_224_reset(struct sha512_ctx *ctx)
{
    // Initialise hash values
    ctx->hv[0] = 0x8c3d37c819544da2;
    ctx->hv[1] = 0x73e1996689dcd4d6;
    ctx->hv[2] = 0x1dfab7ae32ff9c82;
    ctx->hv[3] = 0x679dd514582f9fcf;
    ctx->hv[4] = 0x0f6d2b697bd44da8;
    ctx->hv[5] = 0x77e36f7304C48942;
    ctx->hv[6] = 0x3f9d85a86a1d36C8;
    ctx->hv[7] = 0x1112e6ad91d692a1;
    // Initialise buffer variables
    ctx->pbuf = ctx->buf;
    ctx->msg_len = 0;
}

void sha512_256_reset(struct sha512_ctx *ctx)
{
    // Initialise hash values
    ctx->hv[0] = 0x22312194fc2bf72c;
    ctx->hv[1] = 0x9f555fa3c84c64c2;
    ctx->hv[2] = 0x2393b86b6f53b151;
    ctx->hv[3] = 0x963877195940eabd;
    ctx->hv[4] = 0x96283ee2a88effe3;
    ctx->hv[5] = 0xbe5e1e2553863992;
    ctx->hv[6] = 0x2b0199fc2c85b8aa;
    ctx->hv[7] = 0x0eb72ddC81c52ca2;
    // Initialise buffer variables
    ctx->pbuf = ctx->buf;
    ctx->msg_len = 0;
}

// The array of round constants is fixed
static const uint64_t k64[] =
{
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

#define byte_swap64(x) (          \
    ((uint64_t)((x)[0])<<56) |  \
    ((uint64_t)((x)[1])<<48) |  \
    ((uint64_t)((x)[2])<<40) |  \
    ((uint64_t)((x)[3])<<32) |  \
    ((uint64_t)((x)[4])<<24) |  \
    ((uint64_t)((x)[5])<<16) |  \
    ((uint64_t)((x)[6])<<8)  |  \
    ((uint64_t)((x)[7]))        \
)

#define rightrotate64(x, n) (((x)>>(n))|((x)<<(64-(n))))

void sha512_process_chunk(struct sha512_ctx *ctx)
{
    // Instead of pre-calculating the full 64 message schedule array we calculate
    // each entry as required. Also we only keep the most recent 16 in a
    // cirular fashion and use mod arithmetic to access them
    uint64_t ww[16];
    #define w(a) ww[(a)&0xf]
    
    // Initialise working variables to current hash values
    uint64_t a = ctx->hv[0];
    uint64_t b = ctx->hv[1];
    uint64_t c = ctx->hv[2];
    uint64_t d = ctx->hv[3];
    uint64_t e = ctx->hv[4];
    uint64_t f = ctx->hv[5];
    uint64_t g = ctx->hv[6];
    uint64_t h = ctx->hv[7];
    
    for (int i=0; i<80; ++i)
    {
        if (i < 16)
        {
            w(i) = byte_swap64(&ctx->buf[8*i]);
        }
        else
        {
            uint64_t s0 = rightrotate64(w(i-15), 1) ^ rightrotate64(w(i-15), 8) ^ (w(i-15) >> 7);
            uint64_t s1 = rightrotate64(w(i-2), 19) ^ rightrotate64(w(i-2), 61) ^ (w(i-2) >> 6);
            w(i) = w(i-16) + s0 + w(i-7) + s1;
        }
        uint64_t S1 = rightrotate64(e, 14) ^ rightrotate64(e, 18) ^ rightrotate64(e, 41);
        uint64_t ch = (e & f) ^ (~e & g);
        uint64_t temp1 = h + S1 + ch + k64[i] + w(i);
        uint64_t S0 = rightrotate64(a, 28) ^ rightrotate64(a, 34) ^ rightrotate64(a, 39);
        uint64_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint64_t temp2 = S0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    ctx->hv[0] += a;
    ctx->hv[1] += b;
    ctx->hv[2] += c;
    ctx->hv[3] += d;
    ctx->hv[4] += e;
    ctx->hv[5] += f;
    ctx->hv[6] += g;
    ctx->hv[7] += h;
}

void sha512_append(struct sha512_ctx *ctx, const uint8_t *msg, size_t n)
{
    const uint8_t *p = msg;
    while (n--)
    {
        *ctx->pbuf++ = *p++;
        ++ctx->msg_len;
        if ((ctx->msg_len % 128) == 0)
        {
            sha512_process_chunk(ctx);
            ctx->pbuf = ctx->buf;
        }
    }
}

void sha512_append_ch(struct sha512_ctx *ctx, uint8_t ch)
{
    sha512_append(ctx, &ch, 1);
}

void sha384_finalise(struct sha512_ctx *ctx, uint8_t digest[48])
{
    uint8_t *ptr = (uint8_t*)digest;
    
    uint64_t L = ctx->msg_len * 8;

    // append a single 1 bit
    sha512_append_ch(ctx, 0x80);

    // append k '0' bits where k takes us up to 16 octets before the buf is full
    while ((ctx->msg_len % 128) != 112)
    {
        sha512_append_ch(ctx, 0x00);
    }
    
    // append L - the message size in bits
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, 0);
    }
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, L >> 56);
        L <<= 8;
    }
    
    // Produce the digest
    for (int i=0; i<6; ++i)
    {
        *ptr++ = ctx->hv[i] >> 56 & 0xff;
        *ptr++ = ctx->hv[i] >> 48 & 0xff;
        *ptr++ = ctx->hv[i] >> 40 & 0xff;
        *ptr++ = ctx->hv[i] >> 32 & 0xff;
        *ptr++ = ctx->hv[i] >> 24 & 0xff;
        *ptr++ = ctx->hv[i] >> 16 & 0xff;
        *ptr++ = ctx->hv[i] >> 8 & 0xff;
        *ptr++ = ctx->hv[i] >> 0 & 0xff;
    }
}

void sha512_finalise(struct sha512_ctx *ctx, uint8_t digest[64])
{
    uint8_t *ptr = (uint8_t*)digest;
    
    uint64_t L = ctx->msg_len * 8;

    // append a single 1 bit
    sha512_append_ch(ctx, 0x80);

    // append k '0' bits where k takes us up to 8 octets before the buf is full
    while ((ctx->msg_len % 128) != 112)
    {
        sha512_append_ch(ctx, 0x00);
    }
    
    // append L - the message size in bits
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, 0);
    }
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, L >> 56);
        L <<= 8;
    }
    
    // Produce the digest
    for (int i=0; i<8; ++i)
    {
        *ptr++ = ctx->hv[i] >> 56 & 0xff;
        *ptr++ = ctx->hv[i] >> 48 & 0xff;
        *ptr++ = ctx->hv[i] >> 40 & 0xff;
        *ptr++ = ctx->hv[i] >> 32 & 0xff;
        *ptr++ = ctx->hv[i] >> 24 & 0xff;
        *ptr++ = ctx->hv[i] >> 16 & 0xff;
        *ptr++ = ctx->hv[i] >> 8 & 0xff;
        *ptr++ = ctx->hv[i] >> 0 & 0xff;
    }
}

void sha512_224_finalise(struct sha512_ctx *ctx, uint8_t digest[28])
{
    uint8_t *ptr = (uint8_t*)digest;
    
    uint64_t L = ctx->msg_len * 8;

    // append a single 1 bit
    sha512_append_ch(ctx, 0x80);

    // append k '0' bits where k takes us up to 8 octets before the buf is full
    while ((ctx->msg_len % 128) != 112)
    {
        sha512_append_ch(ctx, 0x00);
    }
    
    // append L - the message size in bits
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, 0);
    }
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, L >> 56);
        L <<= 8;
    }
    
    // Produce the digest
    for (int i=0; i<3; ++i)
    {
        *ptr++ = ctx->hv[i] >> 56 & 0xff;
        *ptr++ = ctx->hv[i] >> 48 & 0xff;
        *ptr++ = ctx->hv[i] >> 40 & 0xff;
        *ptr++ = ctx->hv[i] >> 32 & 0xff;
        *ptr++ = ctx->hv[i] >> 24 & 0xff;
        *ptr++ = ctx->hv[i] >> 16 & 0xff;
        *ptr++ = ctx->hv[i] >> 8 & 0xff;
        *ptr++ = ctx->hv[i] >> 0 & 0xff;
    }
    *ptr++ = ctx->hv[3] >> 56 & 0xff;
    *ptr++ = ctx->hv[3] >> 48 & 0xff;
    *ptr++ = ctx->hv[3] >> 40 & 0xff;
    *ptr++ = ctx->hv[3] >> 32 & 0xff;
}

void sha512_256_finalise(struct sha512_ctx *ctx, uint8_t digest[32])
{
    uint8_t *ptr = (uint8_t*)digest;
    
    uint64_t L = ctx->msg_len * 8;

    // append a single 1 bit
    sha512_append_ch(ctx, 0x80);

    // append k '0' bits where k takes us up to 8 octets before the buf is full
    while ((ctx->msg_len % 128) != 112)
    {
        sha512_append_ch(ctx, 0x00);
    }
    
    // append L - the message size in bits
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, 0);
    }
    for (int i=0; i<8; ++i)
    {
        sha512_append_ch(ctx, L >> 56);
        L <<= 8;
    }
    
    // Produce the digest
    for (int i=0; i<4; ++i)
    {
        *ptr++ = ctx->hv[i] >> 56 & 0xff;
        *ptr++ = ctx->hv[i] >> 48 & 0xff;
        *ptr++ = ctx->hv[i] >> 40 & 0xff;
        *ptr++ = ctx->hv[i] >> 32 & 0xff;
        *ptr++ = ctx->hv[i] >> 24 & 0xff;
        *ptr++ = ctx->hv[i] >> 16 & 0xff;
        *ptr++ = ctx->hv[i] >> 8 & 0xff;
        *ptr++ = ctx->hv[i] >> 0 & 0xff;
    }
}

void sha512_delete(struct sha512_ctx *ctx)
{
    free(ctx);
}

void sha384_calc(uint8_t digest[48], const uint8_t *src, size_t n_bytes)
{
    struct sha512_ctx *ctx = sha512_new();
    sha384_reset(ctx);
    sha512_append(ctx, (const uint8_t*)src, n_bytes);
    sha384_finalise(ctx, digest);
    sha512_delete(ctx);
}

void sha512_calc(uint8_t digest[64], const uint8_t *src, size_t n_bytes)
{
    struct sha512_ctx *ctx = sha512_new();
    sha512_reset(ctx);
    sha512_append(ctx, (const uint8_t*)src, n_bytes);
    sha512_finalise(ctx, digest);
    sha512_delete(ctx);
}

void sha512_224_calc(uint8_t digest[28], const uint8_t *src, size_t n_bytes)
{
    struct sha512_ctx *ctx = sha512_new();
    sha512_224_reset(ctx);
    sha512_append(ctx, (const uint8_t*)src, n_bytes);
    sha512_224_finalise(ctx, digest);
    sha512_delete(ctx);
}

void sha512_256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes)
{
    struct sha512_ctx *ctx = sha512_new();
    sha512_256_reset(ctx);
    sha512_append(ctx, (const uint8_t*)src, n_bytes);
    sha512_256_finalise(ctx, digest);
    sha512_delete(ctx);
}
