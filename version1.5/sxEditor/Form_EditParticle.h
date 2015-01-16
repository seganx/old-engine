/********************************************************************
	created:	2011/06/16
	filename: 	Form_EditParticle.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a form to edit properties of particle
*********************************************************************/
#ifndef GUARD_Form_EditParticle_HEADER_FILE
#define GUARD_Form_EditParticle_HEADER_FILE

#include "BaseForm.h"
#include "EditMaterial.h"

/*!
use to edit particles
*/
class Form_EditParticle : public BaseForm
{

public:
	Form_EditParticle(void);
	virtual ~Form_EditParticle(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

	//! assign particle to edit. call it frequently in loop to check changes.
	void SetParticleToEdit(sx::core::PNodeMember particle);

private:
	void OnMaterialResized(sx::gui::PControl Sender);	//  apply change size of material editor
	void OnParamChange(sx::gui::PControl Sender);		//  apply changes of an parameter to material
	void OnMouseWheel(sx::gui::PControl Sender);		//  to apply mouse wheel
	void OnCheckBoxClick(sx::gui::PControl Sender);		//  for spray and loop

private:
	sx::gui::PLabel			m_pTitle;			//	title of the form

	sx::gui::PCheckBox		m_ch_Spray;			//  check box for spray particles
	sx::gui::PCheckBox		m_ch_Loop;			//  to check continue spraying
	sx::gui::PCheckBox		m_ch_lock_Y;		//  lock particles in y axis
	sx::gui::PCheckBox		m_ch_lock_XZ;		//  lock particles in XZ plane
	sx::gui::PCheckBox		m_ch_lock_XY;		//  lock particles in XY plane
	sx::gui::PCheckBox		m_ch_lock_YZ;		//  lock particles in YZ plane
	sx::gui::PCheckBox		m_ch_reflect;		//	draw this to reflection
	sx::gui::PCheckBox		m_ch_rndTime;		//	use random function in delay time

	sx::gui::TextEdit*		m_flow;				//  use to change particle flow
	sx::gui::TextEdit*		m_quadCount;		//  use to change quad count
	sx::gui::TextEdit*		m_lifeTime;			//  use to change life time
	sx::gui::TextEdit*		m_angleBase;		//  angle base
	sx::gui::TextEdit*		m_angleSpeed;		//  change angle speed
	sx::gui::TextEdit*		m_scaleBaseX;		//	X scale base 
	sx::gui::TextEdit*		m_scaleBaseY;		//	Y scale base 
	sx::gui::TextEdit*		m_scaleSpeed;		//	scale speed
	sx::gui::TextEdit*		m_moveDepend;		//	movement independence
	sx::gui::TextEdit*		m_delayTime;		//	spray delay time
	sx::gui::TextEdit*		m_rndParam1;		//	a random parameter is useful in some effects

	sx::gui::PLabel			m_lbl_speed;		//  label of move speed
	sx::gui::PLabel			m_lbl_range;		//  label of init range
	sx::gui::PLabel			m_lbl_gravity;		//  label of gravity force

	sx::gui::TextEdit*		m_moveSpeed[3];		//  move speed
	sx::gui::TextEdit*		m_initRange[3];		//  initial range
	sx::gui::TextEdit*		m_gravity[3];		//  gravity

	EditMaterial			m_EditMaterial;		//  material editor

	sx::core::PParticle		m_particle;			//  particle to edit

	// additional variable
	bool m_applyChanges;
};
typedef Form_EditParticle *PForm_EditParticle;

#endif	//	GUARD_Form_EditParticle_HEADER_FILE

