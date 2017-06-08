#include "crypto.h"
#include <malloc.h>

#define diffie_hellman_g	    7
#define diffie_hellman_p	    23

#define crypto_token_key        "1qa!z2@ws#x$3%edc4r^fv5&tgb*6yh(n7ujm)8ik9ol0p"
#define crypto_token_keysize    45
#define crypto_token_checksize  8

uint crypto_token_generate(char* dest, const uint destsize, const void* data, const uint size)
{
    uint base64size = sx_base64_encode_len(size);
    uint res = base64size + crypto_token_checksize;
    if (!dest || !destsize) return res;
    if (destsize < res) return 0;
    
    char* tmp = alloca(size);
    sx_encrypt(tmp, data, size, crypto_token_key, crypto_token_keysize);
    char* chsum = dest + sx_base64_encode(dest, destsize, tmp, size) + 1;
    u32 chsumvalue = sx_checksum(dest, base64size);
    sx_base64_encode(chsum, crypto_token_checksize, &chsumvalue, 4);

    return res;
}

uint crypto_token_validate(const void* data, const uint size)
{
    const byte* obj = (const byte*)data;
    char buf[crypto_token_checksize] = init;
    uint objectsize = size - crypto_token_checksize;
    u32 chsumvalue = sx_checksum(data, objectsize);
    sx_base64_encode(buf, crypto_token_checksize, &chsumvalue, 4);
    if ( sx_mem_cmp(buf, obj + objectsize, crypto_token_checksize) == 0 )
        return sx_base64_decode_len(objectsize);
    return 0;
}

uint crypto_token_decode(void* dest, const uint destsize, const char* data, const uint size)
{
    uint tokensize = size - crypto_token_checksize;
    uint objectsize = sx_base64_decode_len(tokensize);
    if (destsize < objectsize) return 0;

    char* tmp = alloca(objectsize);
    sx_base64_decode(tmp, objectsize, data, tokensize);
    sx_decrypt(dest, tmp, objectsize, crypto_token_key, crypto_token_keysize);
    
    return objectsize;
}

int crypto_compute_keys(char* dest_local_key, char* dest_public_key, const char* received_key)
{
    // validate received key
    {
        const int hirange = 65 + diffie_hellman_p;
        for (int i = 0; i < crypto_key_len; ++i)
            if (sx_between_i(received_key[i], 65, hirange) == false)
                return 1;
    }

    // generate secret key
    char secret_key[crypto_key_len] = init;
    sx_dh_secret_Key(secret_key, crypto_key_len);

    // generate public key based on secret key for send to the client
    sx_dh_public_key(dest_public_key, secret_key, crypto_key_len, diffie_hellman_g, diffie_hellman_p);

    // generate local key based on secret and received key for encryption/decryption
    sx_dh_final_key(dest_local_key, secret_key, received_key, crypto_key_len, diffie_hellman_p);

    return 0;
}

void crypto_encrypt(void* dest, const void* src, const uint srcsize, const char* key, const uint keysize)
{
    byte* d = (byte*)dest;
    byte* s = (byte*)src;
    for (uint i = 0; i < srcsize; ++i)
        d[i] = s[i] + key[i % keysize];
}

void crypto_decrypt(void* dest, const void* src, const uint srcsize, const char* key, const uint keysize)
{
    byte* d = (byte*)dest;
    byte* s = (byte*)src;
    for (uint i = 0; i < srcsize; ++i)
        d[i] = s[i] - key[i % keysize];
}



