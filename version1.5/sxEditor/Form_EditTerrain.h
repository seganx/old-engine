/********************************************************************
	created:	2011/09/06
	filename: 	Form_EditTerrain.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of form to edit terrain
*********************************************************************/
#ifndef GUARD_Form_EditTerrain_HEADER_FILE
#define GUARD_Form_EditTerrain_HEADER_FILE


#include "BaseForm.h"
#include "EditMaterial.h"

class UndoManager;

/*!
use to edit terrain nodes
*/
class Form_EditTerrain: public BaseForm
{

public:
	Form_EditTerrain(void);
	virtual ~Form_EditTerrain(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! draw the form
	void Draw(DWORD flag);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

	//! this can be called frequently in suit place to do some operation.
	void Operate(bool& inputHandled);

	//! show th window
	void Show(void);

private:
	void OnMaterialResized(sx::gui::PControl Sender);	//  apply change size of material editor
	void OnParamChange(sx::gui::PControl Sender);		//  apply changes of an parameters
	bool CreateNode(Ray& ray);
	bool DeleteNode(Ray& ray);

private:

	sx::gui::PLabel			m_pTitle;			//	title of the form
	sx::gui::PButton		m_btnCreate;		//  use to create terrain
	sx::gui::PButton		m_btnDelete;		//  use to delete terrain

	EditMaterial			m_EditMaterial;		//  material editor

	sx::gui::PLabel			m_lbLOD;			//  use to show LOD threshold
	sx::gui::PTrackBar		m_trLOD;			//  use to change LOD threshold
	
	sx::gui::PButton		m_btnFinish;		//  use to finish job

	//! additional parameters
	DWORD					m_Mode;
	int						m_heightPlus;		//  additional height

};
typedef Form_EditTerrain *PForm_EditTerrain;

#endif	//	GUARD_Form_EditTerrain_HEADER_FILE
