/********************************************************************
	created:	2011/07/07
	filename: 	EditorCamera.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple camera for use in editor
*********************************************************************/
#ifndef GUARD_EditorCamera_HEADER_FILE
#define GUARD_EditorCamera_HEADER_FILE

#include "MainEditor.h"

class EditorCamera
{

public:
	EditorCamera(void);
	~EditorCamera(void);

	//! set current camera mode
	void SetMode(EditorCameraMode mode);

	//! update camera
	void Update_multi(float elpsTime, bool& inputHandled, sx::core::ArrayPNode& selectedNodes, sx::core::PNodeMember selectedMember);

	//! update camera
	void Update(float elpsTime, bool& inputHandled, sx::core::PNode selectedNode, sx::core::PNodeMember selectedMember);

	//! reset the camera
	void Reset(void);

	//! reference to the engine camera
	sx::core::Camera& operator() (void);

	//! reload camera properties from engine's camera
	void Reload(void);

private:

	//! update camera as RTS
	void Update_RTS(float elpTime, bool& inputHandled);

	//! update camera as spherical
	void Update_SPH(float elpTime, bool& inputHandled);

	sx::core::Camera	m_cam;		//  camera in engine
	sx::core::Camera	m_camRTS;	//  camera for RTS mode


	// additional parameters
	EditorCameraMode	m_Mode;
	float				m_Raduis;
	float				m_Phase;
	float				m_Theta;

};
typedef EditorCamera *PEditorCamera;

#endif	//	GUARD_EditorCamera_HEADER_FILE

