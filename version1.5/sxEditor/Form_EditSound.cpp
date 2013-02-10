#include "Form_EditSound.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"

#define EDITSOUND_WIDTH			300.0f		//  size of the edit mesh form
#define EDITSOUND_HEIGHT		490.0f		//  size of the edit mesh form

bool applyChanges = false;

Form_EditSound::Form_EditSound( void ): BaseForm(), m_sound(NULL), m_soundData(NULL)
{
	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Edit Sound");

	//  label of description
	m_lbl_desc = EditorUI::CreateLabel( m_pBack, 270.0f, 8 );
	m_lbl_desc->SetSize( float2(270, 50) );
	m_lbl_desc->AddProperty( SX_GUI_PROPERTY_MULTILINE );
	m_lbl_desc->SetText( L"description1\ndescription2" );

	//  check boxes
	m_ch_loop = EditorUI::CreateCheckBox( m_pBack, 20, 80, L": Loop" );
	SEGAN_GUI_SET_ONCLICK( m_ch_loop, Form_EditSound::OnParamChange );
	m_ch_is3D = EditorUI::CreateCheckBox( m_pBack, 20, 80, L": Use 3D" );
	SEGAN_GUI_SET_ONCLICK( m_ch_is3D, Form_EditSound::OnParamChange );
	m_ch_onload = EditorUI::CreateCheckBox( m_pBack, 20, 80, L": Play on load" );
	SEGAN_GUI_SET_ONCLICK( m_ch_onload, Form_EditSound::OnParamChange );

	//  frequency of sound
	m_tx_freq = EditorUI::CreateLabeldEditBox( m_pBack, -135, 75.0f, 195.0f, L"Frequency :" );
	SEGAN_GUI_SET_ONTEXT( m_tx_freq, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_tx_freq, Form_EditSound::OnMouseWheel );

	//  sound volume
	m_tx_volm = EditorUI::CreateLabeldEditBox( m_pBack, -135, 75.0f, 195.0f, L"Volume :" );
	SEGAN_GUI_SET_ONTEXT( m_tx_volm, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_tx_volm, Form_EditSound::OnMouseWheel );

	//  pan the sound
	m_tr_pan = EditorUI::CreateTrackbar( m_pBack, 200, -1, 1 );
	SEGAN_GUI_SET_ONSCROLL( m_tr_pan, Form_EditSound::OnParamChange );
	sx::gui::PLabel lbl = EditorUI::CreateLabel( m_tr_pan, 70, 8, L"Balance :" );
	lbl->Position().x = -135;

	//  minimum distance of the sound
	m_tx_mindis = EditorUI::CreateLabeldEditBox( m_pBack, -135, 75.0f, 195.0f, L"Min Distance : " );
	SEGAN_GUI_SET_ONTEXT( m_tx_mindis, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_tx_mindis, Form_EditSound::OnMouseWheel );

	//  maximum distance of the sound
	m_tx_maxdis = EditorUI::CreateLabeldEditBox( m_pBack, -135, 75.0f, 195.0f, L"Max Distance : " );
	SEGAN_GUI_SET_ONTEXT( m_tx_maxdis, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_tx_maxdis, Form_EditSound::OnMouseWheel );

	//  repeat time for sound loop
	m_tx_repeat = EditorUI::CreateLabeldEditBox( m_pBack, -135, 75.0f, 195.0f, L"Repeat Time : " );
	SEGAN_GUI_SET_ONTEXT( m_tx_repeat, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_tx_repeat, Form_EditSound::OnMouseWheel );

	//  chance to play sound in loop
	m_tx_chance = EditorUI::CreateLabeldEditBox( m_pBack, -135, 75.0f, 195.0f, L"Chance : " );
	SEGAN_GUI_SET_ONTEXT( m_tx_chance, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_tx_chance, Form_EditSound::OnMouseWheel );

	//	list of sounds
	m_sndList = sx_new( sx::gui::ListBox );
	m_sndList->SetParent( m_pBack );
	m_sndList->SetSize( 230, 100.0f, 18.0f, false );
	m_sndList->SetFont( EditorUI::GetDefaultFont(8) );
	m_sndList->SetItemIndex( 0 );
	m_sndList->SetOnSelect( this, (GUICallbackEvent)&Form_EditSound::OnListChange );
	lbl = EditorUI::CreateLabel( m_sndList->GetBack(), 120.0f, 8, L"Sounds List :");
	lbl->Position().Set( -55.0f, 57.0f, 0 );
	m_splitter = sx_new( sx::gui::Panel );
	m_splitter->SetParent(m_sndList->GetBack());
	m_splitter->SetSize( float2( 300, 2 ) );
	m_splitter->Position().Set( 20.0f, -65.0f, 0.0f );
	m_splitter->GetElement(0)->Color() = D3DColor(0.1f, 0.1f, 0.1f, 1.0f);

	m_sndAdd	= EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"AddList") );
	m_sndRem	= EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"RemList") );
	m_moveUp	= EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"MoveUp") );
	m_moveDown	= EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"MoveDown") );
	SEGAN_GUI_SET_ONCLICK( m_sndAdd, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_sndRem, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_moveUp, Form_EditSound::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_moveDown, Form_EditSound::OnParamChange );

	m_splitter = sx_new( sx::gui::Panel );
	m_splitter->SetParent(m_pBack);
	m_splitter->GetElement(0)->Color() = D3DColor(0.1f, 0.1f, 0.1f, 1.0f);

	m_tr_position = EditorUI::CreateTrackbar( m_pBack, 270, 0, 0 );
	SEGAN_GUI_SET_ONSCROLL( m_tr_position, Form_EditSound::OnParamChange );

	m_btn_play	= EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"ButtonPlay") );
	SEGAN_GUI_SET_ONCLICK( m_btn_play, Form_EditSound::OnParamChange );
	
	m_btn_pause	= EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"ButtonPause") );
	SEGAN_GUI_SET_ONCLICK( m_btn_pause, Form_EditSound::OnParamChange );
	m_lbl_time	= EditorUI::CreateLabel( m_pBack, 100, 8, L"00:00 / 00:00" );
	m_lbl_time->SetAlign( GTA_RIGHT );

	//  stop resizing
	//SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	
	SetSize( EDITSOUND_WIDTH, EDITSOUND_HEIGHT );
}

Form_EditSound::~Form_EditSound( void )
{
	sx_delete_and_null( m_sndList );
	//  for other things the parent will do everything :)
}

void Form_EditSound::SetSize( float width, float height )
{
	float top = height*0.5f - 16.0f;
	float bot = -height*0.5f + 16.0f;
	float left = -width*0.5f + 16.0f;
	float right = width*0.5f - 16.0f;

	m_pTitle->SetSize( float2(width - 70.0f, 50.0f) );
	m_pTitle->Position().y = top - 5;					top -= 30;
	
	m_lbl_desc->Position().y = top-12;
	m_ch_loop->Position().Set( right - 70, top, 0);		top -= 20;
	m_ch_is3D->Position().Set( right - 70, top, 0);		top -= 20;
	m_ch_onload->Position().Set( right - 70, top, 0);	top -= 70;

	//m_btn_Load->Position().y = top;						
	top -= 20;
	
	left += 115;
	m_sndList->Position().Set( left, top, 0 );			top += 40;
	left -= 115;

	left += 250;
	m_sndAdd->Position().Set( left, top, 0 );			top -= 27;
	m_sndRem->Position().Set( left, top, 0 );			top -= 27;
	m_moveUp->Position().Set( left, top, 0 );			top -= 27;
	m_moveDown->Position().Set( left, top, 0 );			top -= 50;
	left -= 250;

	m_tx_freq->Position().y = top;						top -= 20;
	m_tx_volm->Position().y = top;						top -= 20;
	m_tr_pan->Position().Set(35, top, 0);				top -= 25;
	m_tx_mindis->Position().y = top;					top -= 25;
	m_tx_maxdis->Position().y = top;					top -= 25;
	m_tx_repeat->Position().y = top;					top -= 25;
	m_tx_chance->Position().y = top;					//top -= 50;
	
	//  bottom to top
	m_lbl_time->Position().Set( right-50, bot + 8, 0);
	m_btn_play->Position().Set( left+16, bot, 0);
	m_btn_pause->Position().Set( left+60, bot, 0);		bot += 25;
	m_tr_position->Position().y = bot;					bot += 20;

	m_splitter->SetSize( float2(width , 2.0f) );
	m_splitter->Position().y = bot;

	BaseForm::SetSize(width, height);
}

void Form_EditSound::ProcessInput( bool& inputHandled )
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

void Form_EditSound::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	static str256		 str;

	if ( m_sound )
	{
// 		//  show some info
// 		str = L"Frequency : ";
// 		str << int( m_sound->GetDesc()->freq ) << L" ";
// 		sx::gui::PLabel(m_tx_freq->GetChild(0))->SetText( str );

		if ( m_soundData )
		{
			int LM = int( m_soundData->GetDesc()->lenght / 60 );
			int LS = int( m_soundData->GetDesc()->lenght ) % 60;
			int TM = int( m_sound->GetTimePosition() / 60 );
			int TS = int( m_sound->GetTimePosition() ) % 60;
			str.Format(L" %.2d:%.2d / %.2d:%.2d ", TM, TS, LM, LS);
			m_lbl_time->SetText(str);

			applyChanges = false;
			m_tr_position->SetValue( m_sound->GetTimePosition() );
			applyChanges = true;
		}
		else m_lbl_time->SetText( L" 00:00 / 00:00 " );
	}

	m_sndList->Update(elpsTime);
	BaseForm::Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;
	
}

void Form_EditSound::Draw( DWORD flag )
{
	BaseForm::Draw(flag);
}

void Form_EditSound::SetSoundToEdit( sx::core::PNodeMember sound )
{
	if ( !IsVisible() )
	{
		m_sound = NULL;
		m_soundData = NULL;
		return;
	}

	//  verify the sound
	if ( !sound || sound->GetType() != NMT_SOUND )
	{
		m_sound = NULL;
		m_soundData = NULL;
		Close();
		return;
	}
	if ( m_sound == sound ) return;

	m_sound = (sx::core::PSound)sound;

	//  set title
	if ( m_sound->GetName() )
	{
		str512 str = L"Edit Sound\n";
		str << m_sound->GetName();
		m_pTitle->SetText( str );
	}
	else m_pTitle->SetText(L"Edit Sound\n[no name]");

	ReloadList();

	//  collect information of the sound
	ShowSoundInfo( true );
}

//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
void Form_EditSound::OnParamChange( sx::gui::PControl Sender )
{
	if (!Sender || !m_sound ) return;

	if ( Sender == m_sndAdd )
	{
		Editor::frm_importSound->Import( this, (GUICallbackEvent)&Form_EditSound::LoadSound );
		return;
	}

	if ( !applyChanges ) return;

	if ( Sender == m_sndRem )
	{
		int index = m_sndList->GetItemIndex();
		m_sound->m_resources.Delete( index );
		m_sndList->Remove( index );
	}
	else if ( Sender == m_moveUp )
	{
		int cur = m_sndList->GetItemIndex();
		if ( cur > 0 )
		{
			m_sound->m_resources.Swap( cur, cur-1 );
			ReloadList();
			m_sndList->SetItemIndex( cur-1 );
		}
	}
	else if ( Sender == m_moveDown )
	{
		int cur = m_sndList->GetItemIndex();
		if ( cur < m_sndList->Count()-1 )
		{
			m_sound->m_resources.Swap( cur, cur+1 );
			ReloadList();
			m_sndList->SetItemIndex( cur+1 );
		}
	}
	else if ( Sender == m_ch_loop )
	{
		SoundPlayerDesc desc = *( m_sound->GetDesc() );
		desc.loop = m_ch_loop->Checked();
		m_sound->SetDesc( desc );
	}

	else if ( Sender == m_ch_is3D )
	{
		SoundPlayerDesc desc = *( m_sound->GetDesc() );
		desc.is3D = m_ch_is3D->Checked();
		m_sound->SetDesc( desc );
	}

	else if ( Sender == m_ch_onload )
	{
		if ( m_ch_onload->Checked() )
			m_sound->AddOption( SX_SOUND_PLAYONLOAD );
		else
			m_sound->RemOption( SX_SOUND_PLAYONLOAD );
	}

	else if ( Sender == m_tx_freq )
	{
		SoundPlayerDesc desc = *( m_sound->GetDesc() );
		desc.freq = str128::StrToInt( m_tx_freq->GetText() );
		m_sound->SetDesc( desc );
	}

	else if ( Sender == m_tx_volm )
	{
		SoundPlayerDesc desc = *( m_sound->GetDesc() );
		desc.volume = str128::StrToInt( m_tx_volm->GetText() ) / 100.0f;
		m_sound->SetDesc( desc );
	}

	else if ( Sender == m_tr_pan )
	{
		SoundPlayerDesc desc = *( m_sound->GetDesc() );
		desc.pan = m_tr_pan->GetValue();
		m_sound->SetDesc( desc );
	}

	else if ( Sender == m_tx_mindis )
	{
		SoundPlayerDesc desc = *( m_sound->GetDesc() );
		float md = str128::StrToFloat( m_tx_mindis->GetText() );
		desc.minDistance = md;
		m_sound->SetDesc( desc );
	}

	else if ( Sender == m_tx_maxdis )
	{
		SoundPlayerDesc desc = *( m_sound->GetDesc() );
		float md = str128::StrToFloat( m_tx_maxdis->GetText() );
		desc.maxDistance = md;
		m_sound->SetDesc( desc );
	}

	else if ( Sender == m_tx_repeat )
	{
		float md = str128::StrToFloat( m_tx_repeat->GetText() );
		m_sound->m_repeatTime = md;
		m_sound->CommitDesc();
	}

	else if ( Sender == m_tx_chance )
	{
		float md = str128::StrToFloat( m_tx_chance->GetText() );
		m_sound->m_chance = md;
		m_sound->CommitDesc();
	}

	else if ( Sender == m_tr_position )
	{
		m_sound->SetTimePosition( m_tr_position->GetValue() );
	}

	else if ( Sender == m_btn_play )
	{
		m_sound->Play(false);
	}

	else if ( Sender == m_btn_pause )
	{
		m_sound->Pause();
	}
}

void Form_EditSound::OnMouseWheel( sx::gui::PControl Sender )
{
	if ( !m_sound || !Sender || !applyChanges ) return;

	switch (Sender->GetType())
	{
	case GUI_TEXTEDIT:
		{
			sx::gui::PTextEdit pEdit = (sx::gui::PTextEdit)Sender;
			if ( pEdit->GetFocused() )
			{
				float ceoff = 1.0f;
				if ( Sender == m_tx_chance )
					ceoff = 0.02f;
				else if ( Sender == m_tx_repeat )
					ceoff = 0.2f;

				float r = str128::StrToFloat( pEdit->GetText() );
				r += sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL] * ceoff * 0.5f;
				pEdit->SetText(FloatToStr(	r ) );
			}
		}
	}
}

void Form_EditSound::ShowSoundInfo( bool applyToEditor )
{
	if ( !m_sound ) return;

	int index = m_sndList->GetItemIndex();
	if ( index < 0 ) index = 0;
	if ( index >= m_sound->m_resources.Count() ) return;

	applyChanges = false;

	if ( sx::snd::SoundData::Manager::Get( m_soundData, m_sound->m_resources[index] ) )
	{
		str1024 str = L"Type : ";
		switch ( m_soundData->GetDesc()->type )
		{
		case ST_SAMPLE:	str << L"Sound Sample";	break;
		case ST_STREAM:	str << L"Music Stream";	break;
		default:		str << L"UNKNOWN !!!!";
		}

		str << L"\nMode : ";
		switch ( m_soundData->GetDesc()->channels )
		{
		case 0:		str << L"UNKNOWN !!!!";		break;
		case 1:		str << L"Mono";				break;
		case 2:		str << L"Stereo";			break;
		default:	str << L"Multi channel";	break;
		}

		str << L"\nFreq : " << int(m_soundData->GetDesc()->freq);
		m_lbl_desc->SetText( str );

		m_tr_position->SetMax( m_soundData->GetDesc()->lenght );
		m_tr_position->SetValue(0);

		if ( m_soundData->GetDesc()->is3D )
		{
			m_ch_is3D->AddProperty( SX_GUI_PROPERTY_ENABLE );
			m_ch_is3D->GetElement(0)->Color() = D3DColor( 1.0f, 0.6f, 0.6f, 1.0f );
		}
		else
		{
			m_ch_is3D->RemProperty( SX_GUI_PROPERTY_ENABLE );
			m_ch_is3D->GetElement(0)->Color() = D3DColor(0.5f, 0.5f, 0.5f, 1);
		}
	}
	else
	{
// 		m_lbl_desc->SetText( L"No Sound Loaded !\nUse below button to load a sound." );
// 		sx::gui::PLabel(m_btn_Load->GetChild(0))->SetText( L"..." );
		m_tr_position->SetMax(0);
		m_tr_position->SetValue(0);
		m_soundData = NULL;
	}

	if ( applyToEditor )
	{
		m_ch_loop->Checked() = m_sound->GetDesc()->loop;
		m_ch_is3D->Checked() = m_sound->GetDesc()->is3D;
		m_ch_onload->Checked() = m_sound->HasOption( SX_SOUND_PLAYONLOAD );
		m_tx_freq->SetText( IntToStr(m_sound->GetDesc()->freq ? m_sound->GetDesc()->freq : ( m_soundData ? m_soundData->GetDesc()->freq : 0 ) ) );
		m_tx_volm->SetText( IntToStr( int(m_sound->GetDesc()->volume * 100.0f) ) );
		m_tr_pan->SetValue( m_sound->GetDesc()->pan );
		m_tx_mindis->SetText( FloatToStr( m_sound->GetDesc()->minDistance ) );
		m_tx_maxdis->SetText( FloatToStr( m_sound->GetDesc()->maxDistance ) );
		m_tx_repeat->SetText( FloatToStr( m_sound->m_repeatTime ) );
		m_tx_chance->SetText( FloatToStr( m_sound->m_chance ) );
		applyChanges = true;
	}
	else
	{
		applyChanges = true;

		OnParamChange( m_ch_loop );
		OnParamChange( m_ch_is3D );
		OnParamChange( m_ch_onload );
		OnParamChange( m_tx_freq );
		OnParamChange( m_tx_volm );
		OnParamChange( m_tr_pan );
		OnParamChange( m_tx_mindis );
		OnParamChange( m_tx_maxdis );
		OnParamChange( m_tx_repeat );
		OnParamChange( m_tx_chance );
	}

}

void Form_EditSound::LoadSound( sx::gui::PControl Sender )
{
	if ( !m_sound ) return;

	float timeElapsed = sx::sys::GetSysTime();

	str1024 str = Editor::frm_importSound->GetImportedFileName();
	str.ExtractFileExtension();
	str.MakeLower();

 	if (str == L"snd")
 	{
 		str = Editor::frm_importSound->GetImportedFileName();
		str.ExtractFileName();
 		m_sound->m_resources.PushBack( str );
		m_sound->m_index = m_sound->m_resources.Count() - 1;
		m_sndList->SetItemIndex( m_sound->m_index );
 		ShowSoundInfo( true );
 		//m_sound->Play(true);
		ReloadList();
 
		str1024 tmp = Editor::frm_Explorer->GetPath();
 		str1024 str = L"'";
 		str << tmp	<< L"' loaded to project .";
 
 		Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
 	}
}

void Form_EditSound::OnListChange( sx::gui::PControl Sander )
{
	if ( !m_sound ) return;

	int index = m_sndList->GetItemIndex();
	if( index > -1 )
	{
		m_sound->m_index = index;
		applyChanges = false;
		ShowSoundInfo( true );
		applyChanges = true;
	}
}

void Form_EditSound::ReloadList( void )
{
	m_sndList->Clear();
	if ( !m_sound ) return;

	str1024 str;
	for ( int i=0; i<m_sound->m_resources.Count(); i++ )
	{
		str.Format( L"%d : %s", i, m_sound->m_resources[i].Text() );
		m_sndList->Add( str, NULL, NULL );
	}
	m_sndList->SetItemIndex( m_sound->m_index );

}