#include "sxTool.h"
#include "../sxSystem/sxSystem.h"
#include <stdlib.h>

/*
this class can create and hold some static functions and help me to easy access to them
*/
class Tool_internal
{
public:
	static DWORD CRC32_InitTable(void)
	{
		// This is the official polynomial used by CRC-32 
		// in PKZip, WinZip and Ethernet. 
		ULONG ulPolynomial = 0x04c11db7;

		// 256 values representing ASCII character codes.
		for(int i = 0; i <= 0xFF; i++)
		{
			crc32_table[i]= CRC32_Reflect(i, 8) << 24;
			for (int j = 0; j < 8; j++)
				crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
			crc32_table[i] = CRC32_Reflect(crc32_table[i], 32);
		}

		return 0;
	}

	static UINT CRC32_GetCode(const WCHAR* text)
	{
		if (!text) return 0;

		// Start out with all bits set high.
		int len = (int)wcslen(text) * 2;
		if (len<1) return 0;

		// Save the text in the buffer.
		unsigned char* buffer = (unsigned char*)(LPCTSTR)text;

		// Perform the algorithm on each character
		// in the string, using the lookup table values.
		ULONG  ulCRC(0xffffffff);
		while(len--)
			ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++];

		// Exclusive OR the result with the beginning value.
		return ulCRC ^ 0xffffffff;
	}

	static ULONG CRC32_Reflect(ULONG ref, char ch)
	{
		ULONG value(0);

		// Swap bit 0 for bit 7
		// bit 1 for bit 6, etc.
		for(int i = 1; i < (ch + 1); i++)
		{
			if(ref & 1)
				value |= 1 << (ch - i);
			ref >>= 1;
		}
		return value;
	}

	static ULONG					crc32_table[256];
	static DWORD					LastID;
};
ULONG					Tool_internal::crc32_table[256];
DWORD					Tool_internal::LastID = Tool_internal::CRC32_InitTable();


//  my function to mix bytes
DWORD IDGenerator( BYTE R, BYTE G, BYTE B, BYTE A )
{
	return ((DWORD)((((A)&0xff)<<24)|(((R)&0xff)<<16)|(((G)&0xff)<<8)|((B)&0xff)));
	//return (DWORD) ( ((A&0xff)<<24) | (R&0xff)<<16) | ((G&0xff)<<8) | (B&0xff) );
}

FORCEINLINE float iround(float x)
{
	int t;
	__asm
	{
		fld  x
		fistp t
	}
	return static_cast<float>(t);
}


namespace sx { namespace cmn
{

	//////////////////////////////////////////////////////////////////////////
	//	STRING TOKER
	//////////////////////////////////////////////////////////////////////////
	StringToker::StringToker( void ): m_str(0), m_pointer(0)
	{

	}

	StringToker::~StringToker( void )
	{

	}

	void StringToker::SetString( const WCHAR* str )
	{
		m_str = (WCHAR*)str;
		m_pointer = m_str;
	}

	void StringToker::Reset( void )
	{
		m_pointer = m_str;
	}

	const str1024& StringToker::LookatNext( void )
	{
		static str1024 res;
		res.Clear();

		WCHAR* _pointer = m_pointer;
		while (_pointer && *_pointer)
		{
			if ( *_pointer == '"' )
			{
				_pointer++;

				while ( _pointer )
				{
					if ( *_pointer == '"' ) break;

					res.Append( *_pointer );
					_pointer++;
				}

				break;
			}

			if ( *_pointer == ' ' || *_pointer == '\t' || *_pointer == '\n' || *_pointer == '\r' )
			{
				if ( res.Length() )
				{
					break;
				}
				else
				{
					_pointer++;
					continue;
				}
			}

			if (*_pointer == '=' || *_pointer == '+' || *_pointer == '-' || *_pointer == '*' || *_pointer == '/' ||
				*_pointer == '{' || *_pointer == '}' || *_pointer == '[' || *_pointer == ']' ||
				*_pointer == '(' || *_pointer == ')')
			{
				if ( *_pointer == '-' )
				{
					WCHAR* c = _pointer; c++;
					if ( isdigit( *c ) )
					{
						res.Append( *_pointer );
						_pointer++;
						continue;
					}

				}
				if ( !res.Length() )
				{
					res = *_pointer;
					_pointer++;
				}
				break;
			}

			res.Append( *_pointer );
			_pointer++;
		}

		return res;
	}

	const str1024& StringToker::Next( void )
	{
		static str1024 res;
		res.Clear();

		while (m_pointer && *m_pointer)
		{
			if ( *m_pointer == '"' )
			{
				m_pointer++;
				
				while ( m_pointer )
				{
					if ( *m_pointer == '"' ) break;
					
					if ( res.Length() < 1022 )
						res.Append( *m_pointer );

					m_pointer++;
				}
				m_pointer++;

				break;
			}

			if ( *m_pointer == ' ' || *m_pointer == '\t' || *m_pointer == '\n' || *m_pointer == '\r' )
			{
				if ( res.Length() )
				{
					break;
				}
				else
				{
					m_pointer++;
					continue;
				}
			}

			if (*m_pointer == '=' || *m_pointer == '+' || *m_pointer == '-' || *m_pointer == '*' || *m_pointer == '/' ||
				*m_pointer == '{' || *m_pointer == '}' || *m_pointer == '[' || *m_pointer == ']' ||
				*m_pointer == '(' || *m_pointer == ')')
			{
				if ( *m_pointer == '-' )
				{
					WCHAR* c = m_pointer; c++;
					if ( isdigit( *c ) )
					{
						res.Append( *m_pointer );
						m_pointer++;
						continue;
					}

				}
				if ( !res.Length() )
				{
					res = *m_pointer;
					m_pointer++;
				}
				break;
			}

			res.Append( *m_pointer );
			m_pointer++;
		}

		return res;
	}

	FORCEINLINE WCHAR* StringToker::GetCurChar( void )
	{
		return m_pointer;
	}

	//////////////////////////////////////////////////////////////////////////
	//  STRING LIST
	//////////////////////////////////////////////////////////////////////////
	StringList::StringList()
	{

	}

	StringList::~StringList()
	{
		Clear();
	}

	void StringList::Clear( void )
	{
		for (int i=0; i<m_sList.Count(); i++)
		{
			String* str = m_sList[i];
			sx_delete(str);
		}
		m_sList.Clear();
	}

	int StringList::Count( void )
	{
		return m_sList.Count();
	}

	bool StringList::IsEmpty( void )
	{
		return m_sList.IsEmpty();
	}

	int StringList::IndexOf( const WCHAR* str )
	{
		for (int i=0; i<m_sList.Count(); i++)
		{
			String* pstr = m_sList[i];
			if ( *pstr == str )
				return i;
		}
		return -1;
	}

	void StringList::PushBack( const WCHAR* str )
	{
		if ( !str ) return;
		String* item = sx_new(String);
		item->SetText(str);
		m_sList.PushBack(item);
	}

	void StringList::PushFront( const WCHAR* str )
	{
		if ( !str ) return;
		String* item = sx_new(String);
		item->SetText(str);
		m_sList.PushFront(item);
	}

	void StringList::Pop( void )
	{
		if ( !m_sList.Count() ) return;
		int index = m_sList.Count() - 1;
		String* pstr = m_sList[index];
		m_sList.RemoveByIndex(index);
		sx_delete(pstr);
	}

	void StringList::Delete( int Index )
	{
		if ( Index < 0 || Index >= m_sList.Count() ) return;
		String* pstr = m_sList[Index];
		m_sList.RemoveByIndex(Index);
		sx_delete(pstr);
	}

	String* StringList::Top( void )
	{
		sx_assert( m_sList.Count() );
		int index = Count() - 1;
		if ( index >=0 )
			return m_sList[index];
		else
			return NULL;
	}

	String* StringList::At( int Index )
	{
		sx_assert(Index>=0 && Index<m_sList.Count());
		if ( Index>=0 && Index<m_sList.Count() )
			return m_sList[Index];
		else
			return NULL;
	}

	void StringList::SaveToFile( const WCHAR* FileName )
	{
		sx::sys::FileStream MyFile;
		MyFile.Open(FileName, FM_CREATE);

		char isUnicode[2] = {(char)-1, (char)-2};
		MyFile.Write(isUnicode, 2);

		WCHAR c[2] = {'\r', '\n'};	
		for (int i=0; i<Count(); i++)
		{
			MyFile.Write(At(i)->Text(), At(i)->Length() * sizeof(WCHAR));
			MyFile.Write(c, 2 * sizeof(WCHAR));
		}
	}

	void StringList::LoadFromFile( const WCHAR* FileName )
	{
		if (!sx::sys::FileExist(FileName))
			return;

		sx::sys::FileStream MyFile;
		MyFile.Open(FileName, FM_OPEN_READ | FM_SHARE_READ);

		int fsize = MyFile.Size();
		if (fsize<1) return;

		// check the Unicode file
		char testChar[2] = {0, 0};
		MyFile.Read(testChar, 2);
		bool isUnicode = testChar[0] == -1 && testChar[1] == -2;
		if ( !isUnicode ) MyFile.SetPos(0);

		
		WCHAR* tmp = (WCHAR*)sx_mem_alloc(fsize);
		if ( isUnicode )
		{
			int pos = 0;
			WCHAR c = 0;
			while ( MyFile.Read( &c, 2 ) > 1 )
			{
				if ( c == '\n' || c == '\r' )
				{
					if ( pos )
					{
						tmp[pos] = 0;
						PushBack( tmp );
						pos = 0;
					}
				}
				else
				{
					tmp[pos++] = c;
				}
			}

			if (pos)
			{
				tmp[pos] = 0;
				PushBack( tmp );
			}
		}
		else
		{
			int pos = 0;
			char c = 0;
			while ( MyFile.Read( &c, 1 ) )
			{
				if ( c == '\n' || c == '\r' )
				{
					if ( pos )
					{
						tmp[pos] = 0;
						PushBack( tmp );
						pos = 0;
					}
				}
				else
				{
					tmp[pos++] = c;
				}
			}

			if (pos)
			{
				tmp[pos] = 0;
				PushBack( tmp );
			}
		}
		sx_mem_free(tmp);
	}

	String* StringList::operator[]( int index )
	{
		sx_assert(index>=0 && index<m_sList.Count());
		if ( index>=0 && index<m_sList.Count() )
			return m_sList[index];
		else
			return NULL;
	}

	void StringList::Swap( int index1, int index2 )
	{
		m_sList.Swap( index1, index2 );
	}


	//////////////////////////////////////////////////////////////////////////
	//  STRING OBJECT LIST
	//////////////////////////////////////////////////////////////////////////
	StringObjectList::Item::Item( void ): object(NULL), option(0) {}

	StringObjectList::StringObjectList( void )
	{

	}

	StringObjectList::~StringObjectList( void )
	{
		Clear();
	}

	void StringObjectList::Clear( void )
	{
		for (int i=0; i<m_List.Count(); i++)
			sx_delete_and_null(m_List[i]);
		m_List.Clear();
	}

	int StringObjectList::Count( void )
	{
		return m_List.Count();
	}

	bool StringObjectList::IsEmpty( void )
	{
		return m_List.IsEmpty();
	}

	int StringObjectList::IndexOfObject( void* obj )
	{
		for (int i=0; i<m_List.Count(); i++)
		{
			if ( m_List[i]->object == obj )
				return i;
		}
		return -1;
	}

	int StringObjectList::IndexOfString( const WCHAR* str )
	{
		for (int i=0; i<m_List.Count(); i++)
		{
			if ( m_List[i]->text == str )
				return i;
		}
		return -1;
	}

	void StringObjectList::PushBack( const WCHAR* str, void* obj )
	{
		if ( !str ) return;
		Item* item = sx_new( Item );
		item->text = str;
		item->object = obj;
		m_List.PushBack(item);
	}

	void StringObjectList::PushFront( const WCHAR* str, void* obj )
	{
		if ( !str ) return;
		Item* item = sx_new( Item );
		item->text = str;
		item->object = obj;
		m_List.PushFront(item);
	}

	void StringObjectList::Pop( void )
	{
		Delete(Count() - 1);		
	}

	void StringObjectList::Delete( int Index )
	{
		if (Index<0 || Index>=m_List.Count()) return;
		sx_delete_and_null( m_List[Index] );
		m_List.RemoveByIndex(Index);
	}

	StringObjectList::Item& StringObjectList::Top( void )
	{
		return *m_List[m_List.Count() - 1];
	}

	StringObjectList::Item& StringObjectList::At( int Index )
	{
		return *m_List[Index];
	}

	StringObjectList::Item& StringObjectList::operator[]( int index )
	{
		return *m_List[index];
	}



//////////////////////////////////////////////////////////////////////////
//	SOME USEFUL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
	SEGAN_API UINT GetCRC32( const WCHAR* text )
	{
		return  Tool_internal::CRC32_GetCode(text);
	}

	SEGAN_API void ID_Generalize( DWORD StartFrom )
	{
		Tool_internal::LastID = StartFrom;
	}

	SEGAN_API DWORD ID_Generate( BYTE GroupID /*= 0xFF*/ )
	{
		BYTE id_R, id_G, id_B, id_A;
		ID_Separate(Tool_internal::LastID, id_B, id_G, id_R, id_A);

		id_R++;
		if (!id_R)
		{
			id_R = 1;
			id_G++;
		
			if (!id_G)
			{
				id_G = 1;
				id_B++;

				if (!id_B)
				{
					id_B = 1;
				}
			}
		}

		return IDGenerator(id_B, id_G, id_R, GroupID);
	}

	SEGAN_API BYTE ID_GetGroup( DWORD ID )
	{
		return BYTE(ID >> 24);
	}

	SEGAN_API void ID_Separate( IN const DWORD ID, OUT BYTE& R, OUT BYTE& G, OUT BYTE& B, OUT BYTE& A )
	{
		R = BYTE(ID >> 16);
		G = BYTE(ID >>  8);
		B = BYTE(ID);
		A = BYTE(ID >> 24);
	}

	SEGAN_API FORCEINLINE void Randomize( void )
	{
		srand((UINT)sys::GetSysTime());
	}

	SEGAN_API FORCEINLINE DWORD Random( int iMax )
	{
		return (DWORD)floor( ( (double)rand() / (double)RAND_MAX ) * (double)(iMax+1) );
	}

	SEGAN_API FORCEINLINE float Random( float fMax )
	{
		return ((float)rand() / (float)RAND_MAX ) * fMax;
	}

	SEGAN_API FORCEINLINE float Round( float value )
	{
		return iround(value);
	}

	SEGAN_API FORCEINLINE int Power( int v, int p )
	{
		int r = 1;
		for (int i=0; i<p; i++)
		{
			r *= v;
		}
		return r;
	}

	SEGAN_API FORCEINLINE float ViewDistanceByFrustom( const Frustum& frustun, const float camFOV, const float3& position, const float objRadius )
	{
		// compute view parameter use to set LOD of the objects
		float vp = frustun.p0.Distance( position ) - objRadius;
		float cosFOV = 1.0f - cos( camFOV );
		return vp * cosFOV ;
	}

	SEGAN_API FORCEINLINE float ViewDistanceByCamera( const float3& camEye, const float3& camAt, const float camFOV, const float3& position, const float objRadius )
	{
		// compute view parameter use to set LOD of the objects
		// note that we need distance of objects and the near plane instead of camera position
		float3 dir( camAt.x - camEye.x, camAt.y - camEye.y, camAt.z - camEye.z );
		dir.Normalize( dir );
		float vp = dir.Dot( position ) - dir.Dot( camEye ) - objRadius;
		float cosFOV = 1.0f - cos( camFOV );
		return vp * cosFOV ;
	}

	SEGAN_API void String_Save( String& _string, PStream _stream )
	{
		if (!_stream)
			return;

		int len = _string.Length();
		_stream->Write(&len, sizeof(len));
		if (len)
			_stream->Write(_string.Text(), len * sizeof(WCHAR));
	}

	SEGAN_API void String_Save( String& _string, const WCHAR* FileName, bool unicode /*= true*/ )
	{
		if (!FileName)
			return;

		sx::sys::FileStream MyFile;
		MyFile.Open(FileName, FM_CREATE);

		if ( unicode )
		{
			char isUnicode[2] = {(char)-1, (char)-2};
			MyFile.Write(isUnicode, 2);

			MyFile.Write(_string.Text(), _string.Length() * sizeof(WCHAR));
		}
		else
		{
			for (int i=0; i<_string.Length(); i++)
			{
				char c = (char)_string[i];
				MyFile.Write(&c, 1);
			}
		}

		MyFile.Close();
	}

	SEGAN_API void String_Load( String& _string, PStream _stream )
	{
		int len = 0;
		_stream->Read(&len, sizeof(len));

		if (len && len <= (int)_stream->Size())
		{
			WCHAR* tmp = (WCHAR*)sx_mem_alloc((len+1) * sizeof(WCHAR));
			_stream->Read(tmp, len * sizeof(WCHAR));
			tmp[len]=0;
			_string.SetText(tmp);
			sx_mem_free(tmp);
		}
	}

	SEGAN_API void String_Load( String& _string, const WCHAR* FileName )
	{
		sx_callstack_param(String_Load(String&, FileName=%s), FileName);

		if (!FileName)
			return;

		if (!sx::sys::FileExist(FileName))
			return;

		sx::sys::FileStream MyFile;
		MyFile.Open(FileName, FM_OPEN_READ | FM_SHARE_READ);
		int fsize = MyFile.Size();

		if (fsize<1) 
			return;

		WCHAR *c = NULL;

		// check the Unicode file
		char isUnicode[2] = {0, 0};
		MyFile.Read(isUnicode, 2);
		if (isUnicode[0] == -1 && isUnicode[1] == -2)	// text file format is Unicode
		{
			fsize-=2;
			int len = fsize / sizeof(WCHAR) + 1;

			c = (WCHAR*)sx_mem_alloc(len * sizeof(WCHAR));
			MyFile.Read(c, fsize);
			c[len-1] = 0;
		}
		else	// text file format is NOT Unicode
		{
			MyFile.SetPos(0);

			fsize++;
			char* tmp = (char*)sx_mem_alloc(fsize);
			MyFile.Read(tmp, fsize);
			tmp[fsize-1] = 0;

			c = (WCHAR*)sx_mem_alloc(fsize * sizeof(WCHAR));
			for (int i=0; i<fsize; i++)
				c[i] = (WCHAR)tmp[i];

			sx_mem_free(tmp);
		}

		_string.SetText(c);
		sx_mem_free(c);
	}

	SEGAN_API void String_GetLineList( const WCHAR* _string, PStringList psList )
	{
		String l;
		String f = _string;

		int p; 
		while ( ( p = f.Find(L"\n") ) > -1 )
		{
			l = L"";
			f.CopyTo(l, 0, p-1);
			f.Delete(0, p+1);
			psList->PushBack(*l);
		}

		if (f.Length())
			psList->PushBack(*f);
	}

	SEGAN_API UINT String_GetValue( String& _string, const WCHAR* Key, String& Value )
	{
		Value = "";

		int p;
		if ( ( p = _string.Find(Key) ) < 0 )
			return 0;

		if ( ( p = _string.Find(L"=", p+1) ) < 0 )
			return 0;

		int i = 0;
		bool done = false, comment = false;
		for (i=p+1; i<(int)_string.Length(); i++)
		{
			if (done && !comment && (_string[i]==' ' || _string[i]=='	'))
				break;

			if (_string[i]!=' ' && _string[i]!='	')
				done = true;

			if (_string[i]=='"')
			{
				if (!comment)
				{
					comment = true;
					p++;
				}
				else
				{
					i--;
					break;
				}
			}

			if (!done)
				p++;
		}

		_string.CopyTo(Value, p+1, i-p);

		return i+1;
	}

	SEGAN_API void String_CheckExten( String& sFileName, const WCHAR* sExten )
	{
		str512 str = sFileName.Text();
		str.ExtractFileExtension();
		if( str != sExten )
			sFileName << '.' << sExten;
	}


} } // namespace sx
