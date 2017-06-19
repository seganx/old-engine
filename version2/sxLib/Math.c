#include "Math.h"
#include <time.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static bool s_math_initialized = false;
void sx_math_init(void);

//////////////////////////////////////////////////////////////////////////
//	fast sine/cosine lookup table.
//	code from : http://codepaste.ru/2483/

#define _SINCOS_NUM_BITS            12
#define _SINCOS_TABLE_SIZE          (1 << _SINCOS_NUM_BITS)
#define _SINCOS_TABLE_SHIFT         (16 - _SINCOS_NUM_BITS)
#define _SINCOS_TABLE_MASK          (_SINCOS_TABLE_SIZE - 1)
#define _SINCOS_PI                  32768
#define _SINCOS_2PI                 (2 * _SINCOS_PI)
#define _SINCOS_PI_DIV_2            (_SINCOS_PI / 2)
#define _ANGLE2INT                  (_SINCOS_PI / PI)
#define _INT2ANGLE                  (PI / _SINCOS_PI)

#define angle2int( angle )          (int)( (angle) * _ANGLE2INT )
#define int2angle( val )            ( (val) * _INT2ANGLE )

__declspec (align(16))  float sin_table[_SINCOS_TABLE_SIZE];

SEGAN_LIB_INLINE float sx_sin_fast(const float x)
{
    if (!s_math_initialized) sx_math_init();
    sint a = angle2int(x);
    return sin_table[(a >> _SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
}

SEGAN_LIB_INLINE float sx_cos_fast(const float x)
{
    if (!s_math_initialized) sx_math_init();
    sint a = angle2int(x) + _SINCOS_PI_DIV_2;
    return sin_table[(a >> _SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
}

SEGAN_LIB_INLINE void sx_sin_cos_fast(const float x, float *s, float *c)
{
    if (!s_math_initialized) sx_math_init();
    sint a = angle2int(x);
    *s = sin_table[(a >> _SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
    a += _SINCOS_PI_DIV_2;
    *c = sin_table[(a >> _SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
}


//////////////////////////////////////////////////////////////////////////
//	randomize numbers
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API void sx_randomize(const uint seed)
{
    srand(seed);
}

SEGAN_LIB_API float sx_random_f(const float rmin, const float rmax)
{
    float r = (float)rand() / (float)RAND_MAX;
    return rmin + r * (rmax - rmin);
}

SEGAN_LIB_API sint sx_random_i(const sint rmin, const sint rmax)
{
    return rmin + rand() % (rmax - rmin);
}

//! generate a random number from the seed and advance seed to next step
static sint sx_random_advance(uint *seed)
{
    *seed = ((*seed * 214013L + 2531011L) >> 16);
    return (sint)(*seed & 0x7fff);
}

//! generate a random number from the seed between range and advance seed to next step
SEGAN_LIB_API sint sx_random_advance_i(uint *seed, const sint rmin, const sint rmax)
{
    return rmin + sx_random_advance(seed) % (rmax - rmin);
}

//! generate a random number from the seed between range and advance seed to next step
SEGAN_LIB_API float sx_random_advance_f(uint *seed, const float rmin, const float rmax)
{
    const int val = sx_random_advance(seed) % RAND_MAX;
    float res = (float)val / (float)RAND_MAX;
    return rmin + res * (rmax - rmin);
}


//////////////////////////////////////////////////////////////////////////
//	big number functions
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API byte* sx_big_number_set(byte* dest, uint64 value)
{
    uint i = 0;
    while (value > 0)
    {
        dest[i++] = value % 10;
        value /= 10;
    }
    dest[i] = 10;
    return dest;
}

SEGAN_LIB_API uint sx_big_number_len(const byte* number)
{
    uint res = 0;
    while (number[res] < 10) ++res;
    return res;
}

SEGAN_LIB_API uint sx_big_number_print(const byte* number)
{
    uint len = sx_big_number_len(number);
    for (int i = len - 1; i >= 0; --i)
        printf("%u", number[i]);
    return len;
}

SEGAN_LIB_API uint sx_big_number_remained(const byte* number, const uint value)
{
    uint res = 0;
    uint len = sx_big_number_len(number);
    for (int i = len - 1; i >= 0; --i)
        res = (number[i] + res * 10) % value;
    return res;
}

SEGAN_LIB_API byte* sx_big_number_power(byte* dest, const uint64 i, const uint64 n)
{    // original code from: https://discuss.codechef.com/questions/7349/computing-factorials-of-a-huge-number-in-cc-a-tutorial

    dest[0] = 1;	    // initializes array with only 1 digit, the digit 1.
    int	m = 1;			// initializes digit counter
    int k = 1;			// k is a counter that goes from 1 to n.
    uint64 temp = 0;	// initializes carry variable to 0.
    while (k <= n)
    {
        for (int j = 0; j < m; j++)
        {
            uint64 x = dest[j] * i + temp;	// x contains the digit by digit product
            dest[j] = x % 10;				// contains the digit to store in position j
            temp = x / 10;				    // contains the carry value that will be stored on later indexes
        }
        while (temp > 0)	// while loop that will store the carry value on array.
        {
            dest[m] = temp % 10;
            temp = temp / 10;
            m++; // increments digit counter
        }
        k++;
    }
    dest[m] = 10;

    return dest;
}

//////////////////////////////////////////////////////////////////////////
//	crc32 algorithm
//////////////////////////////////////////////////////////////////////////
static unsigned long crc32_table[256];

static unsigned long crc32_reflect(unsigned long ref, char ch)
{
    unsigned long value = 0;
    for (sint i = 1; i < (ch + 1); ++i)
    {
        if (ref & 1)
            value |= 1 << (ch - i);
        ref >>= 1;
    }
    return value;
}

static uint crc32_init_table(void)
{
    unsigned long ulPolynomial = 0x04c11db7;
    for (sint i = 0; i <= 0xFF; ++i)
    {
        crc32_table[i] = crc32_reflect(i, 8) << 24;
        for (sint j = 0; j < 8; ++j)
            crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
        crc32_table[i] = crc32_reflect(crc32_table[i], 32);
    }
    return 0;
}

SEGAN_LIB_API uint sx_crc32_a(const char* str)
{
    if (!s_math_initialized) sx_math_init();

    sint len = (sint)strlen(str);
    if (len < 1) return 0;

    unsigned char* buffer = (unsigned char*)str;

    unsigned long ulCRC = 0xffffffff;
    while (len--)
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++];

    return ulCRC ^ 0xffffffff;
}

SEGAN_LIB_API uint sx_crc32_w(const wchar* str)
{
    if (!s_math_initialized) sx_math_init();

    sint len = (sint)wcslen(str);
    if (len < 1) return 0;

    wchar* buffer = (wchar*)str;
    unsigned long ulCRC = 0xffffffff;
    while (len--)
    {
        char* c = (char*)(buffer++);
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ c[0]];
        if (c[1])
            ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ c[1]];
    }

    return ulCRC ^ 0xffffffff;
}



//////////////////////////////////////////////////////////////////////////
//	checksum and encryption functions
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API uint sx_checksum(const void* data, const uint size)
{
    if (!data || !size) return 0;
    uint res = 0;
    uint randomer = 1363;
    const byte* d = (byte*)data;
    for (uint i = 0; i < size; ++i)
    	res += d[i] * sx_random_advance(&randomer) * 0x17bcfa72;
    return res;
}

SEGAN_LIB_API void sx_encrypt(void* dest, const void* src, const uint srcsize, const char* key, const uint keysize)
{
    uint randomer = sx_checksum(key, keysize);
    byte* d = (byte*)dest;
    byte* s = (byte*)src;
    for (uint i = 0; i < srcsize; ++i)
    	d[i] = s[i] + key[i % keysize] + sx_random_advance(&randomer);
}

SEGAN_LIB_API void sx_decrypt(void* dest, const void* src, const uint srcsize, const char* key, const uint keysize)
{
    uint randomer = sx_checksum(key, keysize);
    byte* d = (byte*)dest;
    byte* s = (byte*)src;
    for (uint i = 0; i < srcsize; ++i)
        d[i] = s[i] - key[i % keysize] - sx_random_advance(&randomer);
}


//////////////////////////////////////////////////////////////////////////
//  diffie-hellman algorithm
//////////////////////////////////////////////////////////////////////////

SEGAN_LIB_API void sx_dh_secret_Key(char* dest, const int dest_size)
{
    for (int i = 0; i < dest_size; ++i)
        dest[i] = (char)sx_random_i(65, 90);
}

SEGAN_LIB_API void sx_dh_public_key(char* dest, const char* secret_key, const int buff_size, const uint g, const uint p)
{
    byte bignum[128] = { 10 };
    for (int i = 0; i < buff_size; ++i)
    {
        uint64 md = sx_big_number_remained(sx_big_number_power(bignum, g, secret_key[i] - 65), p);
        dest[i] = 65 + (char)md;
    }
}

SEGAN_LIB_API void sx_dh_final_key(char* dest, const char* secret_key, const char* public_key, const int buff_size, const uint p)
{
    byte bignum[128] = { 10 };
    for (int i = 0; i < buff_size; ++i)
    {
        uint64 md = sx_big_number_remained(sx_big_number_power(bignum, public_key[i] - 65, secret_key[i] - 65), p);
        dest[i] = 65 + (char)md;
    }
}


//////////////////////////////////////////////////////////////////////////
//	base 64 functions
//  Base64 encoder/decoder. Originally Apache file ap_base64.c
//  Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////////

/* aaaack but it's fast and const should make it shared text page. */
static const unsigned char base64_pr2six[256] =
{
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static const char base64_basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


SEGAN_LIB_API uint sx_base64_decode_len(const uint len)
{
    return ((len + 3) / 4) * 3;
}

SEGAN_LIB_API uint sx_base64_decode(void* dest, const uint dest_size_in_byte, const char* src, const uint src_size_in_byte)
{
    register int nprbytes = src_size_in_byte;
    uint nbytesdecoded = sx_base64_decode_len(nprbytes);

    //	verify the length of destination
    if (dest_size_in_byte < nbytesdecoded) return 0;

    register const unsigned char *bufin = (const unsigned char *)src;
    register unsigned char* bufout = (unsigned char *)dest;
    bufin = (const unsigned char *)src;

    while (nprbytes > 4)
    {
        *(bufout++) = (unsigned char)(base64_pr2six[*bufin] << 2 | base64_pr2six[bufin[1]] >> 4);
        *(bufout++) = (unsigned char)(base64_pr2six[bufin[1]] << 4 | base64_pr2six[bufin[2]] >> 2);
        *(bufout++) = (unsigned char)(base64_pr2six[bufin[2]] << 6 | base64_pr2six[bufin[3]]);
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1)
        *(bufout++) = (unsigned char)(base64_pr2six[*bufin] << 2 | base64_pr2six[bufin[1]] >> 4);

    if (nprbytes > 2)
        *(bufout++) = (unsigned char)(base64_pr2six[bufin[1]] << 4 | base64_pr2six[bufin[2]] >> 2);

    if (nprbytes > 3)
        *(bufout++) = (unsigned char)(base64_pr2six[bufin[2]] << 6 | base64_pr2six[bufin[3]]);

    //*(bufout++) = '\0';

    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
}

SEGAN_LIB_API uint sx_base64_encode_len(const uint len)
{
    return ((len + 2) / 3 * 4);
}

SEGAN_LIB_API uint sx_base64_encode(char* dest, const uint dest_size_in_byte, const void* src, const uint src_size_in_byte)
{
    //	verify the length
    if (dest_size_in_byte < sx_base64_encode_len(src_size_in_byte)) return 0;

    char* p = dest;
    const char* string = (const char*)src;

    uint i;
    for (i = 0; i < src_size_in_byte - 2; i += 3)
    {
        *p++ = base64_basis_64[(string[i] >> 2) & 0x3F];
        *p++ = base64_basis_64[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
        *p++ = base64_basis_64[((string[i + 1] & 0xF) << 2) | ((int)(string[i + 2] & 0xC0) >> 6)];
        *p++ = base64_basis_64[string[i + 2] & 0x3F];
    }

    if (i < src_size_in_byte)
    {
        *p++ = base64_basis_64[(string[i] >> 2) & 0x3F];

        if (i == (src_size_in_byte - 1))
        {
            *p++ = base64_basis_64[((string[i] & 0x3) << 4)];
            *p++ = '=';
        }
        else
        {
            *p++ = base64_basis_64[((string[i] & 0x3) << 4) | ((int)(string[i + 1] & 0xF0) >> 4)];
            *p++ = base64_basis_64[((string[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    //*p++ = '\0';
    return p - dest - 1;
}

//////////////////////////////////////////////////////////////////////////
//	MD5 functions
//	MAIN SOURCE FROM MONGOOSE API
//////////////////////////////////////////////////////////////////////////
#define MD5_F1(x, y, z)                     (z ^ (x & (y ^ z)))
#define MD5_F2(x, y, z)                     MD5_F1(z, x, y)
#define MD5_F3(x, y, z)                     (x ^ y ^ z)
#define MD5_F4(x, y, z)                     (y ^ (x | ~z))
#define MD5_STEP(f, w, x, y, z, data, s)    ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

typedef struct sx_md5_ctx
{
    uint buf[4];
    uint bits[2];
    byte in[64];
}
sx_md5_ctx;


static int md5_is_big_endian(void)
{
    static const int n = 1;
    return ((char *)&n)[0] == 0;
}

// Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
// initialization constants.
static void md5_init(sx_md5_ctx *ctx) 
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;
    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

static void md5_transform(uint buf[4], uint const in[16])
{
    register uint a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5_STEP(MD5_F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5_STEP(MD5_F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5_STEP(MD5_F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5_STEP(MD5_F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5_STEP(MD5_F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5_STEP(MD5_F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5_STEP(MD5_F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5_STEP(MD5_F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5_STEP(MD5_F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5_STEP(MD5_F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5_STEP(MD5_F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5_STEP(MD5_F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5_STEP(MD5_F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5_STEP(MD5_F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5_STEP(MD5_F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5_STEP(MD5_F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5_STEP(MD5_F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5_STEP(MD5_F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5_STEP(MD5_F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5_STEP(MD5_F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5_STEP(MD5_F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5_STEP(MD5_F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5_STEP(MD5_F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5_STEP(MD5_F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5_STEP(MD5_F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5_STEP(MD5_F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5_STEP(MD5_F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5_STEP(MD5_F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5_STEP(MD5_F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5_STEP(MD5_F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5_STEP(MD5_F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5_STEP(MD5_F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5_STEP(MD5_F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5_STEP(MD5_F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5_STEP(MD5_F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5_STEP(MD5_F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5_STEP(MD5_F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5_STEP(MD5_F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5_STEP(MD5_F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5_STEP(MD5_F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5_STEP(MD5_F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5_STEP(MD5_F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5_STEP(MD5_F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5_STEP(MD5_F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5_STEP(MD5_F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5_STEP(MD5_F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5_STEP(MD5_F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5_STEP(MD5_F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5_STEP(MD5_F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5_STEP(MD5_F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5_STEP(MD5_F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5_STEP(MD5_F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5_STEP(MD5_F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5_STEP(MD5_F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5_STEP(MD5_F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5_STEP(MD5_F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5_STEP(MD5_F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5_STEP(MD5_F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5_STEP(MD5_F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5_STEP(MD5_F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5_STEP(MD5_F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5_STEP(MD5_F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5_STEP(MD5_F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5_STEP(MD5_F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

static void md5_byte_reverse(unsigned char *buf, unsigned longs) 
{
    uint t;

    // Forrest: MD5 expect LITTLE_ENDIAN, swap if BIG_ENDIAN
    if (md5_is_big_endian()) {
        do {
            t = (uint)((unsigned)buf[3] << 8 | buf[2]) << 16 |
                ((unsigned)buf[1] << 8 | buf[0]);
            *(uint *)buf = t;
            buf += 4;
        } while (--longs);
    }
}

static void md5_update(sx_md5_ctx *ctx, unsigned char const *buf, unsigned len) 
{
    uint t;

    t = ctx->bits[0];
    if ((ctx->bits[0] = t + ((uint)len << 3)) < t)
        ctx->bits[1]++;
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;

    if (t) {
        unsigned char *p = (unsigned char *)ctx->in + t;

        t = 64 - t;
        if (len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        md5_byte_reverse(ctx->in, 16);
        md5_transform(ctx->buf, (uint *)ctx->in);
        buf += t;
        len -= t;
    }

    while (len >= 64) {
        memcpy(ctx->in, buf, 64);
        md5_byte_reverse(ctx->in, 16);
        md5_transform(ctx->buf, (uint *)ctx->in);
        buf += 64;
        len -= 64;
    }

    memcpy(ctx->in, buf, len);
}

static void md5_final(unsigned char digest[16], sx_md5_ctx *ctx) 
{
    unsigned count;
    unsigned char *p;
    uint *a;

    count = (ctx->bits[0] >> 3) & 0x3F;

    p = ctx->in + count;
    *p++ = 0x80;
    count = 64 - 1 - count;
    if (count < 8) {
        memset(p, 0, count);
        md5_byte_reverse(ctx->in, 16);
        md5_transform(ctx->buf, (uint *)ctx->in);
        memset(ctx->in, 0, 56);
    }
    else {
        memset(p, 0, count - 8);
    }
    md5_byte_reverse(ctx->in, 14);

    a = (uint *)ctx->in;
    a[14] = ctx->bits[0];
    a[15] = ctx->bits[1];

    md5_transform(ctx->buf, (uint *)ctx->in);
    md5_byte_reverse((unsigned char *)ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
}

// Stringify binary data. Output buffer must be twice as big as input,
// because each byte takes 2 bytes in string representation
static void md5_bin2str(char *to, const unsigned char *p, size_t len)
{
    static const char *hex = "0123456789abcdef";

    for (; len--; p++) {
        *to++ = hex[p[0] >> 4];
        *to++ = hex[p[0] & 0x0f];
    }
    *to = '\0';
}

 SEGAN_LIB_API char* sx_md5(char desc[33], ...)
{
    sx_md5_ctx ctx;
    md5_init(&ctx);

    va_list ap;
    va_start(ap, desc);
    const char *p;
    while ((p = va_arg(ap, const char *)) != NULL)
        md5_update(&ctx, (const unsigned char *)p, (unsigned)strlen(p));
    va_end(ap);

    unsigned char hash[16];
    md5_final(hash, &ctx);
    md5_bin2str(desc, hash, sizeof(hash));
    return desc;
}


//////////////////////////////////////////////////////////////////////////
// initialize internal library
//////////////////////////////////////////////////////////////////////////
static void sx_math_init(void)
{
    //  fill sine/cosine table
    for (sint i = 0; i < _SINCOS_TABLE_SIZE; i++)
    {
        const float angle = int2angle(i << _SINCOS_TABLE_SHIFT);
        sin_table[i] = sinf(angle);
    }

    //	initialize crc32 table
    crc32_init_table();

    s_math_initialized = true;
}
