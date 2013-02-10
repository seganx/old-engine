/********************************************************************
created:	2011/02/24
filename: 	sxListItem.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain an Item class that use GUI of the 
			SeganX engine. this item will be used in lists, gridsand etc.
*********************************************************************/
#ifndef SX_sxListItem_HEADER_FILE
#define SX_sxListItem_HEADER_FILE

#include "ImportEngine.h"

namespace sx { namespace gui {

	/* !
	this class contain a background panel, image panel and a label. this will be
	used in list boxes, grid boxes and etc.
	*/
	class ItemUI : public Form
	{
		
		struct ItemData
		{
			int		index;
			void*	userData;
			DWORD	option;
		};

	public:
		ItemUI(void);
		~ItemUI(void);

		//! set size of the item
		void SetSize(float width, float height, bool displayImage = true);

		//! set parent control for the list box
		void SetParent(PControl parent);

		//! set the text of the label
		void SetText(const WCHAR* text);

		//! return the text of the label
		const WCHAR* GetText(void);

		//! set the texture of the item
		void SetImageTexture(const WCHAR* src);

		//! return the source of the image texture
		const WCHAR* GetImageTexture(void);

		//! reference to the position of the item
		float3& Position(void);

		//! reference to the color of the item
		D3DColor& Color(void);

		//! return the back ground object
		PPanel Back(void);

		//! return the image panel
		PPanel Image(void);

		//! return the label of item
		PLabel Label(void);

		//! reference to the item data structure
		ItemData& Data(void);

	public:
		PPanel				m_pBack;	//  panel of background
		PPanel				m_pImage;	//  panel of image box
		PLabel				m_pLabel;	//  label of the item
		ItemData			m_Data;		//  data of the item
	};


}} // namespace sx { namespace gui {

#endif // SX_sxListItem_HEADER_FILE
