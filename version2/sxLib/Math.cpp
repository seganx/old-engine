#include <time.h>
#include <string>
#include "Bigint.h"
#include "Math.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//	fast sine/cosine lookup table.
//	code from : http://codepaste.ru/2483/

#define angle2int( angle )			( sint( (angle) * _ANGLE2INT ) )
#define int2angle( val )			( (val) * _INT2ANGLE )

const sint	_SINCOS_NUM_BITS = 12;
const sint	_SINCOS_TABLE_SIZE = 1 << _SINCOS_NUM_BITS;
const sint	SINCOS_TABLE_SHIFT = 16 - _SINCOS_NUM_BITS;
const sint	_SINCOS_TABLE_MASK = _SINCOS_TABLE_SIZE - 1;
const sint	_SINCOS_PI = 32768;
const sint	_SINCOS_2PI = 2 * _SINCOS_PI;
const sint	_SINCOS_PI_DIV_2 = _SINCOS_PI / 2;
const float _ANGLE2INT = float(_SINCOS_PI / PI);
const float _INT2ANGLE = float(PI / _SINCOS_PI);

__declspec (align(16))  float sin_table[_SINCOS_TABLE_SIZE];

SEGAN_INLINE float sx_sin_fast(const float x)
{
	sint a = angle2int(x);
	return sin_table[(a >> SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
}

SEGAN_INLINE float sx_cos_fast(const float x)
{
	sint a = angle2int(x) + _SINCOS_PI_DIV_2;
	return sin_table[(a >> SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
}

SEGAN_INLINE void sx_sin_cos_fast(const float IN x, float& OUT s, float& OUT c)
{
	sint a = angle2int(x);
	s = sin_table[(a >> SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
	a += _SINCOS_PI_DIV_2;
	c = sin_table[(a >> SINCOS_TABLE_SHIFT) & _SINCOS_TABLE_MASK];
}



//////////////////////////////////////////////////////////////////////////
//	randomize numbers
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API void sx_randomize(const uint& seed)
{
	srand(seed);
}

SEGAN_INLINE float sx_random_f(const float range)
{
	float r = (float)rand() / (float)RAND_MAX;
	return  (range * r);
}

SEGAN_INLINE sint sx_random_i(const sint range)
{
	float r = (float)rand() / (float)RAND_MAX;
	return sx_round(range * r);
}


//////////////////////////////////////////////////////////////////////////
//	id generator
//////////////////////////////////////////////////////////////////////////
uint id_counter = 0;

SEGAN_INLINE void sx_id_set_counter(const uint id)
{
	id_counter = id;
}

SEGAN_INLINE uint sx_id_generate(void)
{
#if ( SEGAN_CRITICAL_SECTION == 1 )
	sx_enter_cs();
	uint id = id_counter++;
	sx_leave_cs();
	return id;
#else
	return id_counter++;
#endif
}

//////////////////////////////////////////////////////////////////////////
//	crc32 algorithm
//////////////////////////////////////////////////////////////////////////
unsigned long crc32_table[256];

unsigned long crc32_reflect(unsigned long ref, char ch)
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

uint crc32_init_table(void)
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

SEGAN_INLINE uint sx_crc32_a(const char* str)
{
	sint len = (sint)strlen(str);
	if (len < 1) return 0;

	unsigned char* buffer = (unsigned char*)str;

	unsigned long ulCRC(0xffffffff);
	while (len--)
		ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++];

	return ulCRC ^ 0xffffffff;
}

SEGAN_INLINE uint sx_crc32_w(const wchar* str)
{
	sint len = (sint)wcslen(str);
	if (len < 1) return 0;

	wchar* buffer = (wchar*)str;
	unsigned long ulCRC(0xffffffff);
	while (len--)
	{
		union hchar {
			struct {
				char c1;
				char c2;
			};
			wchar c;
		} hc = *(hchar*)(buffer++);
		ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ hc.c1];
		if (hc.c2)
			ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ hc.c2];

	}

	return ulCRC ^ 0xffffffff;
}



//////////////////////////////////////////////////////////////////////////
//	checksum and encryption functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	use protocol random class to handle multi threaded calls
class CSRandom
{
public:
	CSRandom(const uint seed = 1363) : m_seed(seed), m_curr(63) {}

	byte generate(void)
	{
		m_curr += ((m_curr * m_seed * 28454642) + (m_seed * 38745674));
		return m_curr ? m_curr : ++m_curr;
	}

public:
	uint	m_seed;
	byte	m_curr;
};


//////////////////////////////////////////////////////////////////////////
//	encryption functions
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE uint sx_checksum(const void* data, const uint size, const uint key /*= 1363*/)
{
	if (!data || !size) return 0;
	uint r = 0;
	const char* d = (char*)data;
	CSRandom randomer(key);
	for (uint i = 0; i < size; ++i)
		r += d[i] * 0xabcdef12 + randomer.generate() + d[i] + key;
	return r;
}

SEGAN_INLINE void sx_encrypt(void* dest, const void* src, const uint size, const uint key /*= 1363*/)
{
	CSRandom randomer(key);
	byte* d = (byte*)dest;
	byte* s = (byte*)src;
	for (uint i = 0; i < size; ++i)
		d[i] = s[i] + randomer.generate();
}

SEGAN_INLINE void sx_decrypt(void* dest, const void* src, const uint size, const uint key /*= 1363*/)
{
	CSRandom randomer(key);
	byte* d = (byte*)dest;
	byte* s = (byte*)src;
	for (uint i = 0; i < size; ++i)
		d[i] = s[i] - randomer.generate();
}

SEGAN_INLINE void sx_dh_secret_Key(char* dest, const int& dest_size)
{
	for (int i = 0; i < dest_size; ++i)
		dest[i] = (char)sx_random_i_limit(65, 90);
}

SEGAN_INLINE void sx_dh_public_key(char* dest, const char* secret_key, const int& buff_size, const uint& g, const uint& p)
{
	for (int i = 0; i < buff_size; ++i)
	{
		uint64 md = sx_power(g, secret_key[i] - 65) % p;
		dest[i] = 65 + (char)md;
	}
}

SEGAN_INLINE void sx_dh_final_key(char* dest, const char* secret_key, const char* public_key, const int& buff_size, const uint& p)
{
	for (int i = 0; i < buff_size; ++i)
	{
		uint64 md = sx_power(public_key[i] - 65, secret_key[i] - 65) % p;
		dest[i] = 65 + (char)md;
	}
}


//////////////////////////////////////////////////////////////////////////
// initialize internal library
//////////////////////////////////////////////////////////////////////////
bool sx_lib_init_math(void)
{
	//  fill sine/cosine table
	for (sint i = 0; i < _SINCOS_TABLE_SIZE; i++)
	{
		const float angle = int2angle(i << SINCOS_TABLE_SHIFT);
		sin_table[i] = sinf(angle);
	}

	//  initialize random seed
	sx_randomize((uint)time(null));

	//	initialize crc32 table
	crc32_init_table();

	return true;
}

bool s_math_initialized = sx_lib_init_math();
