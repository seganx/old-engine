#include "Memory.h"
#include "Math.h"
#include "Utils.h"
#include "Timer.h"

#include <memory>
#include <time.h>
#include <ctype.h>


#define RSTS			sizeof(SX_RAW_SIZE_TYPE)
#define rawsize(raw)	(*((SX_RAW_SIZE_TYPE*)raw))

//////////////////////////////////////////////////////////////////////////
//	RAW DATA TOOLS
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API const char* sx_raw_read(char* dest, const SX_RAW_SIZE_TYPE destsize, const char* src)
{
	SX_RAW_SIZE_TYPE size = rawsize(src);
	if (size > 0 && size <= destsize)
	{
		src += RSTS;
		sx_mem_copy(dest, src, size);
		src += size;
		return src;
	}
	return null;
}

SEGAN_LIB_API bool sx_raw_read(char* dest, const SX_RAW_SIZE_TYPE destsize, const char* src, const uint dataindex)
{
	for (uint i = 0; i < dataindex; ++i)
	{
		SX_RAW_SIZE_TYPE size = rawsize(src);
		if (size > 0)
			src += RSTS + size;
		else return false;
	}

	SX_RAW_SIZE_TYPE size = rawsize(src);
	if (size > 0 && size <= destsize)
	{
		src += RSTS;
		sx_mem_copy(dest, src, size);
		return true;
	}

	return false;
}

SEGAN_LIB_API uint sx_raw_write(char* dest, const uint destsize, const char* data, const SX_RAW_SIZE_TYPE datasize)
{
	uint occupied = 0;
	SX_RAW_SIZE_TYPE size = rawsize(dest);
	while (size > 0)
	{
		size += RSTS;
		dest += size;
		occupied += size;
		size = rawsize(dest);
	}

	uint res = occupied + datasize + RSTS;
	if (res <= destsize)
	{
		sx_mem_copy(dest, &datasize, RSTS);
		dest += RSTS;
		sx_mem_copy(dest, data, datasize);
		return res;
	}

	return 0;
}

SEGAN_LIB_API uint sx_raw_write_text(char* dest, const uint destsize, const char* text)
{
	if (text)
		return sx_raw_write(dest, destsize, text, sx_str_len(text));
	else
		return sx_raw_write(dest, destsize, "", 1);
}

SEGAN_LIB_API void sx_raw_print(const char* src)
{
	printf("\nRaw Data:\n");

	while (1)
	{
		char buffer[1024] = { 0 };
		SX_RAW_SIZE_TYPE size = rawsize(src);
		if (size > 0 && size < 1024)
		{
			src += RSTS;
			sx_mem_copy(buffer, src, size);
			printf("%u : %s\n", size, buffer);
			src += size;
		}
		else return;
	}
}

//////////////////////////////////////////////////////////////////////////
//	STRING TOOLS
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API bool sx_load_string(String& dest, const wchar* filename)
{
	FILE* f = 0;
	if (_wfopen_s(&f, filename, L"r, ccs=UNICODE") == 0)
	{
		wchar tmp[1024] = { 0 };
		while (fread_s(tmp, 1023, 2, 1023, f))
		{
			dest << tmp;
		}
		fclose(f);
		return true;
	}
	else return false;
}

SEGAN_LIB_API bool sx_load_string_list(Array<String*>& dest, const wchar* filename)
{
	FILE* f = 0;
	if (_wfopen_s(&f, filename, L"r, ccs=UNICODE") == 0)
	{
		wchar tmp[2048] = { 0 };
		wchar c = 0; int index = 0;
		while (fread_s(&c, 2, 2, 1, f))
		{
			if (c == '\n')
			{
				tmp[index] = 0;
				if (index)
				{
					String* str = sx_new(String);
					str->set_text(tmp);
					dest.push_back(str);
				}
				index = 0;
				tmp[0] = 0;
			}
			else if (c && c != '\r')
			{
				tmp[index++] = c;
			}
		}
		if (index)
		{
			String* str = sx_new(String);
			str->set_text(tmp);
			dest.push_back(str);
		}
		fclose(f);
		return true;
	}
	else return false;
}

SEGAN_LIB_API void sx_clear_string_list(Array<String*>& list)
{
	for (sint i = 0; i < list.m_count; ++i)
	{
		sx_safe_delete(list.m_item[i]);
	}
	list.clear();
}

SEGAN_LIB_API uint sx_time_to_str(wchar* dest, const uint destSize, const wchar* timeFormat)
{
	time_t rawTime;
	time(&rawTime);
	struct tm timeInfo;
	localtime_s(&timeInfo, &rawTime);
	return (uint)wcsftime(dest, destSize, timeFormat, &timeInfo);
}


SEGAN_LIB_API void sx_hash_write_index(char* dest, const int destsize, const uint index)
{
	for (int i = 0; i < destsize; ++i)
		dest[i] = sx_random_i(1) == 1 ? sx_random_i_limit('a', 'z') : sx_random_i_limit('0', '9');

	char tmp[16] = { 0 };
	int len = sprintf_s(tmp, 16, "%u", index);
	tmp[len] = sx_random_i_limit('a', 'z');

	int dlen = destsize / 2;
	for (int i = 0; i < dlen && i <= len; ++i)
		dest[i * 2 + 1] = tmp[i];
}

SEGAN_LIB_API uint sx_hash_read_index(const char* hash, const uint defaul_val)
{
	char tmp[16] = { 0 };

	const char *h = hash;
	for (int i = 0; i < 11 && isdigit(*++h); ++i)
		tmp[i] = *h++;

	uint res = defaul_val;
	sscanf_s(tmp, "%u", &res, sizeof(res));
	return res;
}

SEGAN_INLINE void sx_randomize_by_time(void)
{
	sx_randomize( (uint)sx_time_counter() );
}
