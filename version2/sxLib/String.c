#include "String.h"
#include <ctype.h>
#include <string.h>

SEGAN_LIB_INLINE char sx_str_upper(char c) { return toupper(c); }
SEGAN_LIB_INLINE char sx_str_lower(char c) { return tolower(c); }

SEGAN_LIB_INLINE uint sx_str_len(const char* str) { return str ? strlen(str) : 0; }
SEGAN_LIB_INLINE sint sx_str_cmp(const char* str1, const char* str2) { return str1 && str2 ? strcmp(str1, str2) : (str1 ? 1 : (str2 ? -1 : 0)); }
SEGAN_LIB_INLINE const char* sx_str_str(const char* str, const char* what) { return str && what ? strstr(str, what) : null; }
SEGAN_LIB_INLINE sint sx_str_copy(char* dest, const sint dest_size_in_byte, const char* src) { return strcpy_s(dest, dest_size_in_byte, src); }

SEGAN_LIB_INLINE sint sx_str_split_count(const char* str, const char* split)
{
    if (!str || !split) return 0;
    sint res = 1;
    for (const char* f = strstr(str, split); f != null; f = strstr(++f, split))
        res++;
    return res;
}

SEGAN_LIB_INLINE sint sx_str_split(char* dest, const uint destsize, const char* str, const char* split, const uint index)
{
    if (!str || !split) return 0;

    uint splitlen = strlen(split);
    const char *start = str, *end = strstr(str, split);
    for (uint i = 0; i < index && end != null; ++i)
    {
        start = end + splitlen;
        end = strstr(++end, split);
    }

    if (end == null)
        end = str + strlen(str);

    if (start)
    {
        int res = _snprintf_s(dest, destsize, _TRUNCATE, "%.*s", (end - start), start);
        return res < 0 ? destsize - 1 : res;
    }
    else return 0;
}


SEGAN_LIB_INLINE sint sx_str_to_int(const char* str, const sint defaul_val /*= 0 */)
{
    if (!str) return defaul_val;
    sint res = defaul_val;
    sscanf_s(str, "%d", &res, sizeof(res));
    return res;
}

SEGAN_LIB_INLINE uint sx_str_to_uint(const char* str, const uint defaul_val /*= 0 */)
{
    if (!str) return defaul_val;
    uint res = defaul_val;
    sscanf_s(str, "%u", &res, sizeof(res));
    return res;
}

SEGAN_LIB_INLINE uint64 sx_str_to_uint64(const char* str, const uint64 defaul_val /*= 0 */)
{
    if (!str) return defaul_val;
    uint64 res = defaul_val;
    sscanf_s(str, "%llu", &res, sizeof(res));
    return res;
}

SEGAN_LIB_INLINE const char* sx_str_get_filename(const char* filename)
{
    const char* res = filename;
    for (const char* c = filename; *c != 0; ++c)
        if (*c == '/' || *c == '\\')
            res = c + 1;
    return res;
}


SEGAN_LIB_INLINE uint sx_wchar_to_utf8(char* dest, const uint destsize, const uint ch)
{//	code from : http://www.opensource.apple.com/source/OpenLDAP/OpenLDAP-186/OpenLDAP/libraries/libldap/utf-8-conv.c
    uint len = 0;
    if (!dest || !destsize)   /* just determine the required UTF-8 char length. */
    {
        if (ch < 0)         return 0;
        if (ch < 0x80)      return 1;
        if (ch < 0x800)     return 2;
        if (ch < 0x10000)   return 3;
        if (ch < 0x200000)  return 4;
        if (ch < 0x4000000) return 5;
    }
    else if (ch < 0)
    {
        len = 0;
    }
    else if (ch < 0x80)
    {
        if (destsize >= 1)
        {
            dest[len++] = (char)ch;
        }

    }
    else if (ch < 0x800)
    {
        if (destsize >= 2)
        {
            dest[len++] = 0xc0 | (ch >> 6);
            dest[len++] = 0x80 | (ch & 0x3f);
        }

    }
    else if (ch < 0x10000)
    {
        if (destsize >= 3)
        {
            dest[len++] = 0xe0 | (ch >> 12);
            dest[len++] = 0x80 | ((ch >> 6) & 0x3f);
            dest[len++] = 0x80 | (ch & 0x3f);
        }

    }
    else if (ch < 0x200000)
    {
        if (destsize >= 4)
        {
            dest[len++] = 0xf0 | (ch >> 18);
            dest[len++] = 0x80 | ((ch >> 12) & 0x3f);
            dest[len++] = 0x80 | ((ch >> 6) & 0x3f);
            dest[len++] = 0x80 | (ch & 0x3f);
        }

    }
    else if (ch < 0x4000000)
    {
        if (destsize >= 5)
        {
            dest[len++] = 0xf8 | (ch >> 24);
            dest[len++] = 0x80 | ((ch >> 18) & 0x3f);
            dest[len++] = 0x80 | ((ch >> 12) & 0x3f);
            dest[len++] = 0x80 | ((ch >> 6) & 0x3f);
            dest[len++] = 0x80 | (ch & 0x3f);
        }
    }
    else
    {
        if (destsize >= 6)
        {
            dest[len++] = 0xfc | (ch >> 30);
            dest[len++] = 0x80 | ((ch >> 24) & 0x3f);
            dest[len++] = 0x80 | ((ch >> 18) & 0x3f);
            dest[len++] = 0x80 | ((ch >> 12) & 0x3f);
            dest[len++] = 0x80 | ((ch >> 6) & 0x3f);
            dest[len++] = 0x80 | (ch & 0x3f);
        }
    }
    return len;
}

SEGAN_LIB_INLINE uint sx_str_to_utf8(char* dest, const uint destsize, const wchar* src)
{
    int r = 0;
    char tmp[32];
    char* d = dest;
    while (*src)
    {
        r = sx_wchar_to_utf8(tmp, 32, *src++);
        if (r > 0)
        {
            memcpy(d, tmp, r);
            d += r;
        }
        else
        {
            *d++ = (char)*src++;
        }
    }
    *d = 0;
    return (uint)(d - dest);
}


//////////////////////////////////////////////////////////////////////////
//	code from :
//	http://www.opensource.apple.com/source/OpenLDAP/OpenLDAP-186/OpenLDAP/libraries/libldap/utf-8.c

const char ldap_utf8_lentab[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0 };

const char ldap_utf8_mintab[] = {
    (char)0x20, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
    (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
    (char)0x30, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
    (char)0x38, (char)0x80, (char)0x80, (char)0x80, (char)0x3c, (char)0x80, (char)0x00, (char)0x00 };

/* LDAP_MAX_UTF8_LEN is 3 or 6 depending on size of wchar_t */
#define LDAP_MAX_UTF8_LEN		 ( sizeof(wchar) * 3/2 )
#define LDAP_UTF8_ISASCII(p)	 ( !(*(const unsigned char *)(p) & 0x80 ) )
#define LDAP_UTF8_CHARLEN(p)	 ( LDAP_UTF8_ISASCII(p) ? 1 : ldap_utf8_lentab[*(const unsigned char *)(p) ^ 0x80] )
#define LDAP_UTF8_CHARLEN2(p, l) ( ( ( l = LDAP_UTF8_CHARLEN( p )) < 3 || ( ldap_utf8_mintab[*(const unsigned char *)(p) & 0x1f] & (p)[1] ) ) ? l : 0 )

SEGAN_LIB_INLINE uint sx_utf8_to_wchar(wchar dest, const uint destwords, const char* src)
{
    if (!src) return 0;

    /* Get UTF-8 sequence length from 1st byte */
    sint utflen = LDAP_UTF8_CHARLEN2(src, utflen);

    if (utflen == 0 || utflen > (int)LDAP_MAX_UTF8_LEN) return 0;

    /* First byte minus length tag */
    unsigned char mask[] = { 0, 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    wchar ch = (wchar)(src[0] & mask[utflen]);

    for (sint i = 1; i < utflen; ++i)
    {
        /* Subsequent bytes must start with 10 */
        if ((src[i] & 0xc0) != 0x80) return 0;

        ch <<= 6;			/* 6 bits of data in each subsequent byte */
        ch |= (wchar)(src[i] & 0x3f);
    }

    dest = ch;
    return utflen;
}

SEGAN_LIB_INLINE uint sx_utf8_to_str(wchar* dest, const uint destwords, const char* src)
{
    /* If input ptr is NULL or empty... */
    if (!src || !*src)
    {
        if (dest) *dest = 0;
        return 0;
    }

    /* Examine next UTF-8 character.  If output buffer is NULL, ignore count */
    uint wclen = 0;
    while (*src && (!dest || wclen < destwords))
    {
        /* Get UTF-8 sequence length from 1st byte */
        sint utflen = LDAP_UTF8_CHARLEN2(src, utflen);

        if (!utflen || utflen >(sint)LDAP_MAX_UTF8_LEN) return 0;

        /* First byte minus length tag */
        unsigned char mask[] = { 0, 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
        wchar ch = (wchar)(src[0] & mask[utflen]);

        for (sint i = 1; i < utflen; ++i)
        {
            /* Subsequent bytes must start with 10 */
            if ((src[i] & 0xc0) != 0x80) return 0;

            ch <<= 6;			/* 6 bits of data in each subsequent byte */
            ch |= (wchar)(src[i] & 0x3f);
        }

        if (dest) dest[wclen] = ch;

        src += utflen;		/* Move to next UTF-8 character */
        wclen++;			/* Count number of wide chars stored/required */
    }

    /* Add null terminator if there's room in the buffer. */
    if (dest && wclen < destwords) dest[wclen] = 0;

    return wclen;
}



//////////////////////////////////////////////////////////////////////////
//	string object stores strings of characters
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_INLINE char* sx_string_clear(struct sx_string* obj)
{
    free(obj->text);
    obj->text = null;
    obj->len = 0;
    return null;
}

SEGAN_LIB_INLINE char* sx_string_set(struct sx_string* obj, const char* text)
{
    if (obj->text != text)
    {
        if (text)
        {
            obj->len = sx_str_len(text);
            obj->text = realloc(obj->text, obj->len + 1);
            memcpy(obj->text, text, obj->len);
            obj->text[obj->len] = 0;
        }
        else sx_string_clear(obj);
    }
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_format(struct sx_string* obj, const char* format, ...)
{
    //	trusted . safe and optimized function
    if (!format) return obj->text;
    va_list argList;
    va_start(argList, format);
    obj->len = _vscprintf(format, argList);
    obj->text = realloc(obj->text, obj->len + 1);
    vsnprintf_s(obj->text, obj->len, _TRUNCATE, format, argList);
    va_end(argList);
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_append(struct sx_string* obj, const char* str)
{
    //	trusted . safe and optimized function
    if (!str) return obj->text;
    sint slen = sx_str_len(str);
    if (!slen) return obj->text;
    obj->text = realloc(obj->text, obj->len + slen + 1);
    for (sint i = 0; i <= slen; obj->text[i + obj->len] = str[i], ++i);
    obj->len += slen;
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_insert(struct sx_string* obj, const char* str, sint _where)
{
    //	trusted . safe and optimized function
    if (!str) return obj->text;
    if (_where < 0) _where = 0;
    else if (_where > obj->len) _where = obj->len;
    sint slen = sx_str_len(str);
    obj->text = realloc(obj->text, obj->len + slen + 1);
    for (sint i = obj->len; i >= _where; obj->text[i + slen] = obj->text[i], --i);
    for (sint i = 0; i < slen; obj->text[i + _where] = str[i], ++i);
    obj->len += slen;
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_remove(struct sx_string* obj, sint index, sint count)
{
    //	trusted . safe and optimized function
    if (!obj->text || count < 1) return obj->text;
    if (index < 0) index = 0;
    else if (index >= obj->len) return obj->text;
    if (index + count > obj->len) count = obj->len - index;
    for (sint i = index + count; i < obj->len; obj->text[i - count] = obj->text[i], ++i);
    obj->len -= count;
    obj->text = realloc(obj->text, obj->len + 1);
    obj->text[obj->len] = 0;
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_copy_to(struct sx_string* obj, struct sx_string* dest, sint index, sint count)
{
    //	trusted . safe and optimized function
    if (!obj->text || count < 1) return obj->text;
    if (index < 0 || index >= obj->len) return obj->text;
    if (index + count > obj->len) count = obj->len - index;
    dest->text = realloc(dest->text, dest->len + count + 1);
    for (sint i = 0; i < count; dest->text[i + dest->len] = obj->text[i + index], ++i);
    dest->len += count;
    dest->text[dest->len] = 0;
    return obj->text;
}

SEGAN_LIB_INLINE sint sx_string_find(struct sx_string* obj, const char* substr, sint from)
{
    //	trusted . safe and optimized function
    if (!obj->text || !substr) return -1;
    if (from < 0) from = 0;
    char* p = strstr(obj->text + from, substr);
    return p ? ((sint)(p - obj->text)) : -1;
}

SEGAN_LIB_INLINE sint sx_string_find_back(struct sx_string* obj, const char* substr, sint from)
{
    //	trusted . safe and optimized function
    if (!obj->text || !substr) return -1;
    if (from < 0) from = 0;
    for (sint i = obj->len; i > 0; --i) {
        char* p = strstr(obj->text + i - from, substr);
        if (p) return ((sint)(p - obj->text));
    }
    return -1;
}

SEGAN_LIB_INLINE char* sx_string_replace(struct sx_string* obj, const char* what, const char* with)
{
    if (!obj->text || !what || !with) return obj->text;
    int lenwhat = (int)sx_str_len(what);
    int lenwith = (int)sx_str_len(with);
    int index = 0;
    while ((index = sx_string_find(obj, what, index)) > -1)
    {
        sx_string_remove(obj, index, lenwhat);
        sx_string_insert(obj, with, index);
        index += lenwith;
    }
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_revers(struct sx_string* obj, sint from, sint to)
{
    //	trusted . safe and optimized function
    if (from < 0) from = 0;
    if (from >= obj->len) from = obj->len - 1;
    if (to < 0) to = 0;
    if (to >= obj->len) to = obj->len - 1;
    if (from == to) return obj->text;
    if (from < to)
    {
        char c;
        while (from < to)
        {
            c = obj->text[from];
            obj->text[from++] = obj->text[to];
            obj->text[to--] = c;
        }
    }
    else
    {
        char c;
        while (to < from)
        {
            c = obj->text[to];
            obj->text[to++] = obj->text[from];
            obj->text[from--] = c;
        }
    }
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_trim(struct sx_string* obj)
{
    //	trusted . safe and optimized function
    if (!obj->text) return null;
    //  trim right side of text before left side may cause to decrease traverse spaces
    {
        char *end = &obj->text[obj->len - 1];
        while (*end == ' ' || *end == '\t') {
            --obj->len;
            --end;
        }
        *(++end) = 0;
    }
    //  trim left size of text
    {
        char *pos = obj->text;
        while (*pos == ' ' || *pos == '\t' || *pos == 0) {
            --obj->len;
            ++pos;
        }
        if (pos != obj->text) {
            char *first = obj->text;
            while (*pos)
                *(first++) = *(pos++);
            *first = 0;
        }
    }
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_make_upper(struct sx_string* obj)
{
    if (!obj->text) return null;
    for (char* cp = obj->text; *cp; ++cp)
        *cp = sx_str_upper(*cp);
    return obj->text;
}

SEGAN_LIB_INLINE char* sx_string_make_lower(struct sx_string* obj)
{
    if (!obj->text) return null;
    for (char* cp = obj->text; *cp; ++cp)
        *cp = sx_str_lower(*cp);
    return obj->text;
}

SEGAN_LIB_INLINE bool sx_string_is_path_style(struct sx_string* obj)
{
    if (!obj->text) return false;
    return (obj->text[obj->len - 1] == '/' || obj->text[obj->len - 1] == '\\');
}

SEGAN_LIB_INLINE bool sx_string_is_full_path(struct sx_string* obj)
{
    if (!obj->text) return false;
    return (obj->text[1] == ':' || (obj->text[0] == '\\' && obj->text[1] == '\\') || (obj->text[0] == '/' && obj->text[1] == '/'));
}

SEGAN_LIB_INLINE char* sx_string_make_path_style(struct sx_string* obj)
{
    if (!obj->text) return null;
    if (obj->text[obj->len - 1] != '/' && obj->text[obj->len - 1] != '\\') {
        obj->len++;
        obj->text = realloc(obj->text, obj->len + 1);
        obj->text[obj->len - 1] = PATH_PART;
        obj->text[obj->len] = 0;
    }
    return obj->text;
}


