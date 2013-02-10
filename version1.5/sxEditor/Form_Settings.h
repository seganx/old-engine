/********************************************************************
	created:	2011/06/12
	filename: 	Form_Settings.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple form for edit settings of 
				editor and game
*********************************************************************/
#ifndef GUARD_Form_Settings_HEADER_FILE
#define GUARD_Form_Settings_HEADER_FILE


#include "BaseForm.h"
#include "sxListBox.h"
#include "ColorBox.h"

/*!
	setting forms get you to access and change some settings of editor and game
*/
class Form_Settings: public BaseForm
{

public:
	Form_Settings(void);
	virtual ~Form_Settings(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

	//! show
	void Show(void);

private:

	void OnColorClick(sx::gui::PControl Sender);
	void OnShadowSizeChange(sx::gui::PControl Sender);
	void OnTextureLODChange(sx::gui::PControl Sender);
	void OnShaderQualityChange(sx::gui::PControl Sender);
	void OnFogChange(sx::gui::PControl Sender);
	void OnButtonClick(sx::gui::PControl Sender);


	sx::gui::PLabel			m_pTitle;				// Title of the form
	
	sx::gui::PPanel			m_pSunLightPanel;		//  holds sun light properties objects
	sx::gui::PTrackBar		m_pSunTheta;			//  track bar to change sun light position
	sx::gui::PTrackBar		m_pSunPhi;				//  track bar to change sun light position
	sx::gui::PTextEdit		m_pSunDiffuse[4];		//  sun light diffuse color
	sx::gui::PTextEdit		m_pSunAmbient[4];		//  sun light ambient color
	sx::gui::PListBox		m_pShadowLevel;			//  level of shadow quality
	sx::gui::PListBox		m_pShadowSize;			//  size of shadow map

	sx::gui::PListBox		m_pTextureLOD;			//  LOD of textures in the scene
	sx::gui::PListBox		m_pShaderQuality;		//  quality of shaders in the editor
	sx::gui::PListBox		m_reflectQuality;		//  quality of reflections in the editor

	sx::gui::PListBox		m_fog_mode;				//	fog mode
	sx::gui::PTrackBar		m_fog_dens;				//	density of the fog
	sx::gui::PTrackBar		m_fog_start;			//  start plan of fog
	sx::gui::PTrackBar		m_fog_end;				//  end plane of fog
	sx::gui::PPanel			m_fog_color;			//  display color of the fog

	sx::gui::PButton		m_CleanProject;			//  clean project directory

	Colorbox				m_ColorBox;				//  color box for change fog color

};
typedef Form_Settings *PForm_Settings;

#endif	//	GUARD_Form_Settings_HEADER_FILE
