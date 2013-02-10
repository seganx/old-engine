#include "Form_ImportModel.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"

#define IMPORTOBJ_WIDTH		300.0f		//  size of the form
#define IMPORTOBJ_HEIGHT	400.0f		//  size of the form


Form_ImportModel::Form_ImportModel( void ): BaseForm()
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
	m_pTitle->SetText(L"Import OBJ File Option");

	m_ch_Textures	= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Overwrite imported texture(s)" );
	m_ch_renormal	= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Force to recompute normals" );
	m_ch_centerPivot= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Move pivot to the center of mesh(es)" );
	m_ch_shCast		= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Cast shadow" );
	m_ch_shReceive	= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Receive shadow" );
	m_ch_invisible	= EditorUI::CreateCheckBox( m_pBack, 20, 200, L": Invisible mesh(es)" );

	m_td_prefix		= EditorUI::CreateEditBox( m_pBack, 20, 200, NULL );
	sx::gui::PLabel lbl = EditorUI::CreateLabel( m_td_prefix, 200, 8, L"Resource Prefix : " );
	lbl->Position().y = 20;

	m_tr_lodFirst	= EditorUI::CreateTrackbar( m_pBack, 200, 0, 100 );
	SEGAN_GUI_SET_ONSCROLL( m_tr_lodFirst, Form_ImportModel::OnParamChange );
	lbl = EditorUI::CreateLabel( m_tr_lodFirst, 80, 8, L"LOD 1 : " );
	lbl->Position().x = -135;
	m_tr_lodFirst->SetValue(80);

	m_tr_lodSecond	= EditorUI::CreateTrackbar( m_pBack, 200, 0, 100 );
	SEGAN_GUI_SET_ONSCROLL( m_tr_lodSecond, Form_ImportModel::OnParamChange );
	lbl = EditorUI::CreateLabel( m_tr_lodSecond, 80, 8, L"LOD 2 : " );
	lbl->Position().x = -135;
	m_tr_lodSecond->SetValue(60);

	m_btn_Shader	= EditorUI::CreateButton( m_pBack, 270, L"..." );
	SEGAN_GUI_SET_ONCLICK( m_btn_Shader, Form_ImportModel::OnParamChange );
	lbl = EditorUI::CreateLabel( m_btn_Shader, 270, 8, L"Shader :" );
	lbl->Position().y = 22;
	m_btn_Shader->SetHint(L"Use this shader for all imported meshes.\nIf no shader specified then importer automatically will choose a shader.");

	m_btn_ok	= EditorUI::CreateButton( m_pBack, 128, L"OK" );
	SEGAN_GUI_SET_ONCLICK( m_btn_ok, Form_ImportModel::OnParamChange );
	m_btn_Cancel	= EditorUI::CreateButton( m_pBack, 128, L"Cancel" );
	SEGAN_GUI_SET_ONCLICK( m_btn_Cancel, Form_ImportModel::OnParamChange );

	SetSize( IMPORTOBJ_WIDTH, IMPORTOBJ_HEIGHT );
}

Form_ImportModel::~Form_ImportModel( void )
{
	//  for other things the parent will do everything :)
}

void Form_ImportModel::SetSize( float width, float height )
{
	float top = height*0.5f - 16.0f;
	float left = -width*0.5f + 16.0f;
	float right = width*0.5f - 16.0f;

	m_pTitle->SetSize( float2(width - 70.0f, 50.0f) );
	m_pTitle->Position().y = top - 5;					top -= 30;
	
	m_ch_Textures->Position().Set( left, top, 0 );		top -= 20;
	m_ch_renormal->Position().Set( left, top, 0 );		top -= 20;
	m_ch_centerPivot->Position().Set( left, top, 0 );	top -= 20;
	m_ch_shCast->Position().Set( left, top, 0 );		top -= 20;
	m_ch_shReceive->Position().Set( left, top, 0 );		top -= 20;
	m_ch_invisible->Position().Set( left, top, 0 );		top -= 50;
	m_td_prefix->Position().Set( left+100, top, 0);		top -= 40;
	m_tr_lodFirst->Position().Set( 35, top, 0 );		top -= 30;
	m_tr_lodSecond->Position().Set( 35, top, 0 );		top -= 50;
	m_btn_Shader->Position().y = top;					top -= 50;


	m_btn_ok->Position().Set( left + 55, top, 0 );
	m_btn_Cancel->Position().Set( right - 55, top, 0 );

	BaseForm::SetSize(width, height);
}

void Form_ImportModel::ProcessInput( bool& inputHandled )
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

	if ( SEGAN_KEYUP(0, SX_INPUT_KEY_R) )
		SetSize( IMPORTOBJ_WIDTH, IMPORTOBJ_HEIGHT );

	BaseForm::ProcessInput(inputHandled);
}

void Form_ImportModel::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	BaseForm::Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(1).Position.Set(0,0,0);
	m_pBack->State_GetByIndex(0).Position.Set(0,0,0);
}

void Form_ImportModel::Import( const WCHAR* FileName, sx::gui::PForm pForm, GUICallbackEvent OnOkClick, DWORD Tag )
{
	m_fileName = FileName;
	m_OnOkClick.m_pForm = pForm;
	m_OnOkClick.m_pFunc = OnOkClick;
	m_btn_ok->SetUserTag( Tag );

	m_ImportOption.shaderName.Clear();
	sx::gui::PLabel( m_btn_Shader->GetChild(0) )->SetText(L"...");

	Show();
}


//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
void Form_ImportModel::OnParamChange( sx::gui::PControl Sender )
{
	if (!Sender ) return;

	if ( Sender == m_tr_lodFirst )
	{
		str256		 str = L"LOD 1 : ";
		str << int(m_tr_lodFirst->GetValue()) << L" %";
		sx::gui::PLabel( m_tr_lodFirst->GetChild(0) )->SetText(str);
	}
	else if ( Sender == m_tr_lodSecond )
	{
		str256		 str = L"LOD 2 : ";
		str << int(m_tr_lodSecond->GetValue()) << L" %";
		sx::gui::PLabel( m_tr_lodSecond->GetChild(0) )->SetText(str);
	}
	else if ( Sender == m_btn_Shader )
	{
		Editor::frm_Explorer->SetTitle(L"Open Shader");
		Editor::frm_Explorer->Open( this, (GUICallbackEvent)&Form_ImportModel::OnSetShader );
	}
	else if ( Sender == m_btn_Cancel )
	{
		m_fileName.Clear();
		Close();
	}
	else if ( Sender == m_btn_ok )
	{
		m_ImportOption.flag = 0;
		if ( m_ch_Textures->Checked() )		m_ImportOption.flag |= IMPORT_MODEL_TEXTURES;
		if ( m_ch_renormal->Checked() )		m_ImportOption.flag |= IMPORT_MODEL_RENORMAL;
		if ( m_ch_centerPivot->Checked() )	m_ImportOption.flag |= IMPORT_MODEL_CENTERPV;

		m_ImportOption.meshOption = 0;
		if ( m_ch_shCast->Checked() )		m_ImportOption.meshOption |= SX_MESH_CASTSHADOW;
		if ( m_ch_shReceive->Checked() )	m_ImportOption.meshOption |= SX_MESH_RECIEVESHADOW;
		if ( m_ch_invisible->Checked() )	m_ImportOption.meshOption |= SX_MESH_INVISIBLE;

		m_ImportOption.prefix = m_td_prefix->GetText();

		m_ImportOption.precisionLOD1 = m_tr_lodFirst->GetValue()  * 0.01f;
		m_ImportOption.precisionLOD2 = m_tr_lodSecond->GetValue() * 0.01f;

		m_OnOkClick( Sender );
		Close();
	}
}

ImportModelOptions * Form_ImportModel::GetOptions( DWORD flag /*= 0*/ )
{
	m_ImportOption.flag |= flag;
	return &m_ImportOption; 
}

const WCHAR* Form_ImportModel::GetFileName( void )
{
	return m_fileName;
}

void Form_ImportModel::OnSetShader( sx::gui::PControl Sender )
{
	str1024 str = Editor::frm_Explorer->GetPath();
	str.ExtractFileExtension();
	str.MakeLower();
	if ( str == L"fx" || str == L"cs" )
	{
		//  import shader to library
		str = Editor::frm_Explorer->GetPath();
		str.ExtractFileName();
		str.ExcludeFileExtension();
		str << L".sfx";

		String strfx;
		sx::cmn::String_Load( strfx, Editor::frm_Explorer->GetPath() );

		MemoryStream stm;
		sx::cmn::String_Save( strfx, &stm );
		sx::sys::FileManager::File_Save( str, SEGAN_PACKAGENAME_EFFECT, stm );
	}
	else if (str == L"sfx")
	{
		//  just apply shader to selected material
		str = Editor::frm_Explorer->GetPath();
		str.ExtractFileName();
	}

	m_ImportOption.shaderName = str;
	sx::gui::PLabel( m_btn_Shader->GetChild(0) )->SetText( str );

	Editor::frm_Explorer->Close();
}