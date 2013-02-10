/********************************************************************
	created:	2010/08/9
	filename: 	sxTool.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some useful structures and functions
*********************************************************************/
#ifndef GUARD_sxTool_HEADER_FILE
#define GUARD_sxTool_HEADER_FILE

#include "../../sxLib/lib.h"
#include "sxCommon_def.h"
#include "sxMath.h"


namespace sx { namespace cmn
{
	/*
	String tokenizer takes words in a string
	*/
	class SEGAN_API StringToker
	{
		SEGAN_STERILE_CLASS(StringToker);

	public:
		StringToker(void);
		~StringToker(void);

		//! set string to tokenizer
		void SetString(const WCHAR* str);

		//! reset from head of string
		void Reset(void);

		//! return token in the next step
		const str1024& LookatNext(void);

		//! return next token from string
		const str1024& Next(void);

		//! return current pointer to character
		WCHAR* GetCurChar(void);

	private:
		WCHAR*			m_str;
		WCHAR*			m_pointer;
	};

	/*
	String List is a special class to hold a list of strings
	*/
	class SEGAN_API StringList
	{
		SEGAN_STERILE_CLASS(StringList);

	public:
		StringList(void);
		~StringList(void);

		void Clear(void);
		int Count(void);
		bool IsEmpty(void);

		void Swap(int index1, int index2);
		int IndexOf(const WCHAR* str);
		void PushBack(const WCHAR* str);
		void PushFront(const WCHAR* str);
		void Pop(void);
		void Delete(int Index);
		String& Top(void);
		String& At(int Index);
		void SaveToFile(const WCHAR* FileName);
		void LoadFromFile(const WCHAR* FileName);

		String& operator[] (int index);
	private:
		Array<String*> m_sList;		//  list of pointers of strings
	};
	typedef StringList * PStringList;

	/*
	String Object List is a special class to hold a list of strings with a pointer to an object
	*/
	class SEGAN_API StringObjectList
	{
		SEGAN_STERILE_CLASS(StringObjectList);

	public:
		struct Item{
			String		text;
			void*			object;
			DWORD			option;
			Item(void);
		};

		StringObjectList(void);
		~StringObjectList(void);

		void Clear(void);
		int Count(void);
		bool IsEmpty(void);

		int IndexOfObject(void* obj);
		int IndexOfString(const WCHAR* str);
		void PushBack(const WCHAR* str, void* obj);
		void PushFront(const WCHAR* str, void* obj);
		void Pop(void);
		void Delete(int Index);
		Item& Top(void);
		Item& At(int Index);

		Item& operator[] (int index);
	private:
		Array<Item*>	m_List;		//  list of pointers of strings
	};

	//////////////////////////////////////////////////////////////////////////
	//	SOME USEFUL FUNCTIONS
	//////////////////////////////////////////////////////////////////////////
	SEGAN_API UINT	GetCRC32(const WCHAR* text);

	SEGAN_API void  ID_Generalize(DWORD StartFrom);
	SEGAN_API DWORD ID_Generate(BYTE GroupID = 0xFF);
	SEGAN_API BYTE  ID_GetGroup(DWORD ID);
	SEGAN_API void  ID_Separate(IN const DWORD ID, OUT BYTE& R, OUT BYTE& G, OUT BYTE& B, OUT BYTE& A);

	SEGAN_API void  Randomize(void);
	SEGAN_API DWORD Random(int   iMax);
	SEGAN_API float Random(float fMax);

	SEGAN_API float Round(float value);
	SEGAN_API int	Power(int v, int p);
	SEGAN_API float ViewDistanceByFrustom(const Frustum& frustun, const float camFOV, const float3& position, const float objRadius);
	SEGAN_API float ViewDistanceByCamera(const float3& camEye, const float3& camAt, const float camFOV, const float3& position, const float objRadius);

	SEGAN_API void String_Save(String& _string, PStream _stream);
	SEGAN_API void String_Load(String& _string, PStream _stream);

	SEGAN_API void String_Save(String& _string, const WCHAR* FileName, bool unicode = true);
	SEGAN_API void String_Load(String& _string, const WCHAR* FileName);

	SEGAN_API void String_GetLineList(const WCHAR* _string, PStringList psList);
	SEGAN_API UINT String_GetValue(String& _string, const WCHAR* Key, String& Value);		//  TODO : use better algorithm
	SEGAN_API void String_CheckExten(String& sFileName, const WCHAR* sExten);				//  TODO : use better algorithm

} } // namespace sx { namespace sys

#endif // GUARD_sxTool_HEADER_FILE
