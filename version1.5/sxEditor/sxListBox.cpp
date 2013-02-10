#include "sxListBox.h"
#include "sxItemUI.h"
#include "EditorUI.h"

#define LISTBOX_MULTISELECT			0x10000000
#define LISTBOX_DISPLAYIMAGE		0x20000000
#define LISTBOX_MOUSEMOVESELECT		0x40000000

#define ITEM_SELECTED				0x00100000		//	item has been selected
#define ITEM_EXPANDED				0x00200000		//	expand item to show the childes

#define LISTBOX_LEVEL_OFFSET		24.0f

static const D3DColor selectedColor(1.0f, 1.0f, 1.0f, 0.5f);
static const D3DColor deselectColor(0.0f, 0.0f, 0.0f, 0.0f);
static const D3DColor itemindxColor(1.0f, 1.0f, 1.0f, 1.0f);

static int multiselect_fromindex = -1;

namespace sx { namespace gui {

	//////////////////////////////////////////////////////////////////////////
	//	ITEM
	//////////////////////////////////////////////////////////////////////////
	ListBox::Item::Item( void ): m_pTexture(NULL), UserData(NULL), Option(0), Level(0) {}
	ListBox::Item::~Item( void ) { Text.Clear(); SetTexture(NULL); }

	ListBox::Item::Item( Item& item )
	{
		Text		= item.Text;
		UserData	= item.UserData;
		Option		= item.Option;
		Level		= item.Level;

		sx::d3d::Texture::Manager::AddRef( item.m_pTexture );
		m_pTexture	= item.m_pTexture;
		if (m_pTexture) m_pTexture->Activate();
	}

	FORCEINLINE void ListBox::Item::SetTexture( const WCHAR* src )
	{
		if (src && m_pTexture && (wcscmp(m_pTexture->GetSource(), src) == 0)) 
			return;

		d3d::PTexture TX = NULL;
		if (d3d::Texture::Manager::Get(TX, src)) TX->Activate();

		if (m_pTexture)
		{
			m_pTexture->Deactivate();
			d3d::Texture::Manager::Release(m_pTexture);
		}
		m_pTexture = TX;
	}

	FORCEINLINE const WCHAR* ListBox::Item::GetTexture( void )
	{
		if (m_pTexture)
			return m_pTexture->GetSource();
		else
			return NULL;
	}

	FORCEINLINE bool ListBox::Item::operator==( Item& t )
	{
		return Text	== t.Text && UserData == t.UserData && Option == t.Option && Level == t.Level;
	}

	//////////////////////////////////////////////////////////////////////////
	//	LIST BOX
	//////////////////////////////////////////////////////////////////////////
	ListBox::ListBox(void): Form(), m_pBack(NULL), m_ItemIndex(-1), m_TopOffset(0), m_Option(0)
	{
		SetFont( EditorUI::GetDefaultFont(8) );

		m_pBack = static_cast<PPanel>(sx::gui::Create(GUI_PANEL));
		m_pBack->GetElement(0)->Color() = EditorUI::GetListBackColor();
		m_pBack->SetUserData(this);

		m_pHolder = static_cast<PPanel>(sx::gui::Create(GUI_PANEL));
		m_pHolder->SetParent(m_pBack);
		m_pHolder->AddProperty(SX_GUI_PROPERTY_CLIPCHILDS | SX_GUI_PROPERTY_PROCESSKEY | SX_GUI_PROPERTY_ACTIVATE);
		m_pHolder->SetOnMouseClick(this, (GUICallbackEvent)&ListBox::OnItemClick);
		m_pHolder->SetOnKeyDown(this, (GUICallbackEvent)&ListBox::OnKeyDown);
		m_pHolder->SetOnMouseWheel(this, (GUICallbackEvent)&ListBox::OnKeyDown);
		m_pHolder->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
		m_pHolder->SetUserData(this);

		m_pScroll = static_cast<PTrackBar>(sx::gui::Create(GUI_TRACKBAR));
		m_pScroll->SetParent(m_pBack);
		m_pScroll->GetElement(0)->SetTextureSrc( EditorUI::GetScrollTexture(0) );
		m_pScroll->GetElement(1)->SetTextureSrc( EditorUI::GetScrollTexture(1) );
		m_pScroll->AddProperty(SX_GUI_PROPERTY_PROCESSKEY);
		m_pScroll->SetOnKeyDown(this, (GUICallbackEvent)&ListBox::OnKeyDown);
		m_pScroll->SetOnMouseWheel(this, (GUICallbackEvent)&ListBox::OnKeyDown);
		m_pScroll->Rotation().z = -sx::math::PIDIV2;
		m_pScroll->SetUserData(this);
		
		SetSize( 40.0f, 100.0f );
	}

	ListBox::~ListBox(void)
	{
		for (int i=0; i<m_UI.Count(); i++)
			sx_delete( m_UI[i] );

		for (int i=0; i<m_Items.Count(); i++)
			sx_delete(m_Items[i]);

		m_pBack->SetParent(NULL);
		SEGAN_GUI_DELETE( m_pBack );
	}

	void ListBox::SetSize( float width, float height, float itemHeight /*= 18.0f*/, bool displayImage /*= true*/ )
	{
		if (displayImage)
			SEGAN_SET_ADD(m_Option, LISTBOX_DISPLAYIMAGE);
		else
			SEGAN_SET_REM(m_Option, LISTBOX_DISPLAYIMAGE);
		m_ItemHeight = itemHeight;

		m_pBack->SetSize( float2(width, height) );
		m_pHolder->SetSize( float2(width - 16.0f, height-1.0f) );
		m_pHolder->Position().x = -8.0f;
		m_pScroll->SetSize( float2(height, 16.0f) );
		m_pScroll->Position().x = width * 0.5f - 8.0f;

		//////////////////////////////////////////////////////////////////////////
		//  calculate number of UIs to show the items in the list
		int m = m_UI.Count();
		int n = int(height / m_ItemHeight) + 2;

		//  remove further UIs that are not usable
		for (int i=0; i<(m-n); i++)
		{
			int k = m_UI.Count() - 1;
			sx_delete_and_null( m_UI[k] );
			m_UI.RemoveByIndex(k);
		}
		
		//  create shortcoming UIs to show the items
		for (int i=0; i<(n-m); i++)
		{
			ItemUI* tmp = sx_new( ItemUI );
			tmp->SetParent(m_pHolder);
			tmp->Color() = deselectColor;
			tmp->Back()->GetElement(0)->SetTextureSrc( EditorUI::GetListSelectedTexture() );
			tmp->Label()->SetFont(m_Font);
			tmp->Label()->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
			SEGAN_GUI_SET_ONCLICK( tmp->m_pImage, ListBox::OnItemImageClick );
			m_UI.PushBack(tmp);
		}

		for (int i=0; i<m_UI.Count(); i++)
		{
			m_UI[i]->SetSize( width-16.0f, itemHeight, displayImage );
			m_UI[i]->Position().y = ( height-1.0f)*0.5f - itemHeight*0.5f - i*itemHeight;
		}

		CommitChanges();
	}

	float2 ListBox::GetSize( void )
	{
		return m_pBack->GetSize();
	}

	void ListBox::SetFont( const WCHAR* fontName )
	{
		m_Font = fontName;

		for (int i=0; i<m_UI.Count(); i++)
			m_UI[i]->Label()->SetFont(fontName);
	}

	void ListBox::SetParent( sx::gui::PControl parent )
	{
		m_pBack->SetParent(parent);
	}

	void ListBox::Update( float elpsTime )
	{
		float v = m_pScroll->GetBlendingValue();

		//  update UIs position
		float f = (v - int(v)) * m_ItemHeight;
		for (int i=0; i<m_UI.Count(); i++)
			m_UI[i]->Back()->PositionOffset().y = f;

		m_pBack->GetElement(0)->Color().a = 
			EditorUI::GetListBackColor().a - 0.2f*(
			Control::GetFocusedControl() != m_pHolder && 
			Control::GetFocusedControl() != m_pScroll);

		//  update texts
		if (m_TopOffset != int(v))
		{
			m_TopOffset = int(v);
			CommitChanges();
		}
	}

	bool ListBox::GetFocused( void )
	{
		return (m_pHolder->GetFocused() || m_pScroll->GetFocused());
	}

	void ListBox::SetFocused( void )
	{
		if (m_pBack->HasProperty(SX_GUI_PROPERTY_VISIBLE)	&&
			m_pBack->HasProperty(SX_GUI_PROPERTY_ENABLE)	&&
			!m_pBack->HasProperty(_SX_GUI_NOT_ENABLE_)		&&
			!m_pBack->HasProperty(_SX_GUI_NOT_VISIBLE_))
			m_pHolder->SetFocused(true);
	}

	PPanel ListBox::GetBack( void )
	{
		return m_pBack;
	}

	PTrackBar ListBox::GetScroll( void )
	{
		return m_pScroll;
	}

	float3& ListBox::Position( void )
	{
		return m_pBack->Position();
	}

	int ListBox::Count( void )
	{
		return m_Items.Count();
	}

	void ListBox::Clear( void )
	{
		for ( int i=0; i<m_Items.Count(); i++ )
			sx_delete( m_Items[i]);
		m_Items.Clear();
		CommitChanges();
	}

	void ListBox::Add( const WCHAR* itemText, const WCHAR* itemImage, void* userData, const int level )
	{
		Item* tmp = sx_new( Item );
		tmp->Text		= itemText;
		tmp->UserData	= userData;
		tmp->Option		= 0;
		tmp->Level		= level;
		tmp->SetTexture(itemImage);

		m_Items.PushBack( tmp );
		CommitChanges();
	}

	void ListBox::Remove( int index )
	{
		if (index<0 || index>=m_Items.Count()) return;
		sx_delete( m_Items[index] );
		m_Items.RemoveByIndex(index);
		CommitChanges();
	}

	const WCHAR* ListBox::GetText( int index )
	{
		if (index<0 || index>=m_Items.Count()) return NULL;
		return m_Items[index]->Text;
	}

	void* ListBox::GetUserData( int index )
	{
		if (index<0 || index>=m_Items.Count()) return NULL;
		return m_Items[index]->UserData;
	}

	void ListBox::CommitChanges( void )
	{
		if (m_ItemIndex>=m_Items.Count()) 
		{
			m_ItemIndex = m_Items.Count()-1;
			m_OnSelect(m_pBack);
		}

		int n =  int(m_pHolder->GetSize()[1] / m_ItemHeight);
		m_pScroll->SetMax(m_Items.Count() - (float)n);
		if ( m_pScroll->GetMax() < 1 )
			m_pScroll->RemProperty(SX_GUI_PROPERTY_VISIBLE);
		else
			m_pScroll->AddProperty(SX_GUI_PROPERTY_VISIBLE);

		int icount	= m_Items.Count() - m_TopOffset;
		for (int i=0; i<m_UI.Count(); i++)
		{
			ItemUI* puitem = m_UI[i];
			if ( i < icount )
			{
				int index = i + m_TopOffset;
				Item* pitem = m_Items[index];

				puitem->Back()->AddProperty(SX_GUI_PROPERTY_VISIBLE);

				puitem->Data().index = index;
				puitem->SetText( pitem->Text );
				puitem->SetImageTexture( pitem->GetTexture() );
				puitem->Back()->PositionOffset().x = (float)pitem->Level * LISTBOX_LEVEL_OFFSET;
				puitem->m_pImage->SetUserData( pitem );

				if ( m_ItemIndex == index )
					puitem->Color() = itemindxColor;
				else
					puitem->Color() = pitem->Option ? selectedColor : deselectColor;
			}
			else 
				puitem->Back()->RemProperty(SX_GUI_PROPERTY_VISIBLE);
		}
	}

	void ListBox::SelectItemByMouseMove( bool v )
	{
		if (v)
		{
			SEGAN_SET_ADD(m_Option, LISTBOX_MOUSEMOVESELECT);
			m_pHolder->SetOnMouseMove( this, (GUICallbackEvent)&ListBox::OnMouseMove );
		}
		else
		{
			SEGAN_SET_REM(m_Option, LISTBOX_MOUSEMOVESELECT);
			m_pHolder->SetOnMouseMove( NULL, NULL );
		}
	}

	void ListBox::OnMouseMove( PControl Sender )
	{
		if ( !(m_Option & LISTBOX_MOUSEMOVESELECT) ) return;

		float2 uv;
		
		Sender->SetOnMouseMove( NULL, NULL );
		Sender->MouseOver(io::Input::GetCursor_ABS(0)->x, io::Input::GetCursor_ABS(0)->y, uv);
		Sender->SetOnMouseMove( this, (GUICallbackEvent)&ListBox::OnMouseMove );

		uv.y = 0.5f - uv.y;
		SetItemIndex(int(m_pScroll->GetBlendingValue() + uv.y * Sender->GetSize()[1] / m_ItemHeight));
	}

	void ListBox::OnItemClick( sx::gui::PControl Sender )
	{
		float2 uv;
		Sender->MouseOver(io::Input::GetCursor_ABS(0)->x, io::Input::GetCursor_ABS(0)->y, uv);
		uv.y = 0.5f - uv.y;
		SetItemIndex(int(m_pScroll->GetBlendingValue() + uv.y * Sender->GetSize()[1] / m_ItemHeight));
		m_OnClick(m_pBack);
	}

	void ListBox::OnItemImageClick( PControl Sender )
	{
		if ( !Sender || !Sender->GetUserData() ) return;

		Item* pitem = (Item*)Sender->GetUserData();
		if ( pitem->Option & ITEM_EXPANDED )
			SEGAN_SET_REM( pitem->Option, ITEM_EXPANDED );
		else
			SEGAN_SET_ADD( pitem->Option, ITEM_EXPANDED );

		CommitChanges();
	}

	void ListBox::OnKeyDown( sx::gui::PControl Sender )
	{
		int n =  int(m_pHolder->GetSize()[1] / m_ItemHeight) - 1;
		const char* keys = io::Input::GetKeys(0);
		int oldIndex = m_ItemIndex;

		if (keys[SX_INPUT_KEY_MOUSE_WHEEL]<0)
			m_pScroll->SetValue(m_pScroll->GetValue() + 1 + n * (keys[SX_INPUT_KEY_LCONTROL]>0 || keys[SX_INPUT_KEY_RCONTROL]>0));
		else if (keys[SX_INPUT_KEY_MOUSE_WHEEL]>0)
			m_pScroll->SetValue(m_pScroll->GetValue() - 1 - n * (keys[SX_INPUT_KEY_LCONTROL]>0 || keys[SX_INPUT_KEY_RCONTROL]>0));
		else if (keys[SX_INPUT_KEY_DOWN]>0)
			SetItemIndex(oldIndex + 1);
		else if (keys[SX_INPUT_KEY_UP]>0)
		{
			oldIndex -= 1;
			if (oldIndex<0) oldIndex = 0;
			SetItemIndex(oldIndex);
		}
		else if (keys[SX_INPUT_KEY_PAGEDN]>0)
			SetItemIndex(oldIndex + 1 + n);
		else if (keys[SX_INPUT_KEY_PAGEUP]>0)
		{
			oldIndex -= 1 + n;
			if (oldIndex<0) oldIndex = 0;
			SetItemIndex(oldIndex);
		}
		else if (keys[SX_INPUT_KEY_SPACE]>0)
			SetItemIndex(m_ItemIndex);

		m_OnKeyboard(m_pBack);

	}

	int ListBox::GetItemIndex( void )
	{
		return m_ItemIndex;
	}

	void ListBox::SetItemIndex( int index )
	{
		SEGAN_CLAMP(index, -1, m_Items.Count()-1);
		const char* keys = io::Input::GetKeys(0);

		if (index>-1 && m_ItemIndex>-1 && (keys[SX_INPUT_KEY_LSHIFT]>0 || keys[SX_INPUT_KEY_RSHIFT]>0))
		{
			SEGAN_SET_ADD(m_Option, LISTBOX_MULTISELECT);
			if (multiselect_fromindex<0) multiselect_fromindex = m_ItemIndex;

			if ( index != m_ItemIndex )
			{
				int i = multiselect_fromindex;
				while ( i != index )
				{
					m_Items[i]->Option |= ITEM_SELECTED;
					i += i < index ? 1 : -1;
				}
				m_Items[i]->Option  |= ITEM_SELECTED;
			}

			//CommitChanges();
			//m_OnSelect(m_pBack);
		}
		else if (index>-1 && m_ItemIndex>-1 && (keys[SX_INPUT_KEY_LCONTROL]>0 || keys[SX_INPUT_KEY_RCONTROL]>0))
		{
			multiselect_fromindex = -1;
			SEGAN_SET_ADD(m_Option, LISTBOX_MULTISELECT);
			if ( keys[SX_INPUT_KEY_SPACE]>0 && index>-1 )
			{
				if ( m_Items[index]->Option & ITEM_SELECTED )
					SEGAN_SET_REM( m_Items[index]->Option, ITEM_SELECTED );
				else
					SEGAN_SET_ADD( m_Items[index]->Option, ITEM_SELECTED );
			}
			//CommitChanges();
			//m_OnSelect(m_pBack);
		}
		else
		{
			multiselect_fromindex = -1;
			if (m_Option & LISTBOX_MULTISELECT)
			{
				for (int i=0; i<m_Items.Count(); i++) 
					SEGAN_SET_REM( m_Items[i]->Option, ITEM_SELECTED );
			}
			SEGAN_SET_REM(m_Option, LISTBOX_MULTISELECT);

			//CommitChanges();
			//m_OnSelect(m_pBack);
		}

		if (m_ItemIndex != index)
		{
			m_ItemIndex = index;
			int n =  int(m_pHolder->GetSize()[1] / m_ItemHeight) - 1;
			if (m_ItemIndex<int(m_pScroll->GetValue()))		m_pScroll->SetValue(float(m_ItemIndex));
			if (m_ItemIndex>int(m_pScroll->GetValue()) + n)	m_pScroll->SetValue(float(m_ItemIndex - n));

			CommitChanges();
			m_OnSelect(m_pBack);
		}
	}

	void ListBox::SetOnMouseClick( sx::gui::PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnClick.m_pForm = pForm;
		m_OnClick.m_pFunc = pFunc;
	}

	void ListBox::SetOnKeyDown( sx::gui::PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnKeyboard.m_pForm = pForm;
		m_OnKeyboard.m_pFunc = pFunc;
	}

	void ListBox::SetOnSelect( sx::gui::PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnSelect.m_pForm = pForm;
		m_OnSelect.m_pFunc = pFunc;
	}

	ListBox::Item& ListBox::operator[]( int index )
	{
		static Item tmp;
		if (index<0 || index>=m_Items.Count())
			return tmp;
		return *m_Items[index];
	}

	ListBox::Item& ListBox::GetItem( int index )
	{
		static Item tmp;
		if (index<0 || index>=m_Items.Count())
			return tmp;
		return *m_Items[index];
	}

	sx::gui::ItemUI* ListBox::GetUI( int index )
	{
		if (index<0 || index>=m_UI.Count())
			return NULL;
		return m_UI[index];
	}



} } //  namespace sx { namespace gui {