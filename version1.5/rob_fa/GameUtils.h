/********************************************************************
	created:	2011/11/04
	filename: 	GameUtils.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some tools for implementing game play
*********************************************************************/
#ifndef GUARD_GameUtils_HEADER_FILE
#define GUARD_GameUtils_HEADER_FILE

#include "ImportEngine.h"

struct GameString;
class Entity;

namespace GU
{

	class Camera
	{
	public:
		Camera(void);
		virtual ~Camera(void);

		virtual void Initialize(void);

		virtual void ProseccInput(bool& inputHandled, float elpsTime) = 0;

		virtual void Update(float elpsTime) = 0;

	public:
		sx::core::Camera	m_Camera;
	};


	class Camera_RTS : public Camera
	{
	public:

		Camera_RTS(void);

		virtual ~Camera_RTS(void);

		virtual void ProseccInput(bool& inputHandled, float elpsTime);

		virtual void Update(float elpsTime);

		void ProcInput(bool& inputHandled, float elpsTime);

		void SetPos(float x, float z);

		void SetArea(float minX, float maxX, float minZ, float maxZ);

		void SetLimit_Phi(float minPhi, float maxPhi);

		void Reload(void);

	public:

		bool	m_Activate;

		float	m_Rad;
		float	m_maxRad;
		float	m_minRad;

		float	m_Phi;
		float	m_minPhi;
		float	m_maxPhi;
		float	m_phiThreshold;

		float	m_Tht;

		float	m_minX;
		float	m_maxX;
		float	m_minZ;
		float	m_maxZ;

		float	m_shaking;
		bool	m_freeMode;

	}; // Camera_RTS

	class Camera_Mobile : public Camera
	{
	public:

		Camera_Mobile(void);

		virtual ~Camera_Mobile(void);

		virtual void ProseccInput(bool& inputHandled, float elpsTime);

		virtual void Update(float elpsTime);

		void Attach(Entity* enitity);

	public:

		bool				m_Activate;
		float				m_blendSpeed;
		Entity*				m_Entity;
		sx::core::PNode		m_nodeCamera;


	}; // Camera_Tower

} // namespace GU


class GuideText
{
public:
	GuideText(void): m_fresh(true){}
	const uint Use(void)
	{
		m_fresh = false;
		return m_text;
	}
public:
	bool	m_fresh;
	uint	m_text;
};


sx::gui::Label* create_label( sx::gui::Control* parent, const wchar* font, const GUITextAlign align, const wchar* caption, const float& width, const float& height, const float& x, const float& y, const float& z );
sx::gui::Label* create_label( sx::gui::Control* parent, const uint text, const float& width, const float& height, const float& x, const float& y, const float& z );
sx::gui::Label* update_label( sx::gui::Label* label, const uint text );
sx::gui::Button* create_back_button( sx::gui::Control* parent, const float& x, const float& y );


#endif	//	GUARD_GameUtils_HEADER_FILE