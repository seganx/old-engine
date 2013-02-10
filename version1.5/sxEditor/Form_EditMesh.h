/********************************************************************
	created:	2011/05/29
	filename: 	Form_EditMesh.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a form to edit materials of a mesh
*********************************************************************/
#ifndef GUARD_Form_EditMesh_HEADER_FILE
#define GUARD_Form_EditMesh_HEADER_FILE

#include "BaseForm.h"
#include "EditMaterial.h"

/*!
use to edit meshes
*/
class Form_EditMesh: public BaseForm
{

public:
	Form_EditMesh(void);
	virtual ~Form_EditMesh(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

	//! assign mesh to edit. call it frequently in loop to check changes.
	void SetMeshToEdit(sx::core::PNodeMember mesh);

private:

	void ShowMeshInfo(void);
	void OnMaterialResized(sx::gui::PControl Sender);	//  apply change size of material editor
	void OnParamChange(sx::gui::PControl Sender);		//  apply changes of an parameter to material

private:

	sx::gui::PLabel			m_pTitle;			//	title of the form

	sx::gui::PLabel			m_lblInfo;			//  label of information
	sx::gui::PCheckBox		m_CastShadow;		//  the mesh can cast shadow
	sx::gui::PCheckBox		m_RecieveShadow;	//  the mesh render with shadow
	sx::gui::PCheckBox		m_Invisible;		//  the mesh will not render
	sx::gui::PCheckBox		m_hasReflect;			//  the mesh will render to reflectors
	sx::gui::PButton		m_EditGeometry;		//  edit geometry mesh
	EditMaterial			m_EditMaterial;		//  material editor

	sx::core::PMesh			m_mesh;				//  mesh to edit

	int						m_heightPlus;		//  additional height

};
typedef Form_EditMesh *PForm_EditMesh;

#endif	//	GUARD_Form_EditMesh_HEADER_FILE


