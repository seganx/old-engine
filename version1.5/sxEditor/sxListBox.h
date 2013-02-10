/********************************************************************
	created:	2011/02/07
	filename: 	sxListBox.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a list box class that use GUI of the
				SeganX engine.
*********************************************************************/
#ifndef SX_ListBox_HEADER_FILE
#define SX_ListBox_HEADER_FILE

#include "ImportEngine.h"

namespace sx { namespace gui {

	//  forward declaration
	class ItemUI;

	/*
	Listbox control is just like as windows standard list 
	box with multi select features and etc.
	*/
	class ListBox: public Form
	{
	
		typedef class Item {
		public:
			Item(void);
			Item(Item& item);
			~Item(void);
			void SetTexture(const WCHAR* src);
			const WCHAR* GetTexture(void);
			bool operator==(Item& t);

			String			Text;
			void*			UserData;
			DWORD			Option;
			int				Level;

		private:
			d3d::PTexture	m_pTexture;

		} *PItem;
		typedef Array<Item*>		itemArray;
		typedef Array<ItemUI*>		itemUIArray;

	public:
		ListBox(void);
		virtual ~ListBox(void);

		//! set size of the list box
		void SetSize(float width, float height, float itemHeight = 18.0f, bool displayImage = true);

		//! return size of list box
		float2 GetSize(void);

		//! set font of the list box
		void SetFont(const WCHAR* fontName);

		//! set parent control for the list box
		void SetParent(PControl parent);

		//! update list box should be call in forms
		void Update(float elpsTime);

		//! return true if the list box is focused
		bool GetFocused(void);

		//! set this control focused
		void SetFocused(void);

		//! return back ground panel
		sx::gui::PPanel GetBack(void);

		//! return scroll bar
		sx::gui::PTrackBar GetScroll(void);

		//! reference to the position
		float3& Position(void);

		//! return current item index
		int GetItemIndex(void);

		//! set new item index value
		void SetItemIndex(int index);

		//! return number of items in the list box
		int Count(void);

		//! clear all items from the list box
		void Clear(void);

		//! add a new item to the list box
		void Add(const WCHAR* itemText, const WCHAR* itemImage, void* userData = NULL, const int level = 0);

		//! remove an item from the list box
		void Remove(int index);

		//! return the string of an item in the list box by specifying index
		const WCHAR* GetText(int index);

		//! return the image of an item in the list box by specifying index
		const WCHAR* GetImage(int index);

		//! return user data of an item in the list box by specifying index
		void* GetUserData(int index);

		//! update current view list. if you change items data by direct referencing to them, use this function to update the view
		void CommitChanges(void);

		//! set callback function for mouse click
		void SetOnMouseClick(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for key down
		void SetOnKeyDown(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for selection changes
		void SetOnSelect(PForm pForm, GUICallbackEvent pFunc);

		//! reference to the item by index
		Item& operator[](int index);

		//! return reference to specified item. return unused temp item for invalid index
		Item& GetItem(int index);

		//! return the UI item which used to display an item. return null for invalid index
		ItemUI* GetUI(int index);

		//! select item by moving mouse
		void SelectItemByMouseMove(bool v);

	public:
		void OnMouseMove(PControl Sender);
		void OnItemClick(PControl Sender);
		void OnItemImageClick(PControl Sender);
		void OnKeyDown(PControl Sender);

		String					m_Font;				//  the font name of items

		PPanel					m_pBack;			//  main panel
		PPanel					m_pHolder;			//  hold created items and clip them
		PTrackBar				m_pScroll;			//  scroll bar inside of the list
		itemUIArray				m_UI;				//  list of UI to show the items

		itemArray				m_Items;			//  array of items
		float					m_ItemHeight;		//  size of each item in the list
		int						m_ItemIndex;		//  index of selected item
		int						m_TopOffset;		//  use to locate labels
		DWORD					m_Option;			//  some options of list box

		GUICallbackFuncPtr		m_OnKeyboard;		//  call back function for keyboard event
		GUICallbackFuncPtr		m_OnClick;			//  call back function for mouse event
		GUICallbackFuncPtr		m_OnSelect;			//  call back function for selection changes
		
	};
	typedef ListBox *PListBox;

} } // namespace sx { namespace gui {

#endif	//  SX_ListBox_HEADER_FILE