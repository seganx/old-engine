#include "EditorCamera.h"


//////////////////////////////////////////////////////////////////////////
//	 SOME GLOBAL VARIABLES
bool g_keyhold_Shift			= false;
bool g_keyhold_Crlt				= false;
bool g_keyhold_mouseLeft		= false;
bool g_keyhold_mouseRight		= false;
bool g_keyhold_mouseMidd		= false;
bool g_keyhold_move_forward		= false;
bool g_keyhold_move_backward	= false;
bool g_keyhold_move_left		= false;
bool g_keyhold_move_right		= false;


EditorCamera::EditorCamera( void ): m_Mode(ECM_SPHERICAL)
{
	Reset();
}

EditorCamera::~EditorCamera( void )
{

}

void EditorCamera::SetMode( EditorCameraMode mode )
{
	m_Mode = mode;
}

void EditorCamera::Update_multi( float elpsTime, bool& inputHandled, sx::core::ArrayPNode& selectedNodes, sx::core::PNodeMember selectedMember )
{
	if (inputHandled) return;

	//  collect input data
	const char* keys		= sx::io::Input::GetKeys(0);
	g_keyhold_Shift			= SEGAN_KEYHOLD(0, SX_INPUT_KEY_LSHIFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RSHIFT);
	g_keyhold_Crlt			= SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL);
	g_keyhold_mouseLeft		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_LEFT);
	g_keyhold_mouseRight	= SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_RIGHT);
	g_keyhold_mouseMidd		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_MIDDLE);
	g_keyhold_move_forward	= SEGAN_KEYHOLD(0, SX_INPUT_KEY_W);
	g_keyhold_move_backward = SEGAN_KEYHOLD(0, SX_INPUT_KEY_S);
	g_keyhold_move_left		= SEGAN_KEYHOLD(0, SX_INPUT_KEY_A);
	g_keyhold_move_right	= SEGAN_KEYHOLD(0, SX_INPUT_KEY_D);

	//  zoom to fit
	if ( selectedNodes.Count() && g_keyhold_Shift && SEGAN_KEYDOWN(0, SX_INPUT_KEY_Z) )
	{
		Sphere cen( sx::math::VEC3_ZERO, 0 );

		for (int i=0; i<selectedNodes.Count(); i++)
			cen.center += selectedNodes[i]->GetPosition_world();
		cen.center /= (float)selectedNodes.Count();

		for (int i=0; i<selectedNodes.Count(); i++)
			 cen.Cover( selectedNodes[i]->GetSphere_world() );

		if ( selectedMember && selectedNodes.Count()<2 )
			selectedMember->MsgProc(MT_GETSPHERE_WORLD, &cen);

		m_cam.At = cen.center;
		m_Raduis = cen.radius < 1.0f ? 2.0f : cen.radius * 2.0f;

		inputHandled = true;
	}

	if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_L) )
	{
		switch (m_Mode)
		{
		case ECM_SPHERICAL:
			{
				m_Mode = ECM_RTS;
				m_camRTS = m_cam;
			}
			break;

		case ECM_RTS:
			{
				m_Mode = ECM_SPHERICAL;
				m_cam = m_camRTS;
			}
			break;
		}
	}

	switch (m_Mode)
	{
	case ECM_SPHERICAL:
		{
			Update_SPH(elpsTime, inputHandled);
		}
		break;

	case ECM_RTS:
		{
			Update_RTS(elpsTime, inputHandled);
		}
		break;
	}
	

}

void EditorCamera::Reset( void )
{
	m_Raduis = 15.0f;
	m_Phase = 0.7f;
	m_Theta =0.7f;
	m_cam.At = sx::math::VEC3_ZERO;
	m_cam.Up.Set(0.0f, 1.0f, 0.0f);

	m_cam.SetSpherical(m_Raduis, m_Phase, m_Theta);
	m_cam.Far = 6000.0f;
	m_cam.Aspect = SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;
	m_cam.FOV = PI / (1.7f + m_cam.Aspect);

	m_camRTS = m_cam;
	
}

sx::core::Camera& EditorCamera::operator()( void )
{
	return m_cam;
}

void EditorCamera::Update_RTS( float elpsTime, bool& inputHandled )
{
	if ( inputHandled ) return;

	float3 dir = m_camRTS.GetDirection(), left;
	dir.y=0; dir.Normalize( dir );
	left.Cross( dir, m_camRTS.Up );
	left.Normalize( left );

	m_Raduis -= m_Raduis * sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL] * 0.1f;

	float speed = m_Raduis * elpsTime * 0.001f;
	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_W) ) {	m_camRTS.At += dir * speed;			inputHandled = true; }
	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_S) )	{	m_camRTS.At -= dir * speed;			inputHandled = true; }
	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_A) )	{	m_camRTS.At += left * speed;		inputHandled = true; }
	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_D) )	{	m_camRTS.At -= left * speed;		inputHandled = true; }
	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_Q) )	{	m_Phase -= elpsTime * 0.001f;		inputHandled = true; }
	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_E) )	{	m_Phase += elpsTime * 0.001f;		inputHandled = true; }
	m_camRTS.At.y = 0.0f;

	m_camRTS.SetSpherical(m_Raduis, m_Phase, m_Theta);

	m_cam.At.x += (m_camRTS.At.x - m_cam.At.x) * 0.02f * elpsTime;
	m_cam.At.y += (m_camRTS.At.y - m_cam.At.y) * 0.02f * elpsTime;
	m_cam.At.z += (m_camRTS.At.z - m_cam.At.z) * 0.02f * elpsTime;

	m_cam.Eye.x += (m_camRTS.Eye.x - m_cam.Eye.x) * 0.007f * elpsTime;
	m_cam.Eye.y += (m_camRTS.Eye.y - m_cam.Eye.y) * 0.007f * elpsTime;
	m_cam.Eye.z += (m_camRTS.Eye.z - m_cam.Eye.z) * 0.007f * elpsTime;

}

void EditorCamera::Update_SPH( float elpsTime, bool& inputHandled )
{
	//  collect input data
	const char* keys = sx::io::Input::GetKeys(0);

	//  move camera
	if ( 
		!g_keyhold_Shift && !g_keyhold_Crlt && 
		(g_keyhold_move_forward || g_keyhold_move_backward || g_keyhold_move_left || g_keyhold_move_right)
		)
	{
		float3 dir, left;
		dir.Normalize( m_cam.GetDirection() );
		left.Cross( dir, m_cam.Up );

		if ( g_keyhold_move_forward || g_keyhold_move_backward || g_keyhold_move_left || g_keyhold_move_right )
		{
			if ( g_keyhold_move_forward )	m_cam.At += dir * elpsTime * 0.0025f;
			if ( g_keyhold_move_backward )	m_cam.At -= dir * elpsTime * 0.0025f;
			if ( g_keyhold_move_left )		m_cam.At += left * elpsTime * 0.0025f;
			if ( g_keyhold_move_right )		m_cam.At -= left * elpsTime * 0.0025f;

			inputHandled = true;
		}
	}

	//  zoom camera
	m_Raduis -= keys[SX_INPUT_KEY_MOUSE_WHEEL] * m_Raduis * 0.05f;

	//  pan camera
	if ( g_keyhold_mouseMidd || (g_keyhold_Crlt && g_keyhold_mouseRight) )
	{
		Matrix matView;
		m_cam.GetViewMatrix( matView );
		matView.Inverse( matView );

		float3 vecView( -SEGAN_MOUSE_RLX(0)/SEGAN_VP_HEIGHT, SEGAN_MOUSE_RLY(0)/SEGAN_VP_HEIGHT, 0.0f );
		vecView.Transform_Norm( vecView, matView );
		m_cam.At += vecView * m_Raduis;

		inputHandled = true;
	}

	//  rotate camera
	if ( (g_keyhold_Crlt || g_keyhold_move_forward || g_keyhold_move_backward || g_keyhold_move_left || g_keyhold_move_right) && g_keyhold_mouseLeft )
	{
		m_Phase -= SEGAN_MOUSE_RLX(0) * 0.01f;
		m_Theta -= SEGAN_MOUSE_RLY(0) * 0.01f;
		SEGAN_CLAMP(m_Theta, 0.01f, 3.1f);

		inputHandled = true;
	}

	m_cam.SetSpherical(m_Raduis, m_Phase, m_Theta);
}

void EditorCamera::Update( float elpsTime, bool& inputHandled, sx::core::PNode selectedNode, sx::core::PNodeMember selectedMember )
{
	sx::core::ArrayPNode nodes;
	if ( selectedNode )
		nodes.PushBack( selectedNode );
	Update_multi(elpsTime, inputHandled, nodes, selectedMember);
}

void EditorCamera::Reload( void )
{
	m_cam = *sx::core::Renderer::GetCamera();
	m_cam.GetSpherical( &m_Raduis, &m_Phase, &m_Theta );
}

