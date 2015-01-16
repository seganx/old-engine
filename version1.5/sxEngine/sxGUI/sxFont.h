/********************************************************************
	created:	2010/12/06
	filename: 	sxFont.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of Font that hold font data
				from bitmap font. this font will use in other controls.
*********************************************************************/
#ifndef GUARD_sxFont_HEADER_FILE
#define GUARD_sxFont_HEADER_FILE

#include "sxControl.h"


//! helper declaration
typedef Map<DWORD, GUIFontChar*> GUICharacterMap;

namespace sx { namespace gui {

	// forward declaration
	class Label;

	//! class of Font that hold font data from bitmap font. this font will use in other controls.
	class SEGAN_API Font
	{
		friend class Label;
		friend class TextEdit;

		SEGAN_STERILE_CLASS(Font);

	protected:
		Font(void);
		~Font(void);

	public:
		//! return reference to the font description
		GUIFontDesc& GetDesc(void);

		//! save font data to the stream
		void Save(Stream& stream);

		//! load font from stream
		void Load(Stream& stream);

		//! set source file name of the font
		void SetSource(const WCHAR* src);

		//! return the source of the font
		const WCHAR* GetSource(void);

		//! throw out pointer to the character description by given ID. return false if ID is not valid
		bool GetChar(const DWORD charID, OUT PGUIFontChar& pchar);

		/*! throw out pointer to the character description by given ID and return false if ID is not valid.
		this function also support Persian language. */
		bool GetChar(const WCHAR* string, const int charIndex, OUT PGUIFontChar& pchar, bool reversed = false);

	protected:
		void CleanUp(void);

		String					m_src;			//  source file of the font
		GUICharacterMap			m_chars;		//  map of the characters
		d3d::PTexture			m_texture;		//  texture of the font
		GUIFontDesc				m_desc;			//  description of the font
		int						m_refCount;		//  internal reference counter

	public:
		class SEGAN_API Manager
		{
		public:

			/*! 
			return true if font of the 'src' is exist in the manager and throw out founded one.
			NOTE: this function DO NOT increase the internal reference counter of the object. so releasing
			the object after the work finished may cause to destroy the object.
			*/
			static bool Exist(OUT Font*& pFont, const WCHAR* src);

			/*!
			search for font of the src. if not exist in manager, create and return new one.
			NOTE: this function increase the internal reference counter of the object. so release
			the object after the work finished.
			*/
			static bool Get(OUT Font*& pFont, const WCHAR* src);

			//! create a new font independent of the others.
			static bool Create(OUT Font*& pFont, const WCHAR* src);

			//! decrease the internal reference counter and destroy the font if reference counter became zero.
			static void Release(Font*& pFont);

			//! clear all objects in the manager
			static void ClearAll(void);
		};

	};

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxFont_HEADER_FILE