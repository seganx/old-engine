#include "Form_EditMesh.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "ImportTexture.h"

#define EDITMESH_WIDTH			300.0f		//  size of the edit mesh form
#define EDITMESH_HEIGHT			300.0f		//  size of the edit mesh form

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

Form_EditMesh::Form_EditMesh( void ): BaseForm(), m_heightPlus(0)
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

	float tl = - EDITMESH_WIDTH * 0.5f + 20.0f;
	m_lblInfo		= EditorUI::CreateLabel(m_pBack, EDITMESH_WIDTH-24, 8, L"info");
	m_Invisible		= EditorUI::CreateCheckBox(m_pBack, tl, 150.0f, L"Invisible");
	m_CastShadow	= EditorUI::CreateCheckBox(m_pBack, tl, 150.0f, L"Cast Shadow");
	m_RecieveShadow	= EditorUI::CreateCheckBox(m_pBack, tl, 150.0f, L"Receive Shadow");
	m_hasReflect	= EditorUI::CreateCheckBox(m_pBack, tl, 150.0f, L"Has Reflect");
	SEGAN_GUI_SET_ONCLICK(m_CastShadow,		Form_EditMesh::OnParamChange);
	SEGAN_GUI_SET_ONCLICK(m_RecieveShadow,	Form_EditMesh::OnParamChange);
	SEGAN_GUI_SET_ONCLICK(m_hasReflect,		Form_EditMesh::OnParamChange);
	SEGAN_GUI_SET_ONCLICK(m_Invisible,		Form_EditMesh::OnParamChange);

	m_EditGeometry	= EditorUI::CreateButtonEx(m_pBack, 32, 28, EditorUI::GetButtonTexture(L"EditGeometry", 0) );
	SEGAN_GUI_SET_ONCLICK(m_EditGeometry,	Form_EditMesh::OnParamChange);
	m_EditGeometry->SetHint(L"EDIT GEOMETRY :: \nCurrently apply these change to the geometry and save it automatically.\n\
		Weld vertices depend on position and texture coordinates.\n\
		Recompute Normal and Tangent vectors.\n\
		Regenerate 2 sub LOD. ");

	m_scale = EditorUI::CreateLabeldEditBox( m_pBack, 200, 50.0f, 50.0f, L"Scale :" );

	m_EditMaterial.SetParent(m_pBack);
	m_EditMaterial.SetOnSizeChanged(this, (GUICallbackEvent)&Form_EditMesh::OnMaterialResized);

	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	m_MinSize.x = 100.0f;	
	m_MinSize.y = 50.0f;
	
	SetSize( EDITMESH_WIDTH, EDITMESH_HEIGHT );
}

Form_EditMesh::~Form_EditMesh( void )
{
	//  for other things the parent will do everything :)
}

void Form_EditMesh::SetSize( float width, float height )
{
	float top = height*0.5f - 15.0f;
	float left = -width*0.5f;

	m_pTitle->SetSize( float2(width - 70.0f, 50.0f) );
	m_pTitle->Position().y = top;
	top -= 35.0f;

	m_EditGeometry->Position().Set( - left - 32.0f, top, 0.0f );
	m_scale->Position().Set( - left - 40.0f, top - 70.0f, 0.0f );

	top = height*0.5f - 40.0f;
	m_lblInfo->Position().y = top;			top -= 20.0f;
	m_Invisible->Position().y = top;		top -= 20.0f;
	m_CastShadow->Position().y = top;		top -= 20.0f;
	m_RecieveShadow->Position().y = top;	top -= 20.0f;	
	m_hasReflect->Position().y = top;		top -= 20.0f;	
	
	m_heightPlus = 130;
	m_EditMaterial.Position().y = top;
	m_EditMaterial.SetSize(width, height);

	BaseForm::SetSize(width, height);
}

void Form_EditMesh::ProcessInput( bool& inputHandled )
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

void Form_EditMesh::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	BaseForm::Update(elpsTime);

	m_EditMaterial.Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;
	
}

void Form_EditMesh::OnResize( int EditorWidth, int EditorHeight )
{
// 	float offsetW = (float)(EditorWidth  % 2);
// 	float offsetH = (float)(EditorHeight  % 2);
// 	float width = (float)EditorWidth + offsetW;
// 	float height = (float)EditorHeight + offsetH;
}

void Form_EditMesh::SetMeshToEdit( sx::core::PNodeMember mesh )
{
	//  verify the mesh
	if ( !mesh || mesh->GetType() != NMT_MESH )
	{
		m_mesh = NULL;
		m_EditMaterial.SetMaterialToEdit(NULL);
		Close();
		return;
	}
	if ( !IsVisible() || m_mesh == mesh ) return;
	m_mesh = (sx::core::PMesh)mesh;

	//  set title
	if ( m_mesh->GetName() )
	{
		str1024 str = L"Edit Mesh\n";
		str << m_mesh->GetName();
		m_pTitle->SetText( str );
	}
	else m_pTitle->SetText(L"Edit Mesh - [no name]");

	m_CastShadow->Checked() = m_mesh->HasOption(SX_MESH_CASTSHADOW);
	m_RecieveShadow->Checked() = m_mesh->HasOption(SX_MESH_RECIEVESHADOW);
	m_hasReflect->Checked() = !m_mesh->HasOption(SX_MESH_INVIS_IN_REFLET);
	m_Invisible->Checked() = m_mesh->HasOption(SX_MESH_INVISIBLE);

	ShowMeshInfo();

	//  update material list
	m_EditMaterial.SetMaterialToEdit( &m_mesh->Material() );

	m_scale->SetOnTextChange( NULL, NULL );
	m_scale->SetText( FloatToStr( m_mesh->m_scale.x, 2 ) );
	SEGAN_GUI_SET_ONTEXT( m_scale, Form_EditMesh::OnParamChange );
}

//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
void Form_EditMesh::ShowMeshInfo( void )
{
	str1024 str;
	sx::d3d::PGeometry gm = NULL;
	if ( sx::d3d::Geometry::Manager::Exist(gm, m_mesh->GetGeometry()) )
		str.Format( L"Vertices: %d  -  Triangles: %d", gm->GetVertexCount(0), gm->GetFaceCount(0) );
	else
		str = L"Vertices: 0  -  Triangles: 0";
	m_lblInfo->SetText(str);
}

void Form_EditMesh::OnMaterialResized( sx::gui::PControl Sender )
{
	//  update form size depend of height of material editor
	float2 cursize = GetSize();
	cursize.y = m_heightPlus + m_EditMaterial.GetHeight();
	SetSize(cursize.x, cursize.y );
}


void Form_EditMesh::OnParamChange( sx::gui::PControl Sender )
{
	if ( !Sender || !m_mesh ) return;

	if ( m_CastShadow->Checked() )
		m_mesh->AddOption( SX_MESH_CASTSHADOW );
	else
		m_mesh->RemOption( SX_MESH_CASTSHADOW );

	if ( m_RecieveShadow->Checked() )
		m_mesh->AddOption( SX_MESH_RECIEVESHADOW );
	else
		m_mesh->RemOption( SX_MESH_RECIEVESHADOW );

	if ( m_hasReflect->Checked() )
		m_mesh->RemOption( SX_MESH_INVIS_IN_REFLET );
	else
		m_mesh->AddOption( SX_MESH_INVIS_IN_REFLET );

	if ( m_Invisible->Checked() )
		m_mesh->AddOption( SX_MESH_INVISIBLE );
	else
		m_mesh->RemOption( SX_MESH_INVISIBLE );

	if ( Sender == m_EditGeometry )
	{
		sx::d3d::PGeometry gm = NULL;
		if ( sx::d3d::Geometry::Manager::Exist(gm, m_mesh->GetGeometry()) )
		{
			if ( !gm->GetRes(0)->VB_IsEmpty() && !gm->GetRes(0)->IB_IsEmpty() )
			{
				float lod1 =  (float)gm->GetRes(1)->GetFaceCount() / (float)gm->GetRes(0)->GetFaceCount();
				float lod2 =  (float)gm->GetRes(2)->GetFaceCount() / (float)gm->GetRes(0)->GetFaceCount();

				gm->GetRes(0)->Weld_Optimize_ComputeNT();
				gm->GetRes(0)->UpdateBoundingVolume();

				String		  str = L"simplify "; str << m_mesh->GetName();
				gm->GenerateSubLOD( lod1, lod2, str );

				//  save geometry
				MemoryStream mem;
				gm->Save(mem);
				sx::sys::FileManager::File_Save( m_mesh->GetGeometry(), SEGAN_PACKAGENAME_GEOMETRY, mem );
			}
		}

		ShowMeshInfo();
	}

	if ( Sender == m_scale )
	{
		str64 tmp = m_scale->GetText();
		float v = tmp.ToFloat();
		m_mesh->SetScale( v, v, v );
	}

}



