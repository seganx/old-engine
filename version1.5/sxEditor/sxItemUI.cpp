#include "sxItemUI.h"
#include "EditorUI.h"


#define ITEMUI_ASPECT_HORIZ		1.2f	//  threshold of horizontal aspect ratio

namespace sx { namespace gui {

	ItemUI::ItemUI( void ): Form()
	{
		ZeroMemory(&m_Data, sizeof(ItemData));

		m_pBack = static_cast<PPanel>(gui::Create(GUI_PANEL));
		
		m_pImage = static_cast<PPanel>(gui::Create(GUI_PANEL));
		m_pImage->AddProperty(SX_GUI_PROPERTY_PIXELALIGN);
		m_pImage->SetParent(m_pBack);

		m_pLabel = static_cast<PLabel>(gui::Create(GUI_LABEL));
		m_pLabel->AddProperty(SX_GUI_PROPERTY_MULTILINE);
		m_pLabel->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
		m_pLabel->SetParent(m_pBack);
	}

	ItemUI::~ItemUI( void )
	{
		m_pBack->SetParent(NULL);
		SEGAN_GUI_DELETE(m_pBack);
	}

	void ItemUI::SetSize( float width, float height, bool displayImage /*= true*/ )
	{
		m_pBack->SetSize( float2(width, height) );

		if (displayImage)
		{
			//  new we can decide to choose an style of the item
			//  if the item shape is going to a box we can position
			//  the label on the bottom of the image and if shape
			//  of the item is going to a horizontal rectangle we
			//  position the label in right side of the image.

			if (width/height > ITEMUI_ASPECT_HORIZ)
			{
				m_pImage->SetSize( float2(height, height) );
				m_pImage->Position().x = height*0.5f - width*0.5f;
				m_pImage->Position().y = 0.0f;

				m_pLabel->SetSize( float2(width - height - 1.0f, height) );
				m_pLabel->SetAlign(GTA_LEFT);
				m_pLabel->Position().x = height*0.5f + 1.0f;
				m_pLabel->Position().y = 0.0f;
			}
			else
			{
				m_pImage->SetSize( float2(width - 2.0f, width - 2.0f) );
				m_pImage->Position().x = 0.0f;
				m_pImage->Position().y = (height - width)*0.5f;

				m_pLabel->SetSize( float2(width - 2.0f, 20.0f + (height - width)*0.5f) );
				m_pLabel->SetAlign(GTA_CENTER);
				m_pLabel->Position().x = 0.0f;
				m_pLabel->Position().y = - width + height*0.5f - 10.0f;
			}
		}
		else
		{
			m_pImage->SetSize( float2(0.0f, 0.0f) );
			m_pImage->RemProperty(SX_GUI_PROPERTY_VISIBLE);
			m_pLabel->SetSize( float2(width - 2.0f, height - 2.0f) );
			m_pLabel->Position().Set( 0, 0, 0 );
		}
	}

	void ItemUI::SetParent( PControl parent )
	{
		m_pBack->SetParent(parent);
	}

	void ItemUI::SetText( const WCHAR* text )
	{
		m_pLabel->SetText(text);
	}

	const WCHAR* ItemUI::GetText( void )
	{
		return m_pLabel->GetText();
	}

	void ItemUI::SetImageTexture( const WCHAR* src )
	{
		m_pImage->GetElement(0)->SetTextureSrc(src);
	}

	const WCHAR* ItemUI::GetImageTexture( void )
	{
		return m_pImage->GetElement(0)->GetTextureSrc();
	}

	float3& ItemUI::Position( void )
	{
		return m_pBack->Position();
	}

	D3DColor& ItemUI::Color( void )
	{
		return m_pBack->GetElement(0)->Color();
	}

	PPanel ItemUI::Back( void )
	{
		return m_pBack;
	}

	PPanel ItemUI::Image( void )
	{
		return m_pImage;
	}

	PLabel ItemUI::Label( void )
	{
		return m_pLabel;
	}

	ItemUI::ItemData& ItemUI::Data( void )
	{
		return m_Data;
	}

}} // namespace sx { namespace gui {
