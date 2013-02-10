#include "sxCameraBlender.h"

class CameraBlender_internal
{
public:
	static float3	s_Eye;
	static float3	s_At;
	static float3	s_Up;
	static float	s_fov;
	static Matrix	s_matView;
	static Matrix	s_matProj;
	static Frustum	s_Frustum;
};
float3	CameraBlender_internal::s_Eye(0.0f, 0.0f, -10.0f);
float3	CameraBlender_internal::s_At(0.0f, 0.0f, 0.0f);
float3	CameraBlender_internal::s_Up(0.0f, 1.0f, 0.0f);
float	CameraBlender_internal::s_fov(PI / 3.0f);
Matrix	CameraBlender_internal::s_matView(sx::math::MTRX_IDENTICAL);
Matrix	CameraBlender_internal::s_matProj(sx::math::MTRX_IDENTICAL);
Frustum CameraBlender_internal::s_Frustum;


namespace sx { namespace core {

	void CameraBlender::Update( Camera& curCamera, float elpsTime )
	{
		//	cur += (goal - cur) * velocity * (elpsTime * 0.06f);
		//  cur = cur * amplitude;

		if ( curCamera.Velocity && curCamera.Amplitude )
		{
			//  blend camera

			//float a = (1 - w) * m_Vel;
			//if (abs(a)>0.0000001)
			//{
			//	v += a * (elpsTime * 0.06f);
			//	w = (w + v) * m_Amp;
			//}
			//else w = 1;

		}
		else
		{
			CameraBlender_internal::s_Eye	= curCamera.Eye;
			CameraBlender_internal::s_At	= curCamera.At;
			CameraBlender_internal::s_Up	= curCamera.Up;
			CameraBlender_internal::s_fov	= curCamera.FOV;
		}
		
		//  make view matrix
		CameraBlender_internal::s_matView.LookAtLH(
			CameraBlender_internal::s_Eye,
			CameraBlender_internal::s_At,
			CameraBlender_internal::s_Up);

		//  make projection matrix
		float Near = 0.0010f * curCamera.Far - 0.1f;
		float aspect = curCamera.Aspect>0 ? curCamera.Aspect : (float)sx::d3d::Device3D::Viewport()->Width / (float)sx::d3d::Device3D::Viewport()->Height;
		CameraBlender_internal::s_matProj.PerspectiveFovLH(CameraBlender_internal::s_fov, aspect, Near, curCamera.Far);

		//  Extract 5 planes around view frustum
		Matrix mat; mat.Multiply(CameraBlender_internal::s_matView, CameraBlender_internal::s_matProj);

		// Near clipping plane
		CameraBlender_internal::s_Frustum.p0.a	= mat._13;
		CameraBlender_internal::s_Frustum.p0.b	= mat._23;
		CameraBlender_internal::s_Frustum.p0.c	= mat._33;
		CameraBlender_internal::s_Frustum.p0.d	= mat._43;

		// Left clipping plane
		CameraBlender_internal::s_Frustum.p1.a	= mat._14 + mat._11;
		CameraBlender_internal::s_Frustum.p1.b	= mat._24 + mat._21;
		CameraBlender_internal::s_Frustum.p1.c	= mat._34 + mat._31;
		CameraBlender_internal::s_Frustum.p1.d	= mat._44 + mat._41;

		// Right clipping plane
		CameraBlender_internal::s_Frustum.p2.a	= mat._14 - mat._11;
		CameraBlender_internal::s_Frustum.p2.b	= mat._24 - mat._21;
		CameraBlender_internal::s_Frustum.p2.c	= mat._34 - mat._31;
		CameraBlender_internal::s_Frustum.p2.d	= mat._44 - mat._41;

		// Top clipping plane
		CameraBlender_internal::s_Frustum.p3.a	= mat._14 - mat._12;
		CameraBlender_internal::s_Frustum.p3.b	= mat._24 - mat._22;
		CameraBlender_internal::s_Frustum.p3.c	= mat._34 - mat._32;
		CameraBlender_internal::s_Frustum.p3.d	= mat._44 - mat._42;

		// Bottom clipping plane
		CameraBlender_internal::s_Frustum.p4.a	= mat._14 + mat._12;
		CameraBlender_internal::s_Frustum.p4.b	= mat._24 + mat._22;
		CameraBlender_internal::s_Frustum.p4.c	= mat._34 + mat._32;
		CameraBlender_internal::s_Frustum.p4.d	= mat._44 + mat._42;

		// Far clipping plane
		CameraBlender_internal::s_Frustum.p5.a	= mat._14 - mat._13;
		CameraBlender_internal::s_Frustum.p5.b	= mat._24 - mat._23;
		CameraBlender_internal::s_Frustum.p5.c	= mat._34 - mat._33;
		CameraBlender_internal::s_Frustum.p5.d	= mat._44 - mat._43;

	}

	const float3& CameraBlender::GetEyePosition( void )
	{
		return CameraBlender_internal::s_Eye;
	}

	const float3& CameraBlender::GetTarget( void )
	{
		return CameraBlender_internal::s_At;
	}

	const float3& CameraBlender::GetUp( void )
	{
		return CameraBlender_internal::s_Up;
	}

	const float CameraBlender::GetFOV( void )
	{
		return CameraBlender_internal::s_fov;
	}

	const Matrix& CameraBlender::GetViewMatrix( void )
	{
		return CameraBlender_internal::s_matView;
	}

	const Matrix& CameraBlender::GetProjectionMatrix( void )
	{
		return CameraBlender_internal::s_matProj;
	}

	const Frustum& CameraBlender::GetFrustum( void )
	{
		return CameraBlender_internal::s_Frustum;
	}
	
}} // namespace sx { namespace core {