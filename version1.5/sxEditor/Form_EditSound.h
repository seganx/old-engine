/********************************************************************
	created:	2012/01/31
	filename: 	Form_EditSound.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of form to edit sound
*********************************************************************/
#ifndef GUARD_Form_EditSound_HEADER_FILE
#define GUARD_Form_EditSound_HEADER_FILE


#include "BaseForm.h"
#include "sxListBox.h"

/*!
use to edit sound
*/
class Form_EditSound: public BaseForm
{

public:
	Form_EditSound(void);
	virtual ~Form_EditSound(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! draw the form
	void Draw(DWORD flag);

	//! assign sound node to edit. call it frequently in loop to check changes.
	void SetSoundToEdit(sx::core::PNodeMember sound);

private:

	void OnParamChange(sx::gui::PControl Sender);		//  apply changes of an parameters
	void OnMouseWheel(sx::gui::PControl Sender);		//  apply changes of an parameters
	void ShowSoundInfo(bool applyToEdit);
	void LoadSound(sx::gui::PControl Sender);
	void OnListChange( sx::gui::PControl Sander );		//  list changed
	void ReloadList( void );							//	reload list

private:

	sx::gui::PLabel			m_pTitle;		//	title of the form

	sx::gui::PLabel			m_lbl_desc;		//  display some simple description

	sx::gui::PCheckBox		m_ch_is3D;		//	manage sound 3d flag
	sx::gui::PCheckBox		m_ch_loop;		//	manage sound loop flag
	sx::gui::PCheckBox		m_ch_onload;	//	play sound on load
	sx::gui::TextEdit*		m_tx_freq;		//  frequency of sound
	sx::gui::TextEdit*		m_tx_volm;		//  sound volume
	sx::gui::PTrackBar		m_tr_pan;		//  pan the sound to the left / right
	sx::gui::TextEdit*		m_tx_mindis;	//  minimum distance of the sound
	sx::gui::TextEdit*		m_tx_maxdis;	//  maximum distance of the sound
	sx::gui::TextEdit*		m_tx_repeat;	//  repeat time for loop sound
	sx::gui::TextEdit*		m_tx_chance;	//  chance to play sound

	sx::gui::ListBox*		m_sndList;
	sx::gui::PButton		m_sndAdd;
	sx::gui::PButton		m_sndRem;
	sx::gui::PButton		m_moveUp;
	sx::gui::PButton		m_moveDown;

	sx::gui::PPanel			m_splitter;		//  just a simple panel to split form
	sx::gui::PTrackBar		m_tr_position;	//  position of sound
	sx::gui::PButton		m_btn_play;		//  play the sound
	sx::gui::PButton		m_btn_pause;	//  pause the sound
	sx::gui::PLabel			m_lbl_time;		//  display time of length/position

	sx::core::PSound		m_sound;		//	sound object to edit
	sx::snd::PSoundData		m_soundData;	//  sound data

};
typedef Form_EditSound *PForm_EditSound;

#endif	//	GUARD_Form_EditSound_HEADER_FILE
