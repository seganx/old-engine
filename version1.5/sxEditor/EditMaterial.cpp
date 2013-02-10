#include "EditMaterial.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "ImportTexture.h"

using namespace sx;

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


// use this simple structure to hold information of the parameters in editor
typedef struct ParameterInfo
{
	int					iParam;			// index of the shader parameter
	int					iElement;		// index of the element of parameter. float4 has 4 element
	PShaderAnnotation	pAnnot;			// pointer to description of the parameter
	sx::d3d::PMaterial	pMtrl;			// pointer to the specified material

	void Set(int param, int elem, PShaderAnnotation annot, sx::d3d::PMaterial mtrl)
	{
		iParam = param;
		iElement = elem;
		pAnnot = annot;
		pMtrl = mtrl;
	}

	bool operator == (ParameterInfo& pinfo)
	{
		return 
			pinfo.iElement	== iElement		&&
			pinfo.iParam	== iParam		&&
			pinfo.pAnnot	== pAnnot		;
			//pinfo.pMtrl		== pMtrl		;
	}
}
*PParameterInfo;

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES

static float shaderGUI_Top = 0;

//  use last shader to decrease gui creation
static uint lastShader = 0;


EditMaterial::EditMaterial( void ): m_material(0)
{
	//  back round of material editor
	m_pBack = sx_new( sx::gui::Panel );
	m_pBack->SetSize( float2(EPSILON, EPSILON) );
	m_pBack->GetElement(0)->Color() = D3DColor( 0.0f, 0.0f, 0.0f, 0.1f );

	//  caption of the material list
	m_pMtrlListTitle = sx_new( sx::gui::Label );
	m_pMtrlListTitle->SetParent(m_pBack);
	m_pMtrlListTitle->SetAlign(GTA_CENTER);
	m_pMtrlListTitle->SetFont( EditorUI::GetDefaultFont(8) );
	m_pMtrlListTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pMtrlListTitle->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
	m_pMtrlListTitle->SetText(L"Material List");

	//  a list to hold the list of materials
	m_pMtrlList = sx_new( sx::gui::ListBox );
	m_pMtrlList->SetParent(m_pBack);
	m_pMtrlList->SetFont( EditorUI::GetDefaultFont(8) );
	m_pMtrlList->SetOnSelect( this, (GUICallbackEvent)&EditMaterial::OnMtrlClick );

	//  a button to add a material
	ADD_BUTTON(m_pMtrlAdd, L"MtrlAdd");
	m_pMtrlAdd->SetOnMouseClick( this, (GUICallbackEvent)&EditMaterial::OnMtrlAdd );
	m_pMtrlAdd->SetHint(L"ADD\n Add a new material shader to the material list.");

	//  a button to remove a material
	ADD_BUTTON(m_pMtrlRemove, L"MtrlRemove");
	m_pMtrlRemove->SetOnMouseClick( this, (GUICallbackEvent)&EditMaterial::OnMtrlRem );
	m_pMtrlRemove->SetHint(L"REMOVE\n Remove selected material from the material list.");

	//  a button for set shader to the specified material
	ADD_BUTTON(m_pSetShader, L"MtrlShader");
	m_pSetShader->SetOnMouseClick( this, (GUICallbackEvent)&EditMaterial::OnMtrlSet );
	m_pSetShader->SetHint(L"CHANGE\n Change selected material");

	//	a button for copy a material
	m_copy = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture( L"copy" ) );
	SEGAN_GUI_SET_ONCLICK( m_copy, EditMaterial::OnClick );
	m_copy->SetHint(L"COPY\n Copy a material shader");

	//	a button for paste a material
	m_paste = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture( L"paste" ) );
	SEGAN_GUI_SET_ONCLICK( m_paste, EditMaterial::OnClick );
	m_paste->SetHint(L"PASTE\n paste a material shader to shader list");

	//	a button for paste a new material
	m_pasteNew = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture( L"pasteNew" ) );
	SEGAN_GUI_SET_ONCLICK( m_pasteNew, EditMaterial::OnClick );
	m_pasteNew->SetHint(L"PASTE NEW\n paste a new material shader to shader list");

	//  a simple splitter of form
	m_pSplitter = sx_new( sx::gui::Panel );
	m_pSplitter->SetParent(m_pBack);
	m_pSplitter->GetElement(0)->Color() = D3DColor(0.1f, 0.1f, 0.1f, 1.0f);

	//  check boxes to manage render states
	m_pAlphaBlend = EditorUI::CreateCheckBox(m_pBack, 16.0f, 70.0f, L"Alpha blend");
	SEGAN_GUI_SET_ONCLICK(m_pAlphaBlend, EditMaterial::OnRSChanged);

	m_pAlphaAdd = EditorUI::CreateCheckBox(m_pBack, 16.0f, 70.0f, L"Alpha add");
	SEGAN_GUI_SET_ONCLICK(m_pAlphaAdd, EditMaterial::OnRSChanged);

	m_pAlphaMul = EditorUI::CreateCheckBox(m_pBack, 16.0f, 70.0f, L"Alpha multiply");
	SEGAN_GUI_SET_ONCLICK(m_pAlphaMul, EditMaterial::OnRSChanged);

	m_pCulling = EditorUI::CreateCheckBox(m_pBack, 70.0f, 50.0f, L"Culling");
	SEGAN_GUI_SET_ONCLICK(m_pCulling, EditMaterial::OnRSChanged);

	m_pZEnable = EditorUI::CreateCheckBox(m_pBack, 16.0f, 50.0f, L"Z Enable");
	SEGAN_GUI_SET_ONCLICK(m_pZEnable, EditMaterial::OnRSChanged);

	m_pZWriting	= EditorUI::CreateCheckBox(m_pBack, 70.0f, 50.0f, L"Z Writable");
	SEGAN_GUI_SET_ONCLICK(m_pZWriting, EditMaterial::OnRSChanged);
}

EditMaterial::~EditMaterial( void )
{
	guiClear();
	sx_delete( m_pMtrlList );
	//  for other things the parent will do everything :)
}

void EditMaterial::SetParent( sx::gui::PControl parent )
{
	m_pBack->SetParent(parent);
}

void EditMaterial::SetSize( float width, float height )
{
	m_Size.Set( width, height );

	float left = -width*0.5f + 20.0f;
	float top = -10;

	m_pMtrlListTitle->SetSize( float2(width - 70.0f, 24.0f) );
	m_pMtrlListTitle->Position().y = top;
	
	top -= 54.0f;
	m_pMtrlList->SetSize( width - 80.0f, 100.0f, 18.0f, false );
	m_pMtrlList->Position().x = 0;//-16.0f;
	m_pMtrlList->Position().y = top;

	top += 32.0f;
	m_pMtrlAdd->Position().x = left;
	m_pMtrlAdd->Position().y = top;

	m_copy->Position().Set( width * 0.5f - 16.0f, top, 0.0f );

	top -= 32.0f;
	m_pMtrlRemove->Position().x = left;
	m_pMtrlRemove->Position().y = top;

	m_paste->Position().Set( width * 0.5f - 16.0f, top, 0.0f );

	top -= 32.0f;
	m_pSetShader->Position().x = left;
	m_pSetShader->Position().y = top;

	m_pasteNew->Position().Set( width * 0.5f - 16.0f, top, 0.0f );

	top -= 30.0f;
	m_pSplitter->SetSize( float2(width , 2.0f) );
	m_pSplitter->Position().y = top;

	top -= 20.0f;
	m_pAlphaBlend->Position().x = left;	
	m_pAlphaAdd->Position().x = left;	
	m_pAlphaMul->Position().x = left;	

	m_pCulling->Position().x = left + 85.0f;
	
	m_pAlphaBlend->Position().y = top;
	m_pAlphaAdd->Position().y = top - 18;
	m_pAlphaMul->Position().y = top - 36;
	
	m_pCulling->Position().y = top;

	left += 145.0f;
	//top -= 20.0f;
	m_pZEnable->Position().x = left;	m_pZWriting->Position().x = left + 65.0f;
	m_pZEnable->Position().y = top;		m_pZWriting->Position().y = top;

	top -= 36;
	shaderGUI_Top = - top + 20.0f;
}


void EditMaterial::Update( float elpsTime )
{
	m_pMtrlList->Update(elpsTime);
}

void EditMaterial::SetMaterialToEdit( sx::core::PMaterialMan material )
{
	//  verify the material
	if ( m_material == material ) return;
	m_material = material;
	int index = m_material ? m_material->GetActiveMaterialIndex() : 0;

	//  update material list
	UpdateItemsInList();
	m_pMtrlList->SetItemIndex( index );

	//  prepare gui to edit shader constants
	guiPrepare();
}

//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
sx::gui::PTrackBar EditMaterial::guiCreateTrakbar( sx::d3d::PMaterial mtrl, ShaderAnnotation& shDesc, void* paramInfo )
{
	float width = m_Size.x - 30.0f;
	PParameterInfo pInfo	= (PParameterInfo)paramInfo;
	sx::gui::PTrackBar tr	= NULL;

	//  traverse through list of gui to find last created gui which is match our request
	for (int i=0; i<m_arrayGUI.Count(); i++)
	{
		sx::gui::PControl pGUI = m_arrayGUI[i];
		if ( pGUI->GetType() == GUI_TRACKBAR )
		{
			PParameterInfo pInfoGUI = (PParameterInfo)pGUI->GetUserData();
			if ( *pInfoGUI == *pInfo )
			{
				//  release allocated memory as user data
				sx_mem_free( pInfoGUI );

				tr = (sx::gui::PTrackBar)pGUI;
				break;
			}
		}
	}

	//  create control
	if ( !tr )
	{
		tr = sx_new( sx::gui::TrackBar );
		tr->SetSize( float2(width, 16.0f) );
		tr->GetElement(0)->SetTextureSrc( EditorUI::GetScrollTexture(0) );
		tr->GetElement(1)->SetTextureSrc( EditorUI::GetScrollTexture(1) );
		m_arrayGUI.PushBack(tr);
	}

	//  set parameter info to control
	tr->SetUserData(paramInfo);

	//  set default values of control
	switch (shDesc.Type)
	{
	case SPT_FLOAT:
		{
			tr->SetMin( shDesc.uiData[0] );
			tr->SetMax( shDesc.uiData[1] );
			tr->SetOnScroll( this, (GUICallbackEvent)&EditMaterial::OnParamChange );

			if ( FLT_MAX - mtrl->GetFloat(pInfo->iParam) < 10.0f )
				tr->SetValue( shDesc.uiData[2] );
			else
				tr->SetValue( mtrl->GetFloat(pInfo->iParam) );
		}
		break;

	case SPT_FLOAT4:
		{
			tr->SetMin( -1.0f );
			tr->SetMax( 1.0f );
			tr->SetOnScroll( this, (GUICallbackEvent)&EditMaterial::OnParamChange );

			float4 v4 = mtrl->GetFloat4(pInfo->iParam);
			if ( FLT_MAX - v4[pInfo->iElement] < 10.0f )
				tr->SetValue( shDesc.uiData[pInfo->iElement] );
			else
				tr->SetValue( v4[pInfo->iElement] );
		}
		break;
	}

	//  final settings
	tr->SetParent( m_pBack );

	return tr;
}

sx::gui::PTextEdit EditMaterial::guiCreateTextEdit( sx::d3d::PMaterial mtrl, ShaderAnnotation& shDesc, void* paramInfo )
{
	float width = m_Size.x - 30.0f;
	PParameterInfo pInfo	= (PParameterInfo)paramInfo;
	sx::gui::PTextEdit ed	= NULL;

	//  traverse through list of gui to find last created gui which is match our request
	for (int i=0; i<m_arrayGUI.Count(); i++)
	{
		sx::gui::PControl pGUI = m_arrayGUI[i];
		if ( pGUI->GetType() == GUI_TEXTEDIT )
		{
			PParameterInfo pInfoGUI = (PParameterInfo)pGUI->GetUserData();
			if ( *pInfoGUI == *pInfo )
			{
				//  release allocated memory as user data
				sx_mem_free( pInfoGUI );

				ed = (sx::gui::PTextEdit)pGUI;
				break;
			}
		}
	}

	//  create control
	if ( !ed )
	{
		ed = EditorUI::CreateEditBox(NULL, 0, 50.0f);
		SEGAN_GUI_SET_ONWHEEL(ed, EditMaterial::OnMouseWheel);
		m_arrayGUI.PushBack(ed);
	}

	//  set parameter info to control
	ed->SetUserData(paramInfo);

	//  set default values of control
	switch (shDesc.Type)
	{
	case SPT_FLOAT:
		{
			ed->SetSize( float2(width/3, 18.0f) );
			ed->Position().x = width/3;	

			ed->SetOnTextChange( this, (GUICallbackEvent)&EditMaterial::OnParamChange );
			if ( FLT_MAX - mtrl->GetFloat(pInfo->iParam) < 10.0f )
				ed->SetText(FloatToStr(shDesc.uiData[2]) );
			else
				ed->SetText(FloatToStr(mtrl->GetFloat(pInfo->iParam)) );

		}
		break;

	case SPT_FLOAT4:
		{
			ed->SetSize( float2(width/7, 18.0f) );
			ed->Position().x = - width/2 + width/3 + width/12 + pInfo->iElement*width/6;

			ed->SetOnTextChange( this, (GUICallbackEvent)&EditMaterial::OnParamChange );
			float4 v4 = mtrl->GetFloat4(pInfo->iParam);
			if ( FLT_MAX - v4[pInfo->iElement] < 10.0f )
				ed->SetText(FloatToStr(shDesc.uiData[pInfo->iElement]) );
			else
				ed->SetText(FloatToStr(v4[pInfo->iElement]) );
		}
		break;

	}

	//  final settings
	ed->SetParent( m_pBack );

	return ed;
}

sx::gui::PButton EditMaterial::guiCreateButton( sx::d3d::PMaterial mtrl, ShaderAnnotation& shDesc, void* paramInfo )
{
	float width = m_Size.x - 30.0f;
	PParameterInfo pInfo	= (PParameterInfo)paramInfo;
	sx::gui::PButton bt		= NULL;
	sx::gui::PLabel lb		= NULL;

	//  traverse through list of gui to find last created gui which is match our request
	for (int i=0; i<m_arrayGUI.Count(); i++)
	{
		sx::gui::PControl pGUI = m_arrayGUI[i];
		if ( pGUI->GetType() == GUI_BUTTON )
		{
			PParameterInfo pInfoGUI = (PParameterInfo)pGUI->GetUserData();
			if ( *pInfoGUI == *pInfo )
			{
				//  release allocated memory as user data
				sx_mem_free( pInfoGUI );

				bt = (sx::gui::PButton)pGUI;
				lb = (sx::gui::PLabel)bt->GetChild(0);
				break;
			}
		}
	}

	//  create control
	if ( !bt )
	{
		bt = EditorUI::CreateButton(m_pBack, 256, L"...");
		lb = (sx::gui::PLabel)bt->GetChild(0);
		m_arrayGUI.PushBack(bt);
	}

	//  set parameter info to control
	bt->SetUserData(paramInfo);

	//  set default values of control
	bt->SetOnMouseClick( this, (GUICallbackEvent)&EditMaterial::OnParamChange );
	if ( mtrl->GetTexture(pInfo->iParam) )
		lb->SetText( mtrl->GetTexture(pInfo->iParam) );
	else
		lb->SetText( L"..." );

	return bt;
}

float EditMaterial::guiHeight( sx::gui::PControl gui )
{
	if ( !gui ) return 0.0f;

	if ( gui->GetType() == GUI_TEXTEDIT ) return 0.0f;
	//PParameterInfo pInfo = (PParameterInfo)gui->GetUserData();
	//if ( !pInfo ) return gui->GetSize().y;
	//if ( pInfo->iElement && gui->GetType() == GUI_TEXTEDIT ) return 0.0f;

	return gui->GetSize().y;
}

void EditMaterial::guiClear( void )
{
	for (int i=0; i<m_arrayGUI.Count(); i++)
	{
		void* udate = m_arrayGUI[i]->GetUserData();
		sx_mem_free( udate );
		m_arrayGUI[i]->SetParent( NULL );
		sx::gui::Destroy( m_arrayGUI[i] );
	}
	m_arrayGUI.Clear();

//	m_pAlphaBlend->Checked()= false;
//	m_pCulling->Checked()	= false;
//	m_pZEnable->Checked()	= false;
//	m_pZWriting->Checked()	= false;

	Editor::frm_Explorer->SetUserData( NULL );
}

void EditMaterial::guiPrepare( void )
{
	if (!m_material)
	{
		guiClear();
		lastShader = 0;
		return;
	}

	float width = m_Size.x - 30.0f;

	int index = m_pMtrlList->GetItemIndex();
	sx::d3d::PMaterial mtrl = m_material->Get(index);
	if (mtrl)
	{
		DWORD op = mtrl->GetOption();
		m_pAlphaBlend->Checked()= SEGAN_SET_HAS(op, SX_MATERIAL_ALPHABLEND);
		m_pAlphaAdd->Checked()	= SEGAN_SET_HAS(op, SX_MATERIAL_ALPHAADD);
		m_pAlphaMul->Checked()	= SEGAN_SET_HAS(op, SX_MATERIAL_ALPHAMUL);
		m_pCulling->Checked()	= SEGAN_SET_HAS(op, SX_MATERIAL_CULLING);
		m_pZEnable->Checked()	= SEGAN_SET_HAS(op, SX_MATERIAL_ZENABLE);
		m_pZWriting->Checked()	= SEGAN_SET_HAS(op, SX_MATERIAL_ZWRITING);

		sx::d3d::PShader pShader = NULL;
		if ( sx::d3d::Shader::Manager::Exist( pShader, mtrl->GetShader() ) )
		{//  shader founded and ready to use

			//  check that shader changed
			if ( pShader->GetID() != lastShader ) 
				guiClear();
			
			//  traverse between parameters to create controls
			int iFloat=0, iFloat4=0, iTexture=0;
			for (int i=0; i<pShader->GetParamCount(); i++)
			{
				ShaderAnnotation& shDesc = *(pShader->GetParam(i)->GetDesc());
				if (shDesc.Type == SPT_UNKNOWN) continue;

				//  create label as caption of the control
				if ( pShader->GetID() != lastShader)
				{
					sx::gui::PLabel lb = sx_new( sx::gui::Label );
					lb->SetParent(m_pBack);
					lb->SetAlign(GTA_LEFT);
					lb->SetFont( EditorUI::GetDefaultFont(8) );
					lb->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
					lb->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
					lb->SetSize( float2(width, 30.0f) );

					str64 caption = shDesc.uiName;
					lb->SetText( caption );
					m_arrayGUI.PushBack(lb);
				}

				if ( shDesc.Type == SPT_FLOAT )
				{
					if ( strcmp(shDesc.uiType, "trackbar")==0 )
					{
						
						PParameterInfo pInfo = (PParameterInfo)sx_mem_alloc( sizeof(ParameterInfo) );
						pInfo->Set(iFloat, 0, &shDesc, mtrl);
						guiCreateTrakbar(mtrl, shDesc, pInfo);
					}
					else if ( strcmp(shDesc.uiType, "edittext")==0 )
					{
						PParameterInfo pInfo = (PParameterInfo)sx_mem_alloc( sizeof(ParameterInfo) );
						pInfo->Set(iFloat, 0, &shDesc, mtrl);
						guiCreateTextEdit(mtrl, shDesc, pInfo);
					}

					iFloat++;
				}
				else if ( shDesc.Type == SPT_FLOAT4 )
				{
					if ( strcmp(shDesc.uiType, "trackbar")==0 )
					{
						for (int c=0; c<4; c++)
						{
							PParameterInfo pInfo = (PParameterInfo)sx_mem_alloc( sizeof(ParameterInfo) );
							pInfo->Set(iFloat4, c, &shDesc, mtrl);
							guiCreateTrakbar(mtrl, shDesc, pInfo);
						}						
					}
					else if ( strcmp(shDesc.uiType, "edittext")==0 )
					{
						for (int c=0; c<4; c++)
						{
 							PParameterInfo pInfo = (PParameterInfo)sx_mem_alloc( sizeof(ParameterInfo) );
 							pInfo->Set(iFloat4, c, &shDesc, mtrl);
 							guiCreateTextEdit(mtrl, shDesc, pInfo);
						}
					}

					iFloat4++;
				}
				else if ( shDesc.Type == SPT_SAMPLER )
				{
					PParameterInfo pInfo = (PParameterInfo)sx_mem_alloc( sizeof(ParameterInfo) );
					pInfo->Set(iTexture, 0, &shDesc, mtrl);
					guiCreateButton(mtrl, shDesc, pInfo);
					iTexture++;
				}
			}

			//  save last shader to decrease gui creations
			lastShader = pShader->GetID();

		}// if ( sx::d3d::Shader::Manager::Exist( pShader, mtrl->GetShader() ) )
		else
		{
			guiClear();
			lastShader = 0;
		}

	}// if (mtrl)
	else 
	{
		guiClear();
		lastShader = 0;
	}

	//  resize form depend on objects
	float h = 0;
	for (int i=0; i<m_arrayGUI.Count(); i++)
		h += guiHeight(m_arrayGUI[i]);

	//  rearrange objects on the form
	float top = - shaderGUI_Top;
	h = 0;
	for (int i=0; i<m_arrayGUI.Count(); i++)
	{
		sx::gui::PControl gui = m_arrayGUI[i];

		top +=  guiHeight(gui)<1.0f ? h : 0;
		h = guiHeight(gui) / 2;
		top -= h;
		gui->Position().y = top - (float)(gui->GetType()==GUI_LABEL)*10 - (float)(gui->GetType()==GUI_TEXTEDIT)*5;
		top -= h;
	}

	m_OnResize(m_pBack);
}

void EditMaterial::OnOpenShader( sx::gui::PControl Sender )
{
	if (!m_material) return;

	int index = m_pMtrlList->GetItemIndex();
	sx::d3d::PMaterial mtrl = m_material->Get(index);
	if (mtrl)
	{
		str1024 str = Editor::frm_Explorer->GetPath();
		str.ExtractFileExtension();
		str.MakeLower();
		if (str == L"fx" || str == L"cs")
		{
			lastShader = NULL;

			//  import shader to library
			str = Editor::frm_Explorer->GetPath();
			str.ExtractFileName();
			str.ExcludeFileExtension();
			str << L".sfx";

			String		 strfx;
			sx::cmn::String_Load( strfx, Editor::frm_Explorer->GetPath() );

			MemoryStream stm;
			sx::cmn::String_Save( strfx, &stm );
			sx::sys::FileManager::File_Save( str, SEGAN_PACKAGENAME_EFFECT, stm );
			
			//  verify if the shader has been loaded easily recompile this
			sx::d3d::PShader pShader = NULL;
			if ( sx::d3d::Shader::Manager::Exist( pShader, str ) )
			{
				pShader->SetSource( str );
			}

			//  continue applying shader
			mtrl->SetShader( str );
 			UpdateItemsInList();
			guiPrepare();
		}
		
		if (str == L"sfx")
		{
			lastShader = NULL;

			//  just apply shader to selected material
			str = Editor::frm_Explorer->GetPath();
			str.ExtractFileName();
			mtrl->SetShader( str );
 			UpdateItemsInList();
			guiPrepare();
		}
		
	}

	//Editor::frm_Explorer->Close();
}

void EditMaterial::OnOpenTexture( sx::gui::PControl Sender )
{
	if (!Sender || !m_material) return;
	sx::gui::PControl pGUI = (sx::gui::PControl)Editor::frm_Explorer->GetUserData();
	if (!pGUI) return;
	PParameterInfo pInfo = (PParameterInfo)pGUI->GetUserData();
	if (!pInfo) return;

	int index = m_pMtrlList->GetItemIndex();
	sx::d3d::PMaterial mtrl = m_material->Get(index);
	if ( mtrl == pInfo->pMtrl )
	{
		String str = Editor::frm_Explorer->GetPath();
		str.ExtractFileExtension();
		str.MakeLower();
		if (str == L"txr")
		{
			String libPath;
			sx::sys::FileManager::Package_GetPath( SEGAN_PACKAGENAME_TEXTURE, libPath );
			str1024 texPath = Editor::frm_Explorer->GetPath();
			texPath.ExtractFilePath();

			if( texPath != libPath )
			{
				str1024 tmp = Editor::frm_Explorer->GetPath();
				tmp.ExtractFileName();
				libPath.MakePathStyle();
				libPath << tmp;
				sx::sys::CopyFile( Editor::frm_Explorer->GetPath(), libPath );
			}

			str = Editor::frm_Explorer->GetPath();
			str.ExtractFileName();

			sx::d3d::PTexture tex = NULL;
			if ( sx::d3d::Texture::Manager::Exist( tex, str ) )
				tex->Reload();

			mtrl->SetTexture( pInfo->iParam, str );
			sx::gui::PLabel lb = (sx::gui::PLabel)pGUI->GetChild(0);
			if (lb && lb->GetType() == GUI_LABEL) lb->SetText( str );
		}
		else
		{
			//  import texture to library
			str = Editor::frm_Explorer->GetPath();
			str.ExtractFileName();
			str.ExcludeFileExtension();
			str << L".txr";

			sx::sys::FileManager::File_GetPath(str, SEGAN_PACKAGENAME_TEXTURE, str);
			ConvertTexture( Editor::frm_Explorer->GetPath(), str );

			//  continue loading file
			str.ExtractFileName();

			sx::d3d::PTexture tex = NULL;
			if ( sx::d3d::Texture::Manager::Exist( tex, str ) )
				tex->Reload();

			mtrl->SetTexture( pInfo->iParam, str );
			sx::gui::PLabel lb = (sx::gui::PLabel)pGUI->GetChild(0);
			if (lb && lb->GetType() == GUI_LABEL) lb->SetText( str );
		}
	}
}

void EditMaterial::OnMtrlAdd( sx::gui::PControl Sender )
{
	if (!m_material) return;
	lastShader = NULL;
	m_material->Add();
	int index = m_material->Count() - 1;

	UpdateItemsInList();
	m_pMtrlList->SetItemIndex(index);
	guiPrepare();
	OnMtrlSet(NULL);
}

void EditMaterial::OnMtrlRem( sx::gui::PControl Sender )
{
	if (!m_material) return;

	int index = m_pMtrlList->GetItemIndex();
	if (index>-1 && m_material->Count()>1)
	{
		m_material->Remove(index);
		m_pMtrlList->Remove(index);
		lastShader = NULL;
		guiPrepare();
	}
}

void EditMaterial::OnMtrlSet( sx::gui::PControl Sender )
{
	if (!m_material) return;

	int index = m_pMtrlList->GetItemIndex();
	sx::d3d::PMaterial mtrl = m_material->Get(index);
	if (mtrl)
	{
		Editor::frm_Explorer->SetTitle(L"Open Shader");
		Editor::frm_Explorer->Open( this, (GUICallbackEvent)&EditMaterial::OnOpenShader );
	}
}

void EditMaterial::OnMtrlClick( sx::gui::PControl Sender )
{
	if (!m_material) return;

	int index = m_pMtrlList->GetItemIndex();
	sx::d3d::PMaterial mtrl = m_material->Get(index);
	if (mtrl)
	{
		if ( index != m_material->GetActiveMaterialIndex() )
		{
			m_material->SetActiveMaterial( index );
			guiPrepare();
		}
	}
}

void EditMaterial::OnParamChange( sx::gui::PControl Sender )
{
	if (!Sender || !m_material) return;
	PParameterInfo pInfo = (PParameterInfo)Sender->GetUserData();
	if (!pInfo) return;

	switch (Sender->GetType())
	{
	case GUI_TRACKBAR:
		{
			sx::gui::PTrackBar tr = (sx::gui::PTrackBar)Sender;

			if (pInfo->pAnnot->Type == SPT_FLOAT)
			{
				pInfo->pMtrl->SetFloat( pInfo->iParam, tr->GetValue() );
			}
			else
			{
				float4 v4 = pInfo->pMtrl->GetFloat4( pInfo->iParam );
				v4[ pInfo->iElement ] = tr->GetValue();
				pInfo->pMtrl->SetFloat4( pInfo->iParam, v4 );
			}
			
		}
		break;

	case GUI_TEXTEDIT:
		{
			sx::gui::PTextEdit ed = (sx::gui::PTextEdit)Sender;

			if (pInfo->pAnnot->Type == SPT_FLOAT)
			{
				pInfo->pMtrl->SetFloat( pInfo->iParam, str128::StrToFloat( ed->GetText() ) );
			}
			else
			{
				float4 v4 = pInfo->pMtrl->GetFloat4( pInfo->iParam );
				v4[ pInfo->iElement ] = str128::StrToFloat( ed->GetText() );
				pInfo->pMtrl->SetFloat4( pInfo->iParam, v4 );
			}
		}
		break;

	case GUI_BUTTON:
		{
			sx::gui::PButton bt = (sx::gui::PButton)Sender;

			if (pInfo->pAnnot->Type == SPT_SAMPLER)
			{
				Editor::frm_Explorer->SetUserData( Sender );
				Editor::frm_Explorer->SetTitle( L"Open Texture" );
				Editor::frm_Explorer->Open( this, (GUICallbackEvent)&EditMaterial::OnOpenTexture );
			}
		}
		break;
	}
}

float3& EditMaterial::Position( void )
{
	return m_pBack->Position();
}

void EditMaterial::SetOnSizeChanged( sx::gui::PForm pForm, GUICallbackEvent pFunc )
{
	m_OnResize.m_pForm = pForm;
	m_OnResize.m_pFunc = pFunc;
}

float EditMaterial::GetHeight( void )
{
	float h = shaderGUI_Top;
	for (int i=0; i<m_arrayGUI.Count(); i++)
		h += guiHeight(m_arrayGUI[i]);
	return h;
}

void EditMaterial::OnMouseWheel( sx::gui::PControl Sender )
{
	if ( !m_material ) return;

	switch (Sender->GetType())
	{
	case GUI_TEXTEDIT:
		{
			sx::gui::PTextEdit pEdit = (sx::gui::PTextEdit)Sender;
			if ( pEdit->GetFocused() )
			{
				float r = str128::StrToFloat( pEdit->GetText() );
				r += sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL] * 0.1f;
				pEdit->SetText(FloatToStr(	r ) );
			}
		}
	}
}

void EditMaterial::OnRSChanged( sx::gui::PControl Sender )
{
	if (!Sender || !m_material) return;

	int index = m_pMtrlList->GetItemIndex();

	if (index > -1)
	{
		DWORD op = m_material->Get(index)->GetOption();

		if (Sender == m_pAlphaBlend)
		{
			if ( m_pAlphaBlend->Checked() )
				SEGAN_SET_ADD(op, SX_MATERIAL_ALPHABLEND);
			else
				SEGAN_SET_REM(op, SX_MATERIAL_ALPHABLEND);
		}

		if (Sender == m_pAlphaAdd)
		{
			if ( m_pAlphaAdd->Checked() )
				SEGAN_SET_ADD(op, SX_MATERIAL_ALPHAADD);
			else
				SEGAN_SET_REM(op, SX_MATERIAL_ALPHAADD);
		}

		if (Sender == m_pAlphaMul)
		{
			if ( m_pAlphaMul->Checked() )
				SEGAN_SET_ADD(op, SX_MATERIAL_ALPHAMUL);
			else
				SEGAN_SET_REM(op, SX_MATERIAL_ALPHAMUL);
		}

		if (Sender == m_pCulling)
		{
			if ( m_pCulling->Checked() )
			{
				SEGAN_SET_ADD(op, SX_MATERIAL_CULLING);
			}
			else
			{
				SEGAN_SET_REM(op, SX_MATERIAL_CULLING);
			}
		}

		if (Sender == m_pZEnable)
		{
			if ( m_pZEnable->Checked() )
			{
				SEGAN_SET_ADD(op, SX_MATERIAL_ZENABLE);
			}
			else
			{
				SEGAN_SET_REM(op, SX_MATERIAL_ZENABLE);
			}
		}

		if (Sender == m_pZWriting)
		{
			if ( m_pZWriting->Checked() )
			{
				SEGAN_SET_ADD(op, SX_MATERIAL_ZWRITING);
			}
			else
			{
				SEGAN_SET_REM(op, SX_MATERIAL_ZWRITING);
			}
		}


		m_material->Get(index)->SetOption(op);

	}
}

void EditMaterial::OnClick( sx::gui::PControl sender )
{
	static MemoryStream copypaste;
	if ( !m_material ) return;

	if ( sender == m_copy && m_material->GetActiveMaterial() )
	{
		copypaste.Clear();
		m_material->GetActiveMaterial()->Save( copypaste );
	}
	else if ( sender == m_paste && m_material->GetActiveMaterial() && copypaste.Size() > 0 )
	{
		copypaste.SetPos(0);
		m_material->GetActiveMaterial()->Load( copypaste );
		UpdateItemsInList();
		guiPrepare();
	}
	else if ( sender == m_pasteNew && copypaste.Size() > 0 )
	{
		copypaste.SetPos(0);
		lastShader = NULL;
		m_material->Add();
		int index = m_material->Count() - 1;
		m_material->Get( index )->Load( copypaste );

		UpdateItemsInList();
		m_pMtrlList->SetItemIndex(index);
		guiPrepare();
	}
}

void EditMaterial::UpdateItemsInList( void )
{
	int index = m_pMtrlList->GetItemIndex();
	m_pMtrlList->Clear();
	if ( m_material )
	{
		for (int i=0; i<m_material->Count(); i++)
		{
			str512 tmpstr;
			if ( m_material->Get(i)->GetShader() )
				tmpstr.Format( L"%.2d : %s", i, m_material->Get(i)->GetShader() );
			else
				tmpstr.Format( L"%.2d : no shader !", i );

			m_pMtrlList->Add( tmpstr, NULL );
		}
		m_pMtrlList->SetItemIndex( m_material->GetActiveMaterialIndex() );
	}
	m_pMtrlList->SetItemIndex(index);
}
