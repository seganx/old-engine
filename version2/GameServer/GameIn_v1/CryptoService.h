#include "GameIn.h"

#define crypto_key_len	        32
#define crypto_authen_code_len  52

typedef struct crypto_authen_code
{
    byte    version;                    //  version of the object
    char    local_key[crypto_key_len];  //  local key
}
crypto_authen_code;

typedef struct crypto_access_code
{
    byte    version;                    //  version of the object
    char    local_key[crypto_key_len];  //  local key
    uint    profile_id;                 //  unique id of the player used to access from database
}
crypto_access_code;


/*! 
generate a valid token which is ready to sent and return the size of the token in byte.
NOTE: pass null for dest to compute required dest size!
*/
uint crypto_token_generate(char* dest, const uint destsize, const void* data, const uint size);

//! verify the token is valid and return the size of dest object. return 0 on failed
uint crypto_token_validate(const void* data, const uint size);

/*!
decode a valid token to dest object and return total bytes of the result.
NOTE: use crypto_token_validate to estimate object size inside the token!
*/
uint crypto_token_decode(void* dest, const uint destsize, const char* data, const uint size);


//! compute local key and public key by specified received key
int crypto_compute_keys(char* dest_local_key, char* dest_public_key, const char* received_key);

//! encrypt data to send to client
void crypto_encrypt(void* dest, const void* src, const uint srcsize, const char* key, const uint keysize);

//! decrypt data comes from client
void crypto_decrypt(void* dest, const void* src, const uint srcsize, const char* key, const uint keysize);
