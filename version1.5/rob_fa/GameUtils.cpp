#include "GameUtils.h"
#include "ImportEngine.h"
#include "Entity.h"
#include "Game.h"


#define CAMERA_FAR	6000.0f

namespace GU
{

	//////////////////////////////////////////////////////////////////////////
	//	ABSTRACT CAMERA CLASS
	//////////////////////////////////////////////////////////////////////////
	Camera::Camera( void )
	{

	}

	Camera::~Camera( void )
	{

	}

	void Camera::Initialize( void )
	{
		m_Camera = *( sx::core::Renderer::GetCamera() );
	}

	//////////////////////////////////////////////////////////////////////////
	//	CAMERA RTS
	//////////////////////////////////////////////////////////////////////////
	Camera_RTS::Camera_RTS( void ) 
		: Camera()
		, m_Activate(false)
		, m_Rad(10)
		, m_maxRad(60)
		, m_minRad(20)
		, m_Phi(1)
		, m_minPhi(0)
		, m_maxPhi(0)
		, m_phiThreshold(0)
		, m_Tht(0.7f)
		, m_minX(-500)
		, m_maxX(500)
		, m_minZ(-500)
		, m_maxZ(500)
		, m_shaking(0)
		, m_freeMode(false)
	{

	}

	Camera_RTS::~Camera_RTS( void )
	{

	}

	void Camera_RTS::ProseccInput( bool& inputHandled, float elpsTime )
	{
		if ( g_game->m_game_paused || !m_Activate ) return;
		
		sx_callstack();

#if 0
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYUP(0, SX_INPUT_KEY_F) )
		{
			m_freeMode = !m_freeMode;

			if ( g_game->m_mouseMode == MS_FreeCamera )
				g_game->m_mouseMode = MS_Null;
			else
				g_game->m_mouseMode = MS_FreeCamera;
		}

		if ( m_freeMode )
		{
			ProcInput( inputHandled, elpsTime );
			return;
		}
#endif

		float3 dir = m_Camera.GetDirection(), left;
		dir.y=0; dir.Normalize( dir );
		left.Cross( dir, m_Camera.Up );
		left.Normalize( left );

		m_Rad -= m_Rad * sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL] * 0.1f;

		float speed_kbrd = m_Rad * elpsTime * 0.003f;

		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_W) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_UP)		)	{	m_Camera.At += dir * speed_kbrd;	/*inputHandled = true;*/ }
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_S) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_DOWN)	)	{	m_Camera.At -= dir * speed_kbrd;	/*inputHandled = true;*/ }
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_A) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_LEFT)	)	{	m_Camera.At += left * speed_kbrd;	/*inputHandled = true;*/ }
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_D) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RIGHT)	)	{	m_Camera.At -= left * speed_kbrd;	/*inputHandled = true;*/ }
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_Q) )	{	m_Phi -= elpsTime * 0.001f;			/*inputHandled = true;*/ }
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_E) )	{	m_Phi += elpsTime * 0.001f;			/*inputHandled = true;*/ }

		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_MIDDLE) )
		{
			m_Phi += SEGAN_MOUSE_RLX(0) * elpsTime * 0.0001f;
			SEGAN_MOUSE_ABSX(0) -= SEGAN_MOUSE_RLX(0);
			SEGAN_MOUSE_ABSY(0) -= SEGAN_MOUSE_RLY(0);
		}

		float speed_mous = speed_kbrd * 0.1f;

		float f = 10.0f;
		float w = SEGAN_VP_WIDTH-f, h = SEGAN_VP_HEIGHT-f;
		if ( SEGAN_MOUSE_ABSY(0)<f )	{	m_Camera.At += dir * speed_mous * ( f - SEGAN_MOUSE_ABSY(0) );	 }
		if ( SEGAN_MOUSE_ABSY(0)>h )	{	m_Camera.At -= dir * speed_mous * ( SEGAN_MOUSE_ABSY(0) - h );	 }
		if ( SEGAN_MOUSE_ABSX(0)<f )	{	m_Camera.At += left * speed_mous * ( f - SEGAN_MOUSE_ABSX(0) );	 }
		if ( SEGAN_MOUSE_ABSX(0)>w )	{	m_Camera.At -= left * speed_mous * ( SEGAN_MOUSE_ABSX(0) - w );	 }
	}

	void Camera_RTS::Update( float elpsTime )
	{
		if ( !m_Activate ) return;

		sx_callstack();

		float theta;
		if ( m_freeMode )
		{
			theta = m_Tht;
		}
		else
		{
			SEGAN_CLAMP( m_Rad, m_minRad, m_maxRad );
			float coefficient = ( ( m_Rad - m_minRad ) / ( m_maxRad - m_minRad ) );
			float maxX = m_maxX - 30.0f * coefficient;
			float minX = m_minX + 30.0f * coefficient;
			float maxZ = m_maxZ - 30.0f * coefficient;
			float minZ = m_minZ + 30.0f * coefficient;
			SEGAN_CLAMP( m_Camera.At.x, minX, maxX);
			SEGAN_CLAMP( m_Camera.At.z, minZ, maxZ);

			float thetaValue = ( m_Rad - m_minRad ) / ( m_maxRad - m_minRad );
			theta = m_Tht * thetaValue + 0.95f * (1.0f - thetaValue);

			if ( m_minPhi && m_maxPhi )
				SEGAN_CLAMP(m_Phi, m_minPhi, m_maxPhi);
		}


		m_Camera.Far = CAMERA_FAR;
		m_Camera.Aspect = SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;
		m_Camera.FOV = PI / (1.7f + m_Camera.Aspect);
		m_Camera.SetSpherical(m_Rad, m_Phi, theta);

#if 1
		static float camheight = 0.0f;
		static int updatetimes = 0;
		updatetimes++;
		if ( updatetimes > 5 )
		{
			updatetimes = 0;

			msg_IntersectRay msgRay( 
				MT_MESH, 
				Ray( float3(m_Camera.Eye.x, 100.0f, m_Camera.Eye.z), float3(0.001f, -0.999f, 0.001f) ),
				msg_IntersectRay::GEOMETRY,
				L"cameraGuard"
				);
			sx::core::Scene::GetNodeByRay( msgRay );

			if ( msgRay.results->member )
				camheight = msgRay.results->position.y;
			else
				camheight = 0;
		}
		if ( camheight > m_Camera.Eye.y )
			m_Camera.Eye.y = camheight + 5.0f;
#endif

		// calculate camera shaking
		float3 shaking( 0, 0, 0 );
		if ( m_shaking > 0.01f )
		{
			const float zoom = sx_sqrt( m_Rad ) * 0.07f;
			shaking.x = ( sx::cmn::Random( 1.0f ) - sx::cmn::Random( 1.0f ) ) * zoom;
			shaking.y = ( sx::cmn::Random( 1.0f ) - sx::cmn::Random( 1.0f ) ) * zoom;
			shaking.z = ( sx::cmn::Random( 1.0f ) - sx::cmn::Random( 1.0f ) ) * zoom;
			m_shaking -= elpsTime * 0.001f;
		}

		sx::core::PCamera pCam = sx::core::Renderer::GetCamera();

		pCam->Far = m_Camera.Far;
		pCam->FOV += (m_Camera.FOV - pCam->FOV) * 0.01f * elpsTime;
		pCam->Aspect = m_Camera.Aspect;

		pCam->At.x += (m_Camera.At.x - pCam->At.x) * 0.02f * elpsTime + shaking.x;
		pCam->At.y += (m_Camera.At.y - pCam->At.y) * 0.02f * elpsTime + shaking.y;
		pCam->At.z += (m_Camera.At.z - pCam->At.z) * 0.02f * elpsTime + shaking.z;

		pCam->Eye.x += (m_Camera.Eye.x - pCam->Eye.x) * 0.015f * elpsTime + shaking.x;
		pCam->Eye.y += (m_Camera.Eye.y - pCam->Eye.y) * 0.015f * elpsTime + shaking.y;
		pCam->Eye.z += (m_Camera.Eye.z - pCam->Eye.z) * 0.015f * elpsTime + shaking.z;

		sx::core::Renderer::SetCamera( pCam );
	}

	void Camera_RTS::SetPos( float x, float z )
	{
		m_Camera.At.x = x;
		m_Camera.At.z = z;
	}

	void Camera_RTS::SetArea( float minX, float maxX, float minZ, float maxZ )
	{
		m_minX = minX;
		m_maxX = maxX;
		m_minZ = minZ;
		m_maxZ = maxZ;
	}

	void Camera_RTS::Reload( void )
	{
		m_Camera = *sx::core::Renderer::GetCamera();
		m_Camera.GetSpherical( &m_Rad, &m_Phi, &m_Tht );
		m_maxRad = m_Rad;
		m_Rad /= 1.5f;
//		m_minRad = 20.0f;		

		m_minPhi = m_Phi - m_phiThreshold;
		m_maxPhi = m_Phi + m_phiThreshold;

		m_shaking = 0.0f;
	}

	void Camera_RTS::SetLimit_Phi( float minPhi, float maxPhi )
	{
		m_minPhi = minPhi;
		m_maxPhi = maxPhi;
	}

	void Camera_RTS::ProcInput( bool& inputHandled, float elpsTime )
	{
		//  collect input data
		const char* keys = sx::io::Input::GetKeys(0);
		bool g_keyhold_Shift			= SEGAN_KEYHOLD(0, SX_INPUT_KEY_LSHIFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RSHIFT);
		bool g_keyhold_Crlt				= SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL);
		bool g_keyhold_mouseLeft		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_LEFT);
		bool g_keyhold_mouseRight		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_RIGHT);
		bool g_keyhold_mouseMidd		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_MIDDLE);
		bool g_keyhold_move_forward		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_W);
		bool g_keyhold_move_backward	= SEGAN_KEYHOLD(0, SX_INPUT_KEY_S);
		bool g_keyhold_move_left		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_A);
		bool g_keyhold_move_right		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_D);

		//  move camera
		if ( 
			!g_keyhold_Shift && !g_keyhold_Crlt && 
			(g_keyhold_move_forward || g_keyhold_move_backward || g_keyhold_move_left || g_keyhold_move_right)
			)
		{
			float3 dir, left;
			dir.Normalize( m_Camera.GetDirection() );
			left.Cross( dir, m_Camera.Up );

			if ( g_keyhold_move_forward || g_keyhold_move_backward || g_keyhold_move_left || g_keyhold_move_right )
			{
				if ( g_keyhold_move_forward )	m_Camera.At += dir * elpsTime * 0.0025f;
				if ( g_keyhold_move_backward )	m_Camera.At -= dir * elpsTime * 0.0025f;
				if ( g_keyhold_move_left )		m_Camera.At += left * elpsTime * 0.0025f;
				if ( g_keyhold_move_right )		m_Camera.At -= left * elpsTime * 0.0025f;

				inputHandled = true;
			}
		}

		//  zoom camera
		m_Rad -= keys[SX_INPUT_KEY_MOUSE_WHEEL] * m_Rad * 0.05f;

		//  pan camera
		if ( g_keyhold_mouseMidd || (g_keyhold_Crlt && g_keyhold_mouseRight) )
		{
			Matrix matView;
			m_Camera.GetViewMatrix( matView );
			matView.Inverse( matView );

			float3 vecView( -SEGAN_MOUSE_RLX(0)/SEGAN_VP_HEIGHT, SEGAN_MOUSE_RLY(0)/SEGAN_VP_HEIGHT, 0.0f );
			vecView.Transform_Norm( vecView, matView );
			m_Camera.At += vecView * m_Rad;

			inputHandled = true;
		}

		//  rotate camera
		if ( (g_keyhold_Crlt || g_keyhold_move_forward || g_keyhold_move_backward || g_keyhold_move_left || g_keyhold_move_right) && g_keyhold_mouseLeft )
		{
			m_Phi -= SEGAN_MOUSE_RLX(0) * 0.01f;
			m_Tht -= SEGAN_MOUSE_RLY(0) * 0.01f;
			SEGAN_CLAMP(m_Tht, 0.01f, 3.1f);

			inputHandled = true;
		}

		m_Camera.SetSpherical(m_Rad, m_Phi, m_Tht);
	}


	//////////////////////////////////////////////////////////////////////////
	//	CAMERA TOWER
	//////////////////////////////////////////////////////////////////////////
	Camera_Mobile::Camera_Mobile( void )
		: Camera()
		, m_Activate(false)
		, m_blendSpeed(1)
		, m_Entity(0)
		, m_nodeCamera(0)
	{

	}

	Camera_Mobile::~Camera_Mobile( void )
	{

	}

	void Camera_Mobile::ProseccInput( bool& inputHandled, float elpsTime )
	{
		if ( !m_nodeCamera ) m_Activate = false;
	}

	void Camera_Mobile::Update( float elpsTime )
	{
		sx_callstack();

		if ( !m_nodeCamera ) m_Activate = false;
		if ( !m_Activate )
		{
			if ( g_game->m_mouseMode == MS_MobileCamera )
				g_game->m_mouseMode = MS_Null;
			return;
		}

		if ( g_game->m_mouseMode != MS_Null && g_game->m_mouseMode != MS_MobileCamera )
		{
			m_Activate = false;
			return;
		}

		m_Camera.Far = CAMERA_FAR;
		m_Camera.Aspect = SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;
		m_Camera.FOV = PI / (1.7f + m_Camera.Aspect);
		m_Camera.Eye = m_nodeCamera->GetPosition_world();
		m_Camera.At.Transform_Norm( float3(0, 0, 1), m_nodeCamera->GetMatrix_world() );
		m_Camera.At = m_Camera.Eye + m_Camera.At;
		m_Camera.Up.Set(0, 1, 0);

		if ( m_Entity && m_Entity->m_health.icur < 1 )
			m_Camera.Eye.y = 7.0f;

		sx::core::PCamera pCam = sx::core::Renderer::GetCamera();

		pCam->Far = m_Camera.Far;
		pCam->FOV += (m_Camera.FOV - pCam->FOV) * 0.01f * elpsTime;
		pCam->Aspect = m_Camera.Aspect;

#if 1
		float bspeed = m_blendSpeed * 0.01f;
		pCam->At.x += (m_Camera.At.x - pCam->At.x) * bspeed * elpsTime;
		pCam->At.y += (m_Camera.At.y - pCam->At.y) * bspeed * elpsTime;
		pCam->At.z += (m_Camera.At.z - pCam->At.z) * bspeed * elpsTime;

		bspeed = m_blendSpeed * 0.005f;
		pCam->Eye.x += (m_Camera.Eye.x - pCam->Eye.x) * bspeed * elpsTime;
		pCam->Eye.y += (m_Camera.Eye.y - pCam->Eye.y) * bspeed * elpsTime;
		pCam->Eye.z += (m_Camera.Eye.z - pCam->Eye.z) * bspeed * elpsTime;
#else
		//	TEST	
		pCam->Eye = m_Camera.Eye;
		pCam->At = m_Camera.At;
#endif

		sx::core::Renderer::SetCamera( pCam );

		if ( g_game->m_mouseMode == MS_Null )
			g_game->m_mouseMode = MS_MobileCamera;

	}

	void Camera_Mobile::Attach( Entity* entity )
	{
		if ( entity && entity->m_initialized && entity->m_health.deleteTime > 2 )
		{
			if ( m_Entity != entity )
			{
				m_Entity = entity;
				m_blendSpeed = entity->m_partyCurrent == PARTY_TOWER ? 1 : ( PARTY_ALLY ? 0.5f : 2 );
				entity->m_node->GetChildByName(L"camera", m_nodeCamera);
			}
		}
		else
		{
			m_nodeCamera = NULL;
			m_Entity = NULL;
		}

		if ( !m_nodeCamera )
			m_Activate = false;
	}
} // namespace GU

sx::gui::Label* create_label( sx::gui::Control* parent, const wchar* font, const GUITextAlign align, const wchar* caption, const float& width, const float& height, const float& x, const float& y, const float& z )
{
	sx::gui::Label* res =  sx_new( sx::gui::Label );
	res->SetParent( parent );
	res->SetSize( float2(width, height) );
	res->SetAlign( align );
	res->GetElement(0)->Color().a = 0.0f;
	res->GetElement(1)->Color() = 0xffffffff;
	res->SetFont( font );
	res->AddProperty( SX_GUI_PROPERTY_MULTILINE );
	res->AddProperty( SX_GUI_PROPERTY_WORDWRAP );
	res->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
	res->RemProperty( SX_GUI_PROPERTY_PIXELALIGN );
	res->Position().Set(x, y, z);
	res->SetText(caption);
	return res;
}

sx::gui::Label* create_label( sx::gui::Control* parent, const GameString* gameString, const float& width, const float& height, const float& x, const float& y, const float& z )
{
	sx::gui::Label* res = create_label( parent, gameString->font, gameString->align, gameString->text, width, height, x, y, z );
	res->PositionOffset().Set( gameString->x, gameString->y, 0.0f );
}

sx::gui::Label* update_label( sx::gui::Label* label, const GameString* gameString )
{
	label->SetAlign( gameString->align );
	label->SetFont( gameString->font );
	label->PositionOffset().Set( gameString->x, gameString->y, 0.0f );
	label->SetText( gameString->text );
}
