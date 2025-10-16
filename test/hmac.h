
#ifndef HMAC_H
#define HMAC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
class Hmac
{
public:
    Hmac() { reset(); }
    virtual ~Hmac() {}
    
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

void hmac_calc(uint8_t digest[32], const uint8_t *key, size_t keylen, const uint8_t *msg, size_t msglen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HMAC_H
