/********************************************************************
	created:	2011/06/16
	filename: 	EditMaterial.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a editor panel for edit material of
				members
*********************************************************************/
#ifndef GUARD_EditMaterial_HEADER_FILE
#define GUARD_EditMaterial_HEADER_FILE

#include "sxListBox.h"

//  use this class to edit materials
class EditMaterial : public sx::gui::Form
{

public:
	EditMaterial(void);
	virtual ~EditMaterial(void);

	//! set parent
	void SetParent(sx::gui::PControl parent);

	//! set size of the form
	void SetSize(float width, float height);

	//! update the form
	void Update(float elpsTime);

	//! assign material to edit. call it frequently in loop to check changes.
	void SetMaterialToEdit(sx::core::PMaterialMan material);

	//! reference to the position
	float3& Position(void);

	//! set call back function to change size of form
	void SetOnSizeChanged(sx::gui::PForm pForm, GUICallbackEvent pFunc);

	//! return the variable height of material editor
	float GetHeight(void);

	void UpdateItemsInList(void);

private:

	sx::gui::PTrackBar guiCreateTrakbar(sx::d3d::PMaterial mtrl, ShaderAnnotation& shDesc, void* paramInfo	);
	sx::gui::TextEdit* guiCreateTextEdit(sx::d3d::PMaterial mtrl, ShaderAnnotation& shDesc, void* paramInfo	);
	sx::gui::PButton   guiCreateButton(sx::d3d::PMaterial mtrl, ShaderAnnotation& shDesc, void* paramInfo	);

	float guiHeight(sx::gui::PControl gui);			//  return height of the gui
	void guiClear(void);							//  clear created gui for edit shader constants
	void guiPrepare(void);							//  create and prepare gui for edit shader constants
	void OnOpenShader(sx::gui::PControl Sender);	//  called by explorer to open shader
	void OnOpenTexture(sx::gui::PControl Sender);	//  called by explorer to open texture
	void OnMtrlAdd(sx::gui::PControl Sender);		//	add new material and open shader
	void OnMtrlRem(sx::gui::PControl Sender);		//	remove selected material except the last remain
	void OnMtrlSet(sx::gui::PControl Sender);		//	set a new shader to selected material
	void OnMtrlClick(sx::gui::PControl Sender);		//  set active material of mesh to selected material

	void OnParamChange(sx::gui::PControl Sender);	//  apply changes of an parameter to material
	void OnMouseWheel(sx::gui::PControl Sender);
	void OnRSChanged(sx::gui::PControl Sender);		//  will called when each of render states has been changed

	void OnClick(sx::gui::PControl sender);

private:
	float2	m_Size;

	Array<sx::gui::Control*>	
							m_arrayGUI;			//  hold created gui for edit shader

	sx::gui::PPanel			m_pBack;			//  back ground of editor. just for origin
	sx::gui::PLabel			m_pMtrlListTitle;	//  title of material list
	sx::gui::ListBox*		m_pMtrlList;		//	list of materials
	sx::gui::PButton		m_pMtrlAdd;			//  add a material to list
	sx::gui::PButton		m_pMtrlRemove;		//  remove a material from list
	sx::gui::Button*		m_copy;				//	copy material
	sx::gui::Button*		m_paste;			//	past material
	sx::gui::Button*		m_pasteNew;			//	past to new material
	sx::gui::PButton		m_pSetShader;		//  open shader a file from explorer
	sx::gui::PPanel			m_pSplitter;		//  just a simple panel to split form

	sx::gui::PCheckBox		m_pAlphaBlend;		//  check box to turn alpha blending
	sx::gui::PCheckBox		m_pAlphaAdd;		//  check box to turn alpha add
	sx::gui::PCheckBox		m_pAlphaMul;		//  check box to turn alpha multiply
	sx::gui::PCheckBox		m_pCulling;			//  check box to turn culling
	sx::gui::PCheckBox		m_pZEnable;			//  check box to turn z enable
	sx::gui::PCheckBox		m_pZWriting;		//  check box to turn z writing

	sx::core::PMaterialMan	m_material;			//  material to edit

	GUICallbackFuncPtr		m_OnResize;			//  will called when size of material editor changed

};
typedef EditMaterial *PEditMaterial;

#endif	//	GUARD_EditMaterial_HEADER_FILE

