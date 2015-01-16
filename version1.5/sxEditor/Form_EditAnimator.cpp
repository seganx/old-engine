#include "Form_EditAnimator.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorScene.h"

#define EDITPATH_WIDTH			250.0f		//  size of the edit mesh form
#define EDITPATH_HEIGHT			350.0f		//  size of the edit mesh form


Form_EditAnimator::Form_EditAnimator( void ): BaseForm(), m_animator(NULL), m_applyChanges(false)
{
	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	m_pBack->State_Add();
	m_pBack->State_GetByIndex(2) = m_pBack->State_GetByIndex(1);

	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Edit Animator");

	m_animlist = sx_new( sx::gui::ListBox );
	m_animlist->SetParent( m_pBack );
	m_animlist->SetSize( 200, 200.0f, 18.0f, false );
	m_animlist->SetFont( EditorUI::GetDefaultFont(8) );
	m_animlist->SetItemIndex( 0 );
	m_animlist->SetOnSelect( this, (GUICallbackEvent)&Form_EditAnimator::OnListChange );
	sx::gui::PLabel lbl = EditorUI::CreateLabel( m_animlist->GetBack(), 120.0f, 8, L"Animation List :");
	lbl->Position().Set( -40.0f, 105.0f, 0 );

	m_animSpeed = EditorUI::CreateLabeldEditBox( m_pBack, 10, 100, 100, L"Speed : ", L"1.00" );
	SEGAN_GUI_SET_ONTEXT( m_animSpeed, Form_EditAnimator::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_animSpeed, Form_EditAnimator::OnMouseWheel );

	m_blendTime = EditorUI::CreateLabeldEditBox( m_pBack, 10, 100, 100, L"Blend Time : ", L"1.00" );
	SEGAN_GUI_SET_ONTEXT( m_blendTime, Form_EditAnimator::OnParamChange );
	SEGAN_GUI_SET_ONWHEEL( m_blendTime, Form_EditAnimator::OnMouseWheel );

	m_addAnim = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"AddList") );
	m_remAnim = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"RemList") );
	m_clearList = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"ClearList") );
	m_moveUp = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"MoveUp") );
	m_moveDown = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"MoveDown") );

	SEGAN_GUI_SET_ONCLICK( m_addAnim, Form_EditAnimator::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_remAnim, Form_EditAnimator::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_clearList, Form_EditAnimator::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_moveUp, Form_EditAnimator::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_moveDown, Form_EditAnimator::OnParamChange );

	m_loop = EditorUI::CreateCheckBox( m_pBack, 10, 100, L" : Loop " );
	SEGAN_GUI_SET_ONCLICK( m_loop, Form_EditAnimator::OnParamChange );

	SetSize( EDITPATH_WIDTH, EDITPATH_HEIGHT );

}

Form_EditAnimator::~Form_EditAnimator( void )
{
	//  for other things the parent will do everything :)
	sx_delete( m_animlist );
}

void Form_EditAnimator::SetSize( float width, float height )
{
	float top = height*0.5f - 16.0f;
	float bot = -height*0.5f + 16.0f;
	float left = -width*0.5f + 16.0f;
	float right = width*0.5f - 16.0f;

	m_pTitle->SetSize( float2(width - 70.0f, 50.0f) );
	m_pTitle->Position().y = top;
	top -= 140.0f;

	left += 90;
	m_animlist->Position().Set( left, top, 0 );		top += 80;
	left -= 90;

	left += 210;
	m_addAnim->Position().Set( left, top, 0 );			top -= 40;
	m_remAnim->Position().Set( left, top, 0 );			top -= 40;
	m_clearList->Position().Set( left, top, 0 );		top -= 40;
	m_moveUp->Position().Set( left, top, 0 );			top -= 40;
	m_moveDown->Position().Set( left, top, 0 );			top -= 50;
	left -= 210;

	left += 140;
	m_animSpeed->Position().Set( left, top, 0);		top -= 20;
	m_blendTime->Position().Set( left, top, 0);		top -= 20;
	left -= 140;

	m_loop->Position().Set( left, top, 0);
	
	BaseForm::SetSize(width, height);
}

void Form_EditAnimator::ProcessInput( bool& inputHandled )
{
	if ( !IsVisible() || inputHandled ) return;

	const char* keys = sx::io::Input::GetKeys(0);

	if (keys[SX_INPUT_KEY_ESCAPE] == SX_INPUT_STATE_UP)
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

void Form_EditAnimator::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	m_animlist->Update(elpsTime);
	BaseForm::Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;
	if ( m_pBack->State_GetIndex() == 2 )
	{
		SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	}
	else
	{
		SEGAN_SET_ADD(m_Option, FORM_OPTION_MOVABLE);
	}
	
}

void Form_EditAnimator::Draw( DWORD flag )
{

	BaseForm::Draw(flag);
}

void Form_EditAnimator::OnResize( int EditorWidth, int EditorHeight )
{
 	float offsetW = (float)(EditorWidth  % 2);
 	float offsetH = (float)(EditorHeight  % 2);
 	float width = (float)EditorWidth + offsetW;
 	float height = (float)EditorHeight + offsetH;

	m_pBack->State_GetByIndex(2).Position.x = - width/2 - EDITPATH_WIDTH/2 + 120.0f;
	m_pBack->State_GetByIndex(2).Position.y = height*0.5f + EDITPATH_HEIGHT/2 - 40.0f;
}

void Form_EditAnimator::SetAnimatorToEdit( sx::core::PNodeMember anim )
{
	if ( !IsVisible() ) return;

	//  verify the path
	if ( !anim || anim->GetType() != NMT_ANIMATOR )
	{
		m_animator = NULL;
		Close();
		return;
	}
	if ( m_animator == anim ) return;
	m_animator = (sx::core::PAnimator)anim;

	//  set title
	if ( m_animator->GetName() )
	{
		str1024 str = L"Edit Animator\n";
		str << m_animator->GetName();
		m_pTitle->SetText( str );
	}
	else m_pTitle->SetText(L"Edit Animator - [no name]");

	m_applyChanges = false;

	ReloadList();

	int index = m_animlist->GetItemIndex();
	if( index > -1 )
	{
		sx::d3d::PAnimation pAnim = m_animator->GetAnimation( index );
		m_animSpeed->SetText( FloatToStr( pAnim->m_speed ) );
		m_blendTime->SetText( FloatToStr( pAnim->m_blendTime ) );
		m_loop->Checked() = SEGAN_SET_HAS( pAnim->m_Option, SX_ANIMATION_LOOP );
	}

	m_applyChanges = true;

}

//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
void Form_EditAnimator::OnParamChange( sx::gui::PControl Sender )
{
	if ( !m_animator || !Sender || !m_applyChanges ) return;

	float animspeed = String		::StrToFloat( m_animSpeed->GetText() );
	float blendtime = String		::StrToFloat( m_blendTime->GetText() );

	int index = m_animlist->GetItemIndex();
	if( index > -1 )
	{
		sx::d3d::PAnimation pAnim = m_animator->GetAnimation( index );

		pAnim->m_speed = animspeed;
		pAnim->m_blendTime = blendtime;
		if ( m_loop->Checked() )
			SEGAN_SET_ADD( pAnim->m_Option, SX_ANIMATION_LOOP );
		else
			SEGAN_SET_REM( pAnim->m_Option, SX_ANIMATION_LOOP );
	}

	if ( Sender == m_addAnim )
	{
		Editor::frm_Explorer->SetTitle( L"Open Animation File" );
		Editor::frm_Explorer->Open( this, (GUICallbackEvent)&Form_EditAnimator::OnOpenAnimation );
	}
	else if ( Sender == m_remAnim )
	{
		int index = m_animlist->GetItemIndex();
		m_animator->RemoveAnimation( index );
		m_animlist->Remove( index );
	}
	else if ( Sender == m_clearList )
	{
		m_animator->ClearAnimations();
		m_animlist->Clear();
	}
	else if ( Sender == m_moveUp )
	{
		int cur = m_animlist->GetItemIndex();
		if ( cur > 0 )
		{
			m_animator->m_Controller.m_Anims.Swap( cur, cur-1 );
			ReloadList();
			m_animlist->SetItemIndex( cur-1 );
		}
	}
	else if ( Sender == m_moveDown )
	{
		int cur = m_animlist->GetItemIndex();
		if ( cur < m_animlist->Count()-1 )
		{
			m_animator->m_Controller.m_Anims.Swap( cur, cur+1 );
			ReloadList();
			m_animlist->SetItemIndex( cur+1 );
		}
	}
}

void Form_EditAnimator::OnListChange( sx::gui::PControl Sander )
{
	if ( !m_animator ) return;

	int index = m_animlist->GetItemIndex();
	if( index > -1 )
	{
		m_animator->SetAnimationByIndex( index );

		m_applyChanges = false;
		sx::d3d::PAnimation pAnim = m_animator->GetAnimation( index );
		m_animSpeed->SetText( FloatToStr( pAnim->m_speed ) );
		m_blendTime->SetText( FloatToStr( pAnim->m_blendTime ) );
		m_loop->Checked() = SEGAN_SET_HAS( pAnim->m_Option, SX_ANIMATION_LOOP );
		m_applyChanges = true;
	}
}


void Form_EditAnimator::Operate( bool& inputHandled )
{

}

void Form_EditAnimator::Show( void )
{
	BaseForm::Show();
}

void Form_EditAnimator::OnOpenAnimation( sx::gui::PControl Sander )
{
	str1024 FileName = Editor::frm_Explorer->GetPath();

	str1024 str = FileName;
	str.ExtractFileExtension();
	str.MakeLower();
	if (str == L"anim")
	{
		str = FileName ;
		str.ExtractFileName();
		m_animator->AddAnimation( str );
		ReloadList();
	}
}

void Form_EditAnimator::ReloadList( void )
{
	if ( !m_animator ) return;

	str1024 str;
	m_animlist->Clear();
	for ( int i=0; i<m_animator->AnimationCount(); i++ )
	{
		sx::d3d::PAnimation panim = m_animator->GetAnimation(i);

		str.Format( L"%.2d : %s", i, panim->GetSource() );
		m_animlist->Add( str, NULL, NULL );
	}
	m_animlist->SetItemIndex( m_animator->GetAnimationIndex() );

}

void Form_EditAnimator::OnMouseWheel( sx::gui::PControl Sender )
{
	if ( !Sender || !m_applyChanges ) return;

	switch ( Sender->GetType() )
	{
	case GUI_TEXTEDIT:
		{
			sx::gui::TextEdit* pEdit = (sx::gui::TextEdit*)Sender;
			if ( pEdit->GetFocused() )
			{
				m_applyChanges = false;
				float r = str128::StrToFloat( pEdit->GetText() );
				r += sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL] * 0.05f;
				pEdit->SetText(FloatToStr(	r ) );
				m_applyChanges = true;
				OnParamChange( Sender );
				break;
			}
		}
	}
}