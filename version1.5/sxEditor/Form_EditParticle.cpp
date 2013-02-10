#include "Form_EditParticle.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "ImportTexture.h"

#define FORM_EDITPARTICLE_WIDTH			300.0f		//  size of the edit mesh form
#define FORM_EDITPARTICLE_HEIGHT		300.0f		//  size of the edit mesh form

#define ADD_BUTTON(btn, txname){\
	btn = sx_new( sx::gui::Button );\
	btn->SetParent(m_pBack);\
	btn->SetSize( float2(28.0f, 28.0f) );\
	btn->RemProperty(SX_GUI_PROPERTY_BLENDCHILDS);\
	btn->GetElement(0)->SetTextureSrc( EditorUI::GetButtonTexture(txname, 0) );\
	btn->GetElement(0)->Color() = 0xFFcccccc;\
	btn->GetElement(1)->SetTextureSrc( EditorUI::GetButtonTexture(txname, 0) );\
	btn->GetElement(1)->Color() = 0xFFffffff;\
	btn->GetElement(2)->SetTextureSrc( EditorUI::GetButtonTexture(txname, 0) );\
	btn->GetElement(2)->Color() = 0xFF999999;\
}\

//////////////////////////////////////////////////////////////////////////
//  static variables

//////////////////////////////////////////////////////////////////////////

Form_EditParticle::Form_EditParticle( void ): BaseForm(), m_applyChanges(false)
{
	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Edit Mesh");

	const float left = - FORM_EDITPARTICLE_WIDTH * 0.5f + 10.0f;
	
	m_ch_Spray		= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Spray");
	m_ch_Loop		= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Loop");
	m_ch_lock_Y		= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Lock Y");
	m_ch_lock_XZ	= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Lock XZ");
	m_ch_lock_XY	= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Lock XY");
	m_ch_lock_YZ	= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Lock YZ");
	m_ch_reflect	= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Reflect");
	m_ch_rndTime	= EditorUI::CreateCheckBox(m_pBack, left + 190.0f, 100.0f, L"Random delay");

	m_flow			= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Flow :",				L"50");
	m_quadCount		= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Quad count :",			L"50");
	m_lifeTime		= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Life time :",			L"10");
	m_angleBase		= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Angle base :",			L"0.0");
	m_angleSpeed	= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Angle speed :",			L"0.0");
	m_scaleBaseX	= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Scale base X:",			L"1.0");
	m_scaleBaseY	= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Scale base Y:",			L"1.0");
	m_scaleSpeed	= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Scale speed :",			L"0.0");
	m_moveDepend	= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Move independently :",	L"0.0");
	m_delayTime		= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Delay time :",			L"0.0");
	m_rndParam1		= EditorUI::CreateLabeldEditBox(m_pBack, left, 50.0f, 115.0f, L"Random Parameter 1:",	L"0");

	m_lbl_speed		= EditorUI::CreateLabel(m_pBack, 80.0f, 8, L"Aperture :");	m_lbl_speed->Position().x	= left + 40.0f;
	m_lbl_range		= EditorUI::CreateLabel(m_pBack, 80.0f, 8, L"Init Range :");	m_lbl_range->Position().x	= left + 40.0f;
	m_lbl_gravity	= EditorUI::CreateLabel(m_pBack, 80.0f, 8, L"Force :");			m_lbl_gravity->Position().x = left + 40.0f;

	SEGAN_GUI_SET_ONCLICK(m_ch_Spray,	Form_EditParticle::OnCheckBoxClick);
	SEGAN_GUI_SET_ONCLICK(m_ch_Loop,	Form_EditParticle::OnCheckBoxClick);
	SEGAN_GUI_SET_ONCLICK(m_ch_lock_Y,	Form_EditParticle::OnCheckBoxClick);
	SEGAN_GUI_SET_ONCLICK(m_ch_lock_XZ,	Form_EditParticle::OnCheckBoxClick);
	SEGAN_GUI_SET_ONCLICK(m_ch_lock_XY,	Form_EditParticle::OnCheckBoxClick);
	SEGAN_GUI_SET_ONCLICK(m_ch_lock_YZ,	Form_EditParticle::OnCheckBoxClick);
	SEGAN_GUI_SET_ONCLICK(m_ch_reflect,	Form_EditParticle::OnCheckBoxClick);
	SEGAN_GUI_SET_ONCLICK(m_ch_rndTime,	Form_EditParticle::OnCheckBoxClick);

	SEGAN_GUI_SET_ONTEXT(m_flow,		Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_quadCount,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_lifeTime,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_angleBase,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_angleSpeed,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_scaleBaseX,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_scaleBaseY,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_scaleSpeed,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_moveDepend,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_delayTime,	Form_EditParticle::OnParamChange);
	SEGAN_GUI_SET_ONTEXT(m_rndParam1,	Form_EditParticle::OnParamChange);

	SEGAN_GUI_SET_ONWHEEL(m_flow,		Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_quadCount,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_lifeTime,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_angleBase,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_angleSpeed,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_scaleBaseX,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_scaleBaseY,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_scaleSpeed,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_moveDepend,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_delayTime,	Form_EditParticle::OnMouseWheel);
	SEGAN_GUI_SET_ONWHEEL(m_rndParam1,	Form_EditParticle::OnMouseWheel);

	float ts = FORM_EDITPARTICLE_WIDTH / 4.0f;
	for (int i=0; i<3; i++)
	{
		m_moveSpeed[i]	= EditorUI::CreateEditBox(m_pBack, 100.0f + left + ts * i, 40.0f, L"0.0");
		m_initRange[i]	= EditorUI::CreateEditBox(m_pBack, 100.0f + left + ts * i, 40.0f, L"0.0");
		m_gravity[i]	= EditorUI::CreateEditBox(m_pBack, 100.0f + left + ts * i, 40.0f, L"0.0");

		SEGAN_GUI_SET_ONTEXT(m_moveSpeed[i],	Form_EditParticle::OnParamChange);
		SEGAN_GUI_SET_ONTEXT(m_initRange[i],	Form_EditParticle::OnParamChange);
		SEGAN_GUI_SET_ONTEXT(m_gravity[i],		Form_EditParticle::OnParamChange);

		SEGAN_GUI_SET_ONWHEEL(m_moveSpeed[i],	Form_EditParticle::OnMouseWheel);
		SEGAN_GUI_SET_ONWHEEL(m_initRange[i],	Form_EditParticle::OnMouseWheel);
		SEGAN_GUI_SET_ONWHEEL(m_gravity[i],		Form_EditParticle::OnMouseWheel);
	}

	m_EditMaterial.SetParent(m_pBack);
	m_EditMaterial.SetOnSizeChanged(this, (GUICallbackEvent)&Form_EditParticle::OnMaterialResized);

	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	m_MinSize.x = 100.0f;	
	m_MinSize.y = 50.0f;
	
	SetSize( FORM_EDITPARTICLE_WIDTH, FORM_EDITPARTICLE_HEIGHT );
}

Form_EditParticle::~Form_EditParticle( void )
{
	//  for other things the parent will do everything :)
}

void Form_EditParticle::SetSize( float width, float height )
{
	float top = height*0.5f - 15.0f;
	float left = -width*0.5f;

	m_pTitle->SetSize( float2(width - 70.0f, 40.0f) );
	m_pTitle->Position().y = top;
	top -= 25.0f;
	
	m_ch_Spray->Position().y = top;
	m_ch_Loop->Position().y = top - 18.0f;
	m_ch_lock_Y->Position().y = top - 36.0f;
	m_ch_lock_XZ->Position().y = top - 54.0f;
	m_ch_lock_XY->Position().y = top - 72.0f;
	m_ch_lock_YZ->Position().y = top - 90.0f;
	m_ch_reflect->Position().y = top - 108.0f;
	m_ch_rndTime->Position().y = top - 126.0f;

	m_flow->Position().y= top;			top -= 18.0f;
	m_quadCount->Position().y= top;		top -= 18.0f;
	m_lifeTime->Position().y= top;		top -= 18.0f;
	m_angleBase->Position().y= top;		top -= 18.0f;
	m_angleSpeed->Position().y= top;	top -= 18.0f;
	m_scaleBaseX->Position().y= top;	top -= 18.0f;
	m_scaleBaseY->Position().y= top;	top -= 18.0f;
	m_scaleSpeed->Position().y= top;	top -= 18.0f;
	m_moveDepend->Position().y= top;	top -= 18.0f;
	m_delayTime->Position().y= top;		top -= 18.0f;
	m_rndParam1->Position().y= top;		top -= 18.0f;

	m_lbl_speed->Position().y = top;
	for (int i=0; i<3; i++)
		m_moveSpeed[i]->Position().y = top;
	top -= 18.0f;

	m_lbl_gravity->Position().y = top;
	for (int i=0; i<3; i++)
		m_gravity[i]->Position().y = top;
	top -= 18.0f;

	m_lbl_range->Position().y = top;
	for (int i=0; i<3; i++)
		m_initRange[i]->Position().y = top;
	top -= 23.0f;

	m_EditMaterial.Position().y = top;
	m_EditMaterial.SetSize(width, height);

	BaseForm::SetSize(width, height);
}

void Form_EditParticle::ProcessInput( bool& inputHandled )
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

void Form_EditParticle::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	BaseForm::Update(elpsTime);

	m_EditMaterial.Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;
	
}

void Form_EditParticle::OnResize( int EditorWidth, int EditorHeight )
{
// 	float offsetW = (float)(EditorWidth  % 2);
// 	float offsetH = (float)(EditorHeight  % 2);
// 	float width = (float)EditorWidth + offsetW;
// 	float height = (float)EditorHeight + offsetH;
}

void Form_EditParticle::SetParticleToEdit( sx::core::PNodeMember particle )
{
	if ( !IsVisible() ) return;

	//  verify the mesh
	if ( !particle || particle->GetType() != NMT_PARTICLE )
	{
		m_particle = NULL;
		Close();
		return;
	}
	if ( m_particle == particle ) return;
	m_particle = (sx::core::PParticle)particle;

	//  set title
	if ( m_particle->GetName() )
	{
		str1024 str = L"Edit Particle\n";
		str << m_particle->GetName();
		m_pTitle->SetText( str );
	}
	else m_pTitle->SetText(L"Edit Particle - [no name]");

	m_applyChanges = false;

	m_ch_Spray->Checked() = m_particle->HasOption(SX_PARTICLE_SPRAY);
	m_ch_Loop->Checked() = m_particle->HasOption(SX_PARTICLE_LOOP);
	m_ch_lock_Y->Checked() = m_particle->HasOption(SX_PARTICLE_LOCK_Y);
	m_ch_lock_XZ->Checked() = m_particle->HasOption(SX_PARTICLE_LOCK_XZ);
	m_ch_lock_XY->Checked() = m_particle->HasOption(SX_PARTICLE_LOCK_XY);
	m_ch_lock_YZ->Checked() = m_particle->HasOption(SX_PARTICLE_LOCK_YZ);
	m_ch_reflect->Checked() = !m_particle->HasOption(SX_PARTICLE_INVIS_IN_REFLET);
	m_ch_rndTime->Checked() = m_particle->HasOption(SX_PARTICLE_RNDTIME);

	ParticleDesc desc = *m_particle->GetDesc();
	m_quadCount->SetText( IntToStr( desc.quadCount ) );
	m_flow->SetText(FloatToStr( desc.flow ) );
	m_lifeTime->SetText(FloatToStr( desc.lifeTime ) );
	m_angleBase->SetText(FloatToStr( desc.angleBase ) );
	m_angleSpeed->SetText(FloatToStr( desc.angleSpeed ) );
	m_scaleBaseX->SetText(FloatToStr( desc.scaleBaseX ) );
	m_scaleBaseY->SetText(FloatToStr( desc.scaleBaseY ) );
	m_scaleSpeed->SetText(FloatToStr( desc.scaleSpeed ) );
	m_moveDepend->SetText(FloatToStr( desc.moveIndependently ) );
	m_delayTime->SetText(FloatToStr( desc.delayTime ) );
	m_rndParam1->SetText( IntToStr( SEGAN_3TH_BYTEOF(desc.param) ) );

	for (int i=0; i<3; i++)
	{
		m_moveSpeed[i]->SetText(FloatToStr( desc.initSpeed[i] ) );
		m_initRange[i]->SetText(FloatToStr( desc.initRange[i] ) );
		m_gravity[i]->SetText(FloatToStr( desc.gravity[i] ) );
	}

	m_applyChanges = true;

	//  update material
	m_EditMaterial.SetMaterialToEdit( &m_particle->Material() );
	OnMaterialResized( NULL );
}

//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
void Form_EditParticle::OnMaterialResized( sx::gui::PControl Sender )
{
	//  update form size depend of height of material editor
	float2 cursize = GetSize();
	cursize.y = 300.0f + m_EditMaterial.GetHeight();
	SetSize(cursize.x, cursize.y );
}

void Form_EditParticle::OnParamChange( sx::gui::PControl Sender )
{
	if ( !m_particle || !m_applyChanges ) return;

	ParticleDesc desc;

	desc.quadCount			= str64::StrToInt( m_quadCount->GetText() );
	desc.flow				= str128::StrToFloat( m_flow->GetText() );
	desc.lifeTime			= str128::StrToFloat( m_lifeTime->GetText() );
	desc.angleBase			= str128::StrToFloat( m_angleBase->GetText() );
	desc.angleSpeed			= str128::StrToFloat( m_angleSpeed->GetText() );
	desc.scaleBaseX			= str128::StrToFloat( m_scaleBaseX->GetText() );
	desc.scaleBaseY			= str128::StrToFloat( m_scaleBaseY->GetText() );
	desc.scaleSpeed			= str128::StrToFloat( m_scaleSpeed->GetText() );
	desc.moveIndependently	= str128::StrToFloat( m_moveDepend->GetText() );
	desc.delayTime			= str128::StrToFloat( m_delayTime->GetText() );
	SEGAN_3TH_BYTEOF(desc.param) = (BYTE)str64::StrToInt( m_rndParam1->GetText() );

	for (int i=0; i<3; i++)
	{
		desc.initSpeed[i] = str128::StrToFloat( m_moveSpeed[i]->GetText() );
		desc.initRange[i] = str128::StrToFloat( m_initRange[i]->GetText() );
		desc.gravity[i]	  = str128::StrToFloat( m_gravity[i]->GetText() );
	}

	m_particle->SetDesc( &desc );
}

void Form_EditParticle::OnMouseWheel( sx::gui::PControl Sender )
{
	if (!m_particle || !m_applyChanges) return;

	switch (Sender->GetType())
	{
	case GUI_TEXTEDIT:
		{
			sx::gui::PTextEdit pEdit = (sx::gui::PTextEdit)Sender;
			if ( pEdit->GetFocused() )
			{
				if ( pEdit == m_quadCount || pEdit == m_rndParam1 )
				{
					int r = str64::StrToInt( pEdit->GetText() );
					r += sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL];
					pEdit->SetText( IntToStr( r ) );
				}
				else
				{
					float r = str128::StrToFloat( pEdit->GetText() );
					r += sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL] * 0.5f;
					pEdit->SetText(FloatToStr(	r ) );
				}
			}
		}
	}
}

void Form_EditParticle::OnCheckBoxClick( sx::gui::PControl Sender )
{
	if ( !m_particle || !m_applyChanges ) return;

	if ( m_ch_Spray->Checked() )
		m_particle->AddOption( SX_PARTICLE_SPRAY );
	else
		m_particle->RemOption( SX_PARTICLE_SPRAY );

	if ( m_ch_Loop->Checked() )
		m_particle->AddOption( SX_PARTICLE_LOOP );
	else
		m_particle->RemOption( SX_PARTICLE_LOOP );

	if ( m_ch_lock_Y->Checked() )
		m_particle->AddOption( SX_PARTICLE_LOCK_Y );
	else
		m_particle->RemOption( SX_PARTICLE_LOCK_Y );

	if ( m_ch_lock_XZ->Checked() )
		m_particle->AddOption( SX_PARTICLE_LOCK_XZ );
	else
		m_particle->RemOption( SX_PARTICLE_LOCK_XZ );

	if ( m_ch_lock_XY->Checked() )
		m_particle->AddOption( SX_PARTICLE_LOCK_XY );
	else
		m_particle->RemOption( SX_PARTICLE_LOCK_XY );

	if ( m_ch_lock_YZ->Checked() )
		m_particle->AddOption( SX_PARTICLE_LOCK_YZ );
	else
		m_particle->RemOption( SX_PARTICLE_LOCK_YZ );

	if ( m_ch_reflect->Checked() )
		m_particle->RemOption( SX_PARTICLE_INVIS_IN_REFLET );
	else
		m_particle->AddOption( SX_PARTICLE_INVIS_IN_REFLET );

	if ( m_ch_rndTime->Checked() )
		m_particle->AddOption( SX_PARTICLE_RNDTIME );
	else
		m_particle->RemOption( SX_PARTICLE_RNDTIME );

}
