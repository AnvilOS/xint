
#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
class Sha256
{
public:
    Sha256() { reset(); }
    virtual ~Sha256() {}
    
    void reset();
    void append(const uint8_t *msg, size_t n);
    void append(uint8_t ch);
    void finalise(uint8_t digest[32]);

private:
    void process_chunk();

    uint32_t hv[8];
    uint8_t buf[64];
    uint8_t *pbuf;
    size_t msg_len;
};

extern "C"
{
#endif // __cplusplus

void sha256_calc(uint8_t digest[32], const uint8_t *src, size_t n_bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SHA256_H
