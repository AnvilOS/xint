
#include "sha256.h"

#include <string.h>

void Sha256::reset()
{
    // Initialise hash values
    hv[0] = 0x6a09e667;
    hv[1] = 0xbb67ae85;
    hv[2] = 0x3c6ef372;
    hv[3] = 0xa54ff53a;
    hv[4] = 0x510e527f;
    hv[5] = 0x9b05688c;
    hv[6] = 0x1f83d9ab;
    hv[7] = 0x5be0cd19;
    // Initialise buffer variables
    pbuf = buf;
    msg_len = 0;
}

// The array of round constants is fixed
static const uint32_t k[] =
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

#define byte_swap(x) ((x)[0]<< 24|(x)[1]<<16|(x)[2]<<8|(x)[3])
#define rightrotate(x, n) (((x)>>(n))|((x)<<(32-(n))))

void Sha256::process_chunk()
{
    // Instead of pre-calculating the full 64 message schedule array we calculate
    // each entry as required. Also we only keep the most recent 16 in a
    // cirular fashion and use mod arithmetic to access them
    uint32_t ww[16];
#define w(a) ww[(a)&0xf]
    
    // Initialise working variables to current hash values
    uint32_t a = hv[0];
    uint32_t b = hv[1];
    uint32_t c = hv[2];
    uint32_t d = hv[3];
    uint32_t e = hv[4];
    uint32_t f = hv[5];
    uint32_t g = hv[6];
    uint32_t h = hv[7];
    
    for (int i=0; i<64; ++i)
    {
        if (i < 16)
        {
            w(i) = byte_swap(&buf[4*i]);
        }
        else
        {
            uint32_t s0 = rightrotate(w(i-15), 7) ^ rightrotate(w(i-15), 18) ^ (w(i-15) >> 3);
            uint32_t s1 = rightrotate(w(i-2), 17) ^ rightrotate(w(i-2), 19) ^ (w(i-2) >> 10);
            w(i) = w(i-16) + s0 + w(i-7) + s1;
        }
        uint32_t S1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = h + S1 + ch + k[i] + w(i);
        uint32_t S0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
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

    hv[0] += a;
    hv[1] += b;
    hv[2] += c;
    hv[3] += d;
    hv[4] += e;
    hv[5] += f;
    hv[6] += g;
    hv[7] += h;
}

void Sha256::append(const uint8_t *msg, size_t n)
{
    const uint8_t *p = msg;
    while (n--)
    {
        *pbuf++ = *p++;
        ++msg_len;
        if ((msg_len % 64) == 0)
        {
            process_chunk();
            pbuf = buf;
        }
    }
}

void Sha256::append(uint8_t ch)
{
    append(&ch, 1);
}

void Sha256::finalise(uint8_t digest[32])
{
    uint8_t *ptr = (uint8_t*)digest;
    
    uint64_t L = msg_len * 8;

    // append a single 1 bit
    append(0x80);

    // append k '0' bits where k takes us up to 8 octets before the buf is full
    while ((msg_len % 64) != 56)
    {
        append(0x00);
    }
    
    // append L - the message size in bits
    for (int i=0; i<8; ++i)
    {
        append(L >> 56);
        L <<= 8;
    }
    
    // Produce the digest
    for (int i=0; i<8; ++i)
    {
        *ptr++ = hv[i] >> 24 & 0xff;
        *ptr++ = hv[i] >> 16 & 0xff;
        *ptr++ = hv[i] >> 8 & 0xff;
        *ptr++ = hv[i] >> 0 & 0xff;
    }
}

void sha256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes)
{
    Sha256 sha256;
    sha256.reset();
    sha256.append((const uint8_t*)src, n_bytes);
    sha256.finalise(digest);
}
