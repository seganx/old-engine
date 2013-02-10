#include "sxCamera.h"

static const UINT cameraFileID = MAKEFOURCC('C', 'A', 'M', 'R');

namespace sx { namespace core {


	//////////////////////////////////////////////////////////////////////////
	//	CAMERA STRUCTURE
	Camera::Camera( void )
		: FOV(PI/3.0f), Aspect(0), Far(6000.0f), Velocity(0), Amplitude(0),
		Eye(0.0f, 0.0f, -1.0f), At(math::VEC3_ZERO), Up(0.0f, 1.0f, 0.0f), matMode(0)
	{
		
	}

	Camera::Camera( const Camera& cam )
		: FOV(cam.FOV), Aspect(cam.Aspect), Far(cam.Far), 
		Velocity(cam.Velocity), Amplitude(cam.Amplitude),
		Eye(cam.Eye), At(cam.At), Up(cam.Up), matMode(cam.matMode)
	{

	}

	FORCEINLINE void Camera::SetSpherical( float radius, float phi, float theta )
	{
		Eye.x = At.x + radius * cos(phi) * sin(theta);
		Eye.y = At.y + radius * cos(theta);
		Eye.z = At.z + radius * sin(phi) * sin(theta);
	}

	void Camera::GetSpherical( pfloat raduis /*= NULL*/, pfloat phi /*= NULL*/, pfloat theta /*= NULL*/ )
	{
		const double d[3] = {Eye.x-At.x, Eye.y-At.y, Eye.z-At.z};
		double r = sqrt( d[0]*d[0] + d[1]*d[1] + d[2]*d[2] );
		if ( r < 0.01 ) r = 0.01;
		if (raduis) *raduis = static_cast<float>(r);
		
		double t = acos( d[1] / r );
		SEGAN_CLAMP(t, 0.001, 3.135);
		if (theta) *theta = static_cast<float>(t);

		if (phi)
		{
			double rad = sin(t) * r;
			if (rad < DBL_EPSILON) rad = DBL_EPSILON;
			rad = asin( d[2] / rad );

			if (d[0]>0)
				*phi = static_cast<float>(rad);
			else
				*phi = PI - static_cast<float>(rad);
		}
	}

	FORCEINLINE float3& Camera::GetDirection( void )
	{
		static float3 dir;
		dir = At - Eye;
		return dir;
	}

	FORCEINLINE void Camera::GetViewMatrix( Matrix& matView )
	{
		matView.LookAtLH(Eye, At, Up);
	}

	void Camera::GetProjectionMatrix( Matrix& matProj )
	{
		switch (matMode)
		{
		case 0:
			{
				//  compute near distance depend on far distance
				float Near = 0.00005f * Far;

				//  aspect ratio correction
				float aspect = Aspect>0 ? Aspect : SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;

				matProj.PerspectiveFovLH(FOV, aspect, Near, Far);
			}
			break;

		case 1:
			{
				if (orthoWidth>EPSILON && orthoHeight>EPSILON)
				{
					float Near = -0.002f * Far;
					matProj.OrthoLH( orthoWidth, orthoHeight, Near, Far );
				}
				else
				{
					const float d[3] = {Eye.x-At.x, Eye.y-At.y, Eye.z-At.z};
					const float r = sqrt( d[0]*d[0] + d[1]*d[1] + d[2]*d[2] );
					matProj.OrthoLH( r * (SEGAN_VP_WIDTH/SEGAN_VP_HEIGHT), r, -Far, Far);
				}
			}
			break;
		}

	}

	void Camera::GetFrustum( Frustum& frustum )
	{
		static Matrix mat, m_matView, m_matProj;

		//  Extract 5 planes around view frustum
		GetViewMatrix(m_matView);
		GetProjectionMatrix(m_matProj);
		mat.Multiply(m_matView, m_matProj);

		// Near clipping plane
		frustum.p0.a = mat._13;
		frustum.p0.b = mat._23;
		frustum.p0.c = mat._33;
		frustum.p0.d = mat._43;

		// Left clipping plane
		frustum.p1.a = mat._14 + mat._11;
		frustum.p1.b = mat._24 + mat._21;
		frustum.p1.c = mat._34 + mat._31;
		frustum.p1.d = mat._44 + mat._41;

		// Right clipping plane
		frustum.p2.a = mat._14 - mat._11;
		frustum.p2.b = mat._24 - mat._21;
		frustum.p2.c = mat._34 - mat._31;
		frustum.p2.d = mat._44 - mat._41;

		// Top clipping plane
		frustum.p3.a = mat._14 - mat._12;
		frustum.p3.b = mat._24 - mat._22;
		frustum.p3.c = mat._34 - mat._32;
		frustum.p3.d = mat._44 - mat._42;

		// Bottom clipping plane
		frustum.p4.a = mat._14 + mat._12;
		frustum.p4.b = mat._24 + mat._22;
		frustum.p4.c = mat._34 + mat._32;
		frustum.p4.d = mat._44 + mat._42;

		// Far clipping plane
		frustum.p5.a = mat._14 - mat._13;
		frustum.p5.b = mat._24 - mat._23;
		frustum.p5.c = mat._34 - mat._33;
		frustum.p5.d = mat._44 - mat._43;
	}

	Ray& Camera::GetRay( float absX, float absY )
	{
		static Ray ray(math::VEC3_ZERO, math::VEC3_ZERO);

		Matrix matView, matProj;
		GetViewMatrix(matView);
		GetProjectionMatrix(matProj);
		ray.Compute( absX, absY, SEGAN_VP_WIDTH, SEGAN_VP_HEIGHT, matView, matProj );
		return ray;
	}

	void Camera::SetToDevice( void )
	{
		Matrix matView, matProj;
		GetViewMatrix(matView);
		GetProjectionMatrix(matProj);
		d3d::Device3D::Matrix_View_Set(matView);
		d3d::Device3D::Matrix_Project_Set(matProj);
		sx::d3d::ShaderPool::Update(0);
	}

	FORCEINLINE float Camera::ComputeViewDistance( const float3& position, float objRadius )
	{
		return cmn::ViewDistanceByCamera(Eye, At, FOV, position, objRadius);
	}

	void Camera::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, cameraFileID);

		int version = 1;
		SEGAN_STREAM_WRITE(stream, version);

		SEGAN_STREAM_WRITE(stream, FOV);
		SEGAN_STREAM_WRITE(stream, Aspect);
		SEGAN_STREAM_WRITE(stream, Far);
		SEGAN_STREAM_WRITE(stream, Velocity);
		SEGAN_STREAM_WRITE(stream, Amplitude);
		SEGAN_STREAM_WRITE(stream, Eye);
		SEGAN_STREAM_WRITE(stream, At);
		SEGAN_STREAM_WRITE(stream, Up);
		SEGAN_STREAM_WRITE(stream, orthoWidth);
		SEGAN_STREAM_WRITE(stream, orthoHeight);
		SEGAN_STREAM_WRITE(stream, matMode);

	}

	void Camera::Load( Stream& stream )
	{
		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != cameraFileID)
		{
			sxLog::Log(L"Incompatible file format for loading camera !");
			return;
		}

		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if ( version == 1 )
		{
			SEGAN_STREAM_READ(stream, FOV);
			SEGAN_STREAM_READ(stream, Aspect);
			SEGAN_STREAM_READ(stream, Far);
			SEGAN_STREAM_READ(stream, Velocity);
			SEGAN_STREAM_READ(stream, Amplitude);
			SEGAN_STREAM_READ(stream, Eye);
			SEGAN_STREAM_READ(stream, At);
			SEGAN_STREAM_READ(stream, Up);
			SEGAN_STREAM_READ(stream, orthoWidth);
			SEGAN_STREAM_READ(stream, orthoHeight);
			SEGAN_STREAM_READ(stream, matMode);
		}

	}

}} // namespace sx { namespace core {