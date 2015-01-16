#include "Form_ImportSound.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"

#define IMPORTSOUND_WIDTH		300.0f		//  size of the form
#define IMPORTSOUND_HEIGHT		150.0f		//  size of the form


Form_ImportSound::Form_ImportSound( void ): BaseForm()
{
	// invisible close button
	m_pClose->RemProperty( SX_GUI_PROPERTY_VISIBLE );

	//  stop resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	//  stop moving
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);

	//  simply disable any things
	sx::gui::PPanel panel = sx_new( sx::gui::Panel );
	panel->SetParent(m_pBack);
	panel->SetSize( float2(99999, 99999) );
	panel->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	panel->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.2f);

	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Import Sound Option");

	m_ch_is3D	= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Import sound with 3D functionality" );
	m_ch_Sample = EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Import as sound sample" );
	SEGAN_GUI_SET_ONCLICK( m_ch_Sample, Form_ImportSound::OnParamChange );
	m_ch_Stream	= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Import as Music stream" );
	SEGAN_GUI_SET_ONCLICK( m_ch_Stream, Form_ImportSound::OnParamChange );

	m_btn_ok	= EditorUI::CreateButton( m_pBack, 128, L"OK" );
	SEGAN_GUI_SET_ONCLICK( m_btn_ok, Form_ImportSound::OnParamChange );
	m_btn_Cancel	= EditorUI::CreateButton( m_pBack, 128, L"Cancel" );
	SEGAN_GUI_SET_ONCLICK( m_btn_Cancel, Form_ImportSound::OnParamChange );

	m_ch_Sample->Checked() = true;
	
	SEGAN_SET_ADD(m_Option, FORM_OPTION_ONTOP);

	SetSize( IMPORTSOUND_WIDTH, IMPORTSOUND_HEIGHT );
}

Form_ImportSound::~Form_ImportSound( void )
{
	//  for other things the parent will do everything :)
}

void Form_ImportSound::SetSize( float width, float height )
{
	float top = height*0.5f - 16.0f;
	float left = -width*0.5f + 16.0f;
	float right = width*0.5f - 16.0f;

	m_pTitle->SetSize( float2(width - 70.0f, 50.0f) );
	m_pTitle->Position().y = top - 5;					top -= 30;
	
	m_ch_Sample->Position().Set( left, top, 0 );		top -= 20;
	m_ch_Stream->Position().Set( left, top, 0 );		top -= 20;
	m_ch_is3D->Position().Set(	 left, top, 0 );		top -= 40;

	m_btn_ok->Position().Set( left + 55, top, 0 );
	m_btn_Cancel->Position().Set( right - 55, top, 0 );

	BaseForm::SetSize(width, height);
}

void Form_ImportSound::ProcessInput( bool& inputHandled )
{
	if ( !IsVisible() || inputHandled ) return;

	if ( SEGAN_KEYUP(0, SX_INPUT_KEY_ESCAPE) )
	{
		sx::gui::ArrayPControl clist;
		m_pBack->GetChildren(clist);
		for (int i=0; i<clist.Count(); i++)
		{
			if ( clist[i]->GetFocused() )
			{
				Close();
				inputHandled = true;
				return;
			}
		}
	}

	BaseForm::ProcessInput(inputHandled);
}

void Form_ImportSound::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	BaseForm::Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(1).Position.Set(0,0,0);
	m_pBack->State_GetByIndex(0).Position.Set(0,0,0);
	
}

void Form_ImportSound::Import( sx::gui::PForm pForm, GUICallbackEvent OnOkClick )
{
	m_OnOkClick.m_pForm = pForm;
	m_OnOkClick.m_pFunc = OnOkClick;

	Editor::frm_Explorer->SetTitle( L"Load/Import Sound" );
	Editor::frm_Explorer->Open( this, (GUICallbackEvent)&Form_ImportSound::LoadSound );
}

const WCHAR* Form_ImportSound::GetImportedFileName( void )
{
	return m_fileName;
}

//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
void Form_ImportSound::OnParamChange( sx::gui::PControl Sender )
{
	if (!Sender ) return;

	if ( Sender == m_ch_Sample )
	{
		m_ch_Stream->Checked() = !m_ch_Sample->Checked();
	}
	else if ( Sender == m_ch_Stream )
	{
		m_ch_Sample->Checked() = !m_ch_Stream->Checked();
	}
	else if ( Sender == m_btn_Cancel )
	{
		m_fileName.Clear();
		Close();
	}
	else if ( Sender == m_btn_ok )
	{
		SoundDataDesc dataDesc(ST_SAMPLE);
		
		if ( m_ch_Stream->Checked() )	dataDesc.type = ST_STREAM;
		if ( m_ch_is3D->Checked() )		dataDesc.is3D = true;

		sx::snd::PSoundData sdata(NULL);
		sx::snd::SoundData::Manager::Create( sdata, NULL );
		sdata->LoadFromFile( m_fileName , dataDesc, true);
		sx::snd::SoundData::Manager::Release( sdata );

		m_fileName.ExtractFileName();
		m_fileName.ExcludeFileExtension();
		m_fileName << L".snd";

		m_OnOkClick( Sender );
		Close();
	}
}

void Form_ImportSound::LoadSound( sx::gui::PControl Sender )
{
	str1024 str = Editor::frm_Explorer->GetPath();
	str.ExtractFileExtension();
	str.MakeLower();

	if (str == L"snd")
	{
		m_fileName = Editor::frm_Explorer->GetPath();
		m_OnOkClick( Sender );
		Close();
	}
	else
	{
		m_fileName = Editor::frm_Explorer->GetPath();
		Editor::frm_Explorer->Close();
		Show();
	}
}

