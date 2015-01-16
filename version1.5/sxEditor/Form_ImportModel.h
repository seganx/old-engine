/********************************************************************
	created:	2012/02/06
	filename: 	Form_ImportOBJ.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple form to import obj file
*********************************************************************/
#ifndef GUARD_Form_ImportOBJ_HEADER_FILE
#define GUARD_Form_ImportOBJ_HEADER_FILE


#include "BaseForm.h"
#include "ImportModel.h"

/*!
use to import sound
*/
class Form_ImportModel: public BaseForm
{

public:
	Form_ImportModel(void);
	virtual ~Form_ImportModel(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! call to import file
	void Import(const WCHAR* FileName, sx::gui::PForm pForm, GUICallbackEvent OnOkClick, DWORD Tag);

	//!	return pointer to the option structure
	ImportModelOptions * GetOptions(DWORD flag = 0);

	//! return file name to import
	const WCHAR* GetFileName(void);

private:

	void OnParamChange(sx::gui::PControl Sender);	//  apply changes of an parameters
	void OnSetShader(sx::gui::PControl Sender);		//  apply new shader

private:

	sx::gui::PLabel			m_pTitle;			//	title of the form

	sx::gui::PCheckBox		m_ch_Textures;		//	force to overwrite all texture
	sx::gui::PCheckBox		m_ch_renormal;		//	force to recompute normals
	sx::gui::PCheckBox		m_ch_centerPivot;	//	move pivot to center
	sx::gui::PCheckBox		m_ch_shCast;		//	meshes can cast shadow
	sx::gui::PCheckBox		m_ch_shReceive;		//	meshes can receive shadow
	sx::gui::PCheckBox		m_ch_invisible;		//	invisible all meshes

	sx::gui::TextEdit*		m_td_prefix;		//	prefix of resources

	sx::gui::PTrackBar		m_tr_lodFirst;		//  precision of first LOD
	sx::gui::PTrackBar		m_tr_lodSecond;		//  precision of second LOD

	sx::gui::PButton		m_btn_Shader;		//	shader to use in import
	
	sx::gui::PButton		m_btn_ok;			//  OK button
	sx::gui::PButton		m_btn_Cancel;		//  Cancel button

	String					m_fileName;			//  OBJ filename to import
	GUICallbackFuncPtr		m_OnOkClick;		//  call loading function

	ImportModelOptions		m_ImportOption;		//  import options
};
typedef Form_ImportModel *PForm_ImportModel;

#endif	//	GUARD_Form_ImportOBJ_HEADER_FILE
