#include "Form_Toolbar.h"
#include "sxItemUI.h"
#include "EditorUI.h"
#include "EditorSettings.h"


Form_Toolbar::Form_Toolbar( void ): BaseForm()
{
	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	m_MinSize.x = 100.0f;	
	m_MinSize.y = 50.0f;
	SetSize( (float)EditorSettings::GetInitWindowSize().x, FORM_TOOLBAR_SIZE_H );
}

Form_Toolbar::~Form_Toolbar( void )
{
	//  for other things the parent will do everything :)
}

void Form_Toolbar::SetSize( float width, float height )
{
	BaseForm::SetSize(width, height);
}

void Form_Toolbar::ProcessInput( bool& inputHandled )
{
	if (inputHandled) return;

	const char* keys = sx::io::Input::GetKeys(0);

	if (keys[SX_INPUT_KEY_ESCAPE] == SX_INPUT_STATE_UP)
	{
		Close();
		inputHandled = true;
		return;
	}

	BaseForm::ProcessInput(inputHandled);
}

void Form_Toolbar::Update( float elpsTime )
{
	BaseForm::Update(elpsTime);
}

void Form_Toolbar::OnResize( int EditorWidth, int EditorHeight )
{
	float offsetW = (float)(EditorWidth  % 2);
	float offsetH = (float)(EditorHeight  % 2);
	float width = (float)EditorWidth + offsetW;
	float height = (float)EditorHeight + offsetH;
	float top = height*0.5f - FORM_TOOLBAR_SIZE_H + 40.0f;

	SetSize(width-30.0f, FORM_TOOLBAR_SIZE_H);

	//  set new states
	m_pBack->State_GetByIndex(0).Center		= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(0).Position	= float3(0.0f, top, 0.0f);
	m_pBack->State_GetByIndex(0).Scale		= float3(1.0f, 1.0f, 1.0f);
	m_pBack->State_GetByIndex(0).Color.w	= 0.0f;

	m_pBack->State_GetByIndex(1).Center		= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(1).Position	= float3(0.0f, top, 0.0f);
	m_pBack->State_GetByIndex(1).Scale		= float3(1.0f, 1.0f, 1.0f);
}
