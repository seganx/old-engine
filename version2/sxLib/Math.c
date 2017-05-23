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

#define angle2int( angle )			(int)( (angle) * _ANGLE2INT )
#define int2angle( val )			( (val) * _INT2ANGLE )

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
    	res += d[i] + sx_random_advance(&randomer) + 0x17bcfa72;
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
