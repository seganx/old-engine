#include "sxUI3D.h"
#include "sxDevice3D.h"
#include "sxResource3D.h"


#define SPHERE_PHI		36
#define SPHERE_THETA	18

#define CONE_VCOUNT		40

/*
this class contain some static data that we will create once
*/
class UI3D_internal
{
public:
	static void ReadyToDebug(D3DColor color)
	{
		sx::d3d::Device3D::SetEffect(NULL);
		for (int i=0; i<8; i++)
			sx::d3d::Device3D::SetTexture(i, NULL);
		for (int i=0; i<8; i++)
			sx::d3d::Device3D::SetVertexBuffer(i, NULL, 0);
		sx::d3d::Device3D::SetIndexBuffer(NULL);
		sx::d3d::Device3D::SetMaterialColor(color);
		sx::d3d::Device3D::Texture_ResetStateStages();
		sx::d3d::Device3D::Texture_SetSampleState();
	}

	static PDirect3DVertexBuffer GetCompassVB(void)
	{
		if (vbCompas) return vbCompas;

		if ( sx::d3d::Resource3D::CreateVertexBuffer(18 * SEGAN_SIZE_VERTEX_0, vbCompas) )
		{
			PD3DVertex0 pv;
			if (SUCCEEDED( vbCompas->Lock(0, 18 * SEGAN_SIZE_VERTEX_0, (void**)&pv, 0) ))
			{
				float	bs = 0.15f;
				float	bl = 0.85f;

				//  define x line
				pv[0].pos = Vector(0, 0, 0);
				pv[1].pos = Vector(1, 0, 0);
				pv[2].pos = Vector(bl, 0, bs);
				pv[3].pos = Vector(bl, 0,-bs);
				pv[4].pos = pv[1].pos;

				//  define y line
				pv[5].pos = Vector(0, 0, 0);
				pv[6].pos = Vector(0, 1, 0);
				pv[7].pos = Vector(0, bl, bs);
				pv[8].pos = Vector(0, bl, -bs);
				pv[9].pos = pv[6].pos;
				pv[10].pos	= Vector( bs, bl, 0);
				pv[11].pos	= Vector(-bs, bl, 0);
				pv[12].pos	= pv[6].pos;

				//  define z line
				pv[13].pos	= Vector(0, 0, 0);
				pv[14].pos	= Vector(0, 0, 1);
				pv[15].pos	= Vector( bs, 0, bl);
				pv[16].pos	= Vector(-bs, 0, bl);
				pv[17].pos	= pv[14].pos;

				vbCompas->Unlock();
			}
			else sx::d3d::Resource3D::ReleaseVertexBuffer(vbCompas);
		}

		return vbCompas;
	}

	static PDirect3DVertexBuffer GetGridVB(void)
	{
		if (vbGrid) return vbGrid;

		if ( sx::d3d::Resource3D::CreateVertexBuffer(406 * SEGAN_SIZE_VERTEX_0, vbGrid) )
		{
			PD3DVertex0 pv;
			if (SUCCEEDED( vbGrid->Lock(0, 406 * SEGAN_SIZE_VERTEX_0, (void**)&pv, 0)))
			{
				//  place lines along x
				for (int i=0, x=1; i<200; x++, i+=4)
				{
					pv[i+0].pos = Vector( (float)x, 0, 1);
					pv[i+1].pos = Vector( (float)x, 0,-1);

					pv[i+2].pos = Vector(-(float)x, 0, 1);
					pv[i+3].pos = Vector(-(float)x, 0,-1);
				}

				//  place lines along z
				for (int i=200, z=1; i<400; z++, i+=4)
				{
					pv[i+0].pos = Vector( 1, 0, (float)z);
					pv[i+1].pos = Vector(-1, 0, (float)z);

					pv[i+2].pos = Vector( 1, 0,-(float)z);
					pv[i+3].pos = Vector(-1, 0,-(float)z);
				}

				//  place a line for x axes
				pv[400].pos = Vector( 1, 0, 0);
				pv[401].pos = Vector(-1, 0, 0);

				//  place a line for y axes
				pv[402].pos = sx::math::VEC3_ZERO;
				pv[403].pos = Vector( 0, 1, 0);

				//  place a line for z axes
				pv[404].pos = Vector( 0, 0, 1);
				pv[405].pos = Vector( 0, 0,-1);

				vbGrid->Unlock();
			}
			else sx::d3d::Resource3D::ReleaseVertexBuffer(vbGrid);
		}

		return vbGrid;
	}

	static PDirect3DVertexBuffer GetSphereBuffer0(void)
	{
		if (vbSphere0) return vbSphere0;

		const float sPhi = sx::math::PIMUL2 / SPHERE_PHI;
		const float sTheta = PI / SPHERE_THETA;
		const int vCount = SPHERE_PHI * SPHERE_THETA * 6;

		if ( sx::d3d::Resource3D::CreateVertexBuffer(vCount * SEGAN_SIZE_VERTEX_0, vbSphere0) )
		{
			if ( sx::d3d::Resource3D::CreateVertexBuffer(vCount * SEGAN_SIZE_VERTEX_1, vbSphere1) )
			{
				//	lock vertex buffers
				PD3DVertex0 pv0;
				if (SUCCEEDED( vbSphere0->Lock(0, vCount * SEGAN_SIZE_VERTEX_0, (void**)&pv0, 0)))
				{
					PD3DVertex1 pv1;
					if (SUCCEEDED( vbSphere1->Lock(0, vCount * SEGAN_SIZE_VERTEX_1, (void**)&pv1, 0)))
					{
						// compute sphere vertices
						for (int t=0; t<SPHERE_THETA; t++)
						{
							for (int p=0; p<SPHERE_PHI; p++)
							{
								float p1 = (float)p * sPhi;
								float t1 = (float)t * sTheta;
								float p2 = p1 + sPhi;
								float t2 = t1 + sTheta;

								int i = (t * SPHERE_PHI + p) * 6;

								pv0[i + 0].pos = float3(cos(p1) * sin(t1), cos(t1), sin(p1) * sin(t1));
								pv0[i + 1].pos = float3(cos(p2) * sin(t1), cos(t1), sin(p2) * sin(t1));
								pv0[i + 2].pos = float3(cos(p1) * sin(t2), cos(t2), sin(p1) * sin(t2));											
								pv0[i + 3].pos = float3(cos(p1) * sin(t2), cos(t2), sin(p1) * sin(t2));
								pv0[i + 4].pos = float3(cos(p2) * sin(t1), cos(t1), sin(p2) * sin(t1));
								pv0[i + 5].pos = float3(cos(p2) * sin(t2), cos(t2), sin(p2) * sin(t2));

								pv1[i + 0].txc = float2(p1/PI, t1/PI);
								pv1[i + 1].txc = float2(p2/PI, t1/PI);
								pv1[i + 2].txc = float2(p1/PI, t2/PI);
								pv1[i + 3].txc = float2(p1/PI, t2/PI);
								pv1[i + 4].txc = float2(p2/PI, t1/PI);
								pv1[i + 5].txc = float2(p2/PI, t2/PI);

								pv1[i + 0].nrm = pv0[i + 0].pos;
								pv1[i + 1].nrm = pv0[i + 1].pos;
								pv1[i + 2].nrm = pv0[i + 2].pos;
								pv1[i + 3].nrm = pv0[i + 3].pos;
								pv1[i + 4].nrm = pv0[i + 4].pos;
								pv1[i + 5].nrm = pv0[i + 5].pos;

								pv1[i + 0].col0= 0xffffffff;
								pv1[i + 1].col0= 0xffffffff;
								pv1[i + 2].col0= 0xffffffff;
								pv1[i + 3].col0= 0xffffffff;
								pv1[i + 4].col0= 0xffffffff;
								pv1[i + 5].col0= 0xffffffff;								

								pv1[i + 0].col1= 0xffffffff;
								pv1[i + 1].col1= 0xffffffff;
								pv1[i + 2].col1= 0xffffffff;
								pv1[i + 3].col1= 0xffffffff;
								pv1[i + 4].col1= 0xffffffff;
								pv1[i + 5].col1= 0xffffffff;
							}
						}
						// compute sphere vertices

						vbSphere1->Unlock();
					}
					else sx::d3d::Resource3D::ReleaseVertexBuffer(vbSphere1);

					vbSphere0->Unlock();
				}
				else sx::d3d::Resource3D::ReleaseVertexBuffer(vbSphere0);
				//	lock vertex buffers
			}
			else sx::d3d::Resource3D::ReleaseVertexBuffer(vbSphere0);
			
		}

		return vbSphere0;
	}

	static PDirect3DVertexBuffer GetSphereBuffer1(void)
	{
		return vbSphere1;
	}

	static PDirect3DVertexBuffer GetCircleBuffer(void)
	{
		if (vbCircle) return vbCircle;

		const int	vCount	= 181;
		const float vPhi	= PI / ((float)vCount-1.0f);
		if ( sx::d3d::Resource3D::CreateVertexBuffer(vCount * SEGAN_SIZE_VERTEX_0, vbCircle) )
		{
			//	lock vertex buffers
			PVertex0 pv;
			if (SUCCEEDED( vbCircle->Lock(0, vCount * SEGAN_SIZE_VERTEX_0, (void**)&pv, 0)))
			{
				// compute circle vertices
				for (int i=0; i<vCount; i++)
				{
					float phi = vPhi * (float)i * 2.0f;
					pv[i].pos.x = cos(phi);
					pv[i].pos.y = sin(phi);
					pv[i].pos.z = 0.0f;
				}

				vbCircle->Unlock();
			}
			else sx::d3d::Resource3D::ReleaseVertexBuffer(vbCircle);
		}
		
		return vbCircle;
	}

	static PDirect3DVertexBuffer GetConeBuffer(void)
	{
		if (vbCone) return vbCone;

		const int	vCount	= CONE_VCOUNT;
		const float vPhi	= PI / ((float)vCount-1.0f);
		if ( sx::d3d::Resource3D::CreateVertexBuffer(vCount * SEGAN_SIZE_VERTEX_0, vbCone) )
		{
			//	lock vertex buffers
			PD3DVertex0 v0;
			if ( SUCCEEDED( vbCone->Lock(0, 0, (void**)&v0, 0) ) )
			{
				float D = PI / 9.0f;
				float sinD , cosD;

				v0[0].pos	= float3(0.0f, 1.0f, 0.0f);
				for (int i=1; i<20; i++)
				{
					cosD = cos( (float)(i-1) * D + sx::math::PIDIV2 );
					sinD = sin( (float)(i-1) * D + sx::math::PIDIV2 );

					v0[i].pos.x = - cosD;
					v0[i].pos.y = 0.0f;
					v0[i].pos.z = + sinD;
				}

				v0[20].pos	= float3(0.0f, 0.0f, 0.0f);
				for (int i=20; i<40; i++)
				{
					cosD = cos( ((float)i-0.5f) * D + sx::math::PIDIV2 );
					sinD = sin( ((float)i-0.5f) * D + sx::math::PIDIV2 );

					v0[i].pos.z = - cosD;
					v0[i].pos.y = 0.0f;
					v0[i].pos.x = + sinD;
				}


				vbCone->Unlock();
			} 
			else sx::d3d::Resource3D::ReleaseVertexBuffer(vbCone);
			//	lock vertex buffers
		}

		return vbCone;
	}

private:
	static PDirect3DVertexBuffer	vbCompas;
	static PDirect3DVertexBuffer	vbGrid;
	static PDirect3DVertexBuffer	vbSphere0;
	static PDirect3DVertexBuffer	vbSphere1;
	static PDirect3DVertexBuffer	vbCircle;
	static PDirect3DVertexBuffer	vbCone;
};
PDirect3DVertexBuffer	UI3D_internal::vbCompas		= NULL;
PDirect3DVertexBuffer	UI3D_internal::vbGrid		= NULL;
PDirect3DVertexBuffer	UI3D_internal::vbSphere0	= NULL;
PDirect3DVertexBuffer	UI3D_internal::vbSphere1	= NULL;
PDirect3DVertexBuffer	UI3D_internal::vbCircle		= NULL;
PDirect3DVertexBuffer	UI3D_internal::vbCone		= NULL;

namespace sx { namespace d3d
{

	FORCEINLINE void UI3D::ReadyToDebug( const D3DColor color )
	{
		sx_callstack();

		UI3D_internal::ReadyToDebug( color>0 ? color : 0xffffffff );
	}

	void UI3D::DrawLine( const Vector& v1, const Vector& v2 )
	{
		static Vertex0 vert[2];
		vert[0].pos = v1;
		vert[1].pos = v2;
		Device3D::DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &vert, SEGAN_SIZE_VERTEX_0);
	}

	void UI3D::DrawQuad( void )
	{
		sx_callstack();

		static PDirect3DVertexBuffer vbQuad0 = NULL;
		static PDirect3DVertexBuffer vbQuad1 = NULL;
		if ( !vbQuad0 || !vbQuad1 )
		{
			bool failed = false;
			
			if ( Resource3D::CreateVertexBuffer(4 * SEGAN_SIZE_VERTEX_0, vbQuad0) )
			{
				if ( Resource3D::CreateVertexBuffer(4 * SEGAN_SIZE_VERTEX_1, vbQuad1) )
				{		
					//  Apply Vertices To Buffer
					PD3DVertex0 v0;
					if ( SUCCEEDED( vbQuad0->Lock(0, 0, (void**)&v0, 0) ) )
					{
						v0[0].pos = float3(-1.0f,  1.0f, 0.0f);
						v0[1].pos = float3( 1.0f,  1.0f, 0.0f);
						v0[2].pos = float3(-1.0f, -1.0f, 0.0f);
						v0[3].pos = float3( 1.0f, -1.0f, 0.0f);
						vbQuad0->Unlock();

						PD3DVertex1 v1;
						if ( SUCCEEDED( vbQuad1->Lock(0, 0, (void**)&v1, 0) ) )
						{
							v1[0].txc	= Vector2(0.0f, 0.0f);
							v1[1].txc	= Vector2(1.0f, 0.0f);
							v1[2].txc	= Vector2(0.0f, 1.0f);
							v1[3].txc	= Vector2(1.0f, 1.0f);

							for (int i=0; i<4; i++)
							{
								v1[i].nrm	= math::VEC3_ZERO;
								v1[i].col0	= 0xFFFFFFFF;
								v1[i].col1	= 0xFFFFFFFF;
							}

							vbQuad1->Unlock();
						}//if ( SUCCEEDED( vbQuad1->Lock(0, 0, (void**)&v1, 0) ) )
					}//if ( SUCCEEDED( vbQuad0->Lock(0, 0, (void**)&v0, 0) ) )
				}
			}

			if (failed)
			{
				Resource3D::ReleaseVertexBuffer( vbQuad0 );
				Resource3D::ReleaseVertexBuffer( vbQuad1 );
			}
		}

		//  draw quad
		d3d::Device3D::SetIndexBuffer(NULL);
		d3d::Device3D::SetVertexBuffer(0, vbQuad0,	SEGAN_SIZE_VERTEX_0);
		d3d::Device3D::SetVertexBuffer(1, vbQuad1,	SEGAN_SIZE_VERTEX_1);
		d3d::Device3D::SetVertexBuffer(2, NULL,	0);
		d3d::Device3D::SetVertexBuffer(3, NULL,	0);
		d3d::Device3D::SetVertexBuffer(4, NULL,	0);
		d3d::Device3D::SetVertexBuffer(5, NULL,	0);
		d3d::Device3D::DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}

	void UI3D::DrawRectangle( const Vector& v1, const Vector& v2, const Vector& v3, const Vector& v4 )
	{
		static struct vertStruct {float3 pos; float2 uv;} vert[4];
		vert[0].pos = v1; vert[0].uv.x = 0.0f; vert[0].uv.y = 0.0f;
		vert[1].pos = v2; vert[1].uv.x = 1.0f; vert[1].uv.y = 0.0f;
		vert[2].pos = v4; vert[2].uv.x = 0.0f; vert[2].uv.y = 1.0f;
		vert[3].pos = v3; vert[3].uv.x = 1.0f; vert[3].uv.y = 1.0f;

		Device3D::DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertStruct), D3DFVF_XYZ | D3DFVF_TEX1 );
	}

	void UI3D::DrawWiredRectangle( const Vector& v1, const Vector& v2, const Vector& v3, const Vector& v4 )
	{
		static Vertex0 vert[5];
		vert[0].pos = v1;
		vert[1].pos = v2;
		vert[2].pos = v3;
		vert[3].pos = v4;
		vert[4].pos = v1;
		Device3D::DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, SEGAN_SIZE_VERTEX_0);
	}

	void UI3D::DrawAABox( const math::AABox& box, const D3DColor color )
	{

	}

	void UI3D::DrawOBBox( const math::OBBox& Box, D3DColor color )
	{

	}

	void UI3D::DrawWiredAABox( const math::AABox& box, const D3DColor color )
	{
		UI3D_internal::ReadyToDebug( color>0 ? color : 0xff000000 );

		static float3 v[24];

		v[0].Set(  box.Min.x, box.Min.y, box.Min.z );	v[1].Set(  box.Max.x, box.Min.y, box.Min.z );
		v[2].Set(  box.Min.x, box.Min.y, box.Min.z );	v[3].Set(  box.Min.x, box.Max.y, box.Min.z );
		v[4].Set(  box.Min.x, box.Min.y, box.Min.z );	v[5].Set(  box.Min.x, box.Min.y, box.Max.z );

		v[6].Set(  box.Max.x, box.Max.y, box.Max.z );	v[7].Set(  box.Min.x, box.Max.y, box.Max.z );
		v[8].Set(  box.Max.x, box.Max.y, box.Max.z );	v[9].Set(  box.Max.x, box.Min.y, box.Max.z );
		v[10].Set( box.Max.x, box.Max.y, box.Max.z );	v[11].Set( box.Max.x, box.Max.y, box.Min.z );

		v[12].Set( box.Max.x, box.Min.y, box.Min.z );	v[13].Set( box.Max.x, box.Max.y, box.Min.z );
		v[14].Set( box.Max.x, box.Min.y, box.Min.z );	v[15].Set( box.Max.x, box.Min.y, box.Max.z );

		v[16].Set( box.Min.x, box.Max.y, box.Min.z );	v[17].Set( box.Min.x, box.Max.y, box.Max.z );
		v[18].Set( box.Min.x, box.Max.y, box.Min.z );	v[19].Set( box.Max.x, box.Max.y, box.Min.z );

		v[20].Set( box.Min.x, box.Min.y, box.Max.z );	v[21].Set( box.Min.x, box.Max.y, box.Max.z );
		v[22].Set( box.Min.x, box.Min.y, box.Max.z );	v[23].Set( box.Max.x, box.Min.y, box.Max.z );

		Device3D::DrawPrimitiveUP(D3DPT_LINELIST, 12, v, sizeof(Vector));
	}

	void UI3D::DrawWiredOBBox( const math::OBBox& ooBox, const D3DColor color )
	{
		UI3D_internal::ReadyToDebug( color>0 ? color : 0xff000000 );

		static float3 v[24];
		v[0]  = ooBox.v[0];	v[1]  = ooBox.v[1];
		v[2]  = ooBox.v[1];	v[3]  = ooBox.v[2];
		v[4]  = ooBox.v[2];	v[5]  = ooBox.v[3];
		v[6]  = ooBox.v[3];	v[7]  = ooBox.v[0];
		v[8]  = ooBox.v[4];	v[9]  = ooBox.v[5];
		v[10] = ooBox.v[5];	v[11] = ooBox.v[6];
		v[12] = ooBox.v[6];	v[13] = ooBox.v[7];
		v[14] = ooBox.v[7];	v[15] = ooBox.v[4];
		v[16] = ooBox.v[0];	v[17] = ooBox.v[5];
		v[18] = ooBox.v[1];	v[19] = ooBox.v[6];
		v[20] = ooBox.v[2];	v[21] = ooBox.v[7];
		v[22] = ooBox.v[3];	v[23] = ooBox.v[4];

		Device3D::DrawPrimitiveUP(D3DPT_LINELIST, 12, v, sizeof(Vector));
	}

	void UI3D::DrawSelectAABox( const math::AABox& box, const D3DColor color )
	{

	}

	void UI3D::DrawSelectOBBox( const math::OBBox& box, const D3DColor color )
	{

	}

	void UI3D::DrawCircle( const Matrix& matWorld, float radius, const D3DColor color )
	{
		UI3D_internal::ReadyToDebug( color );

		Matrix mat; mat.Scale(radius, radius, radius);
		mat.Multiply(mat, matWorld);
		Device3D::Matrix_World_Set(mat);

		Device3D::SetVertexBuffer(0, UI3D_internal::GetCircleBuffer(), SEGAN_SIZE_VERTEX_0);
		Device3D::SetVertexBuffer(1, NULL, 0);
		Device3D::SetVertexBuffer(2, NULL, 0);
		Device3D::SetVertexBuffer(3, NULL, 0);
		Device3D::SetVertexBuffer(4, NULL, 0);
		Device3D::DrawPrimitive(D3DPT_LINESTRIP, 0, 180);

		Device3D::Matrix_World_Set(matWorld);
	}

	void UI3D::DrawSphere( const Sphere& sphere, const D3DColor color )
	{
		UI3D_internal::ReadyToDebug(color);

		Matrix matWorld; Device3D::Matrix_World_Get(matWorld);

		Matrix mat; 
		mat.Scale(sphere.r, sphere.r, sphere.r);
		mat.TranslateIt(sphere.x, sphere.y, sphere.z);
		mat.Multiply(mat, matWorld);
		Device3D::Matrix_World_Set(mat);

		Device3D::SetVertexBuffer(0, UI3D_internal::GetSphereBuffer0(), SEGAN_SIZE_VERTEX_0);
		Device3D::SetVertexBuffer(1, UI3D_internal::GetSphereBuffer1(), SEGAN_SIZE_VERTEX_1);
		Device3D::SetVertexBuffer(2, NULL, 0);
		Device3D::SetVertexBuffer(3, NULL, 0);
		Device3D::SetVertexBuffer(4, NULL, 0);
		Device3D::DrawPrimitive(D3DPT_TRIANGLELIST, 0, SPHERE_PHI * SPHERE_THETA * 2);

		Device3D::Matrix_World_Set(matWorld);
	}

	void UI3D::DrawWiredSphere( const Sphere& sphere, const D3DColor color )
	{
		UI3D_internal::ReadyToDebug( color );

		Matrix matWorld; Device3D::Matrix_World_Get(matWorld);

		Matrix mat;
		mat.Translation(sphere.x, sphere.y, sphere.z);
		mat.Multiply(mat, matWorld);
		DrawCircle( mat, sphere.r, color>0 ? color : D3DColor(0.0f, 0.0f, 1.0f, 1.0f) );

		mat.RotationPYR(0.0f, math::PIDIV2, 0.0f);
		mat.TranslateIt(sphere.x, sphere.y, sphere.z);
		mat.Multiply(mat, matWorld);
		DrawCircle( mat, sphere.r, color>0 ? color : D3DColor(1.0f, 0.0f, 0.0f, 1.0f) );

		mat.RotationPYR(math::PIDIV2, 0.0f, 0.0f);
		mat.TranslateIt(sphere.x, sphere.y, sphere.z);
		mat.Multiply(mat, matWorld);
		DrawCircle( mat, sphere.r, color>0 ? color : D3DColor(0.0f, 1.0f, 0.0f, 1.0f) );

		Device3D::Matrix_World_Set(matWorld);
	}

	void UI3D::DrawCone( const D3DColor color )
	{
		UI3D_internal::ReadyToDebug( color );

		Device3D::SetVertexBuffer(0, UI3D_internal::GetConeBuffer(), SEGAN_SIZE_VERTEX_0);
		Device3D::DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 18);
		Device3D::DrawPrimitive(D3DPT_TRIANGLEFAN, 20, 18);
	}

	void UI3D::DrawCompass( void )
	{
		UI3D_internal::ReadyToDebug(D3DColor(1, 0, 0, 1));

		Matrix currentProj;
		Device3D::Matrix_Project_Get(currentProj);

		Matrix compasProj;
		compasProj.OrthoLH(	(float)Device3D::Viewport()->Width,
							(float)Device3D::Viewport()->Height,
							-2000.0f, 
							+2000.0f);

		Device3D::Matrix_Project_Set(compasProj);

		Device3D::SetVertexBuffer(0, UI3D_internal::GetCompassVB(), SEGAN_SIZE_VERTEX_0);
		Device3D::SetVertexBuffer(1, NULL, 0);
		Device3D::SetVertexBuffer(2, NULL, 0);
		Device3D::SetVertexBuffer(3, NULL, 0);
		Device3D::SetVertexBuffer(4, NULL, 0);

		Device3D::RS_ZEnabled(false);
		Device3D::RS_AntialiasLine(true);
		Device3D::DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
		Device3D::SetMaterialColor(D3DColor(0, 1, 0, 1));
		Device3D::DrawPrimitive(D3DPT_LINESTRIP, 5, 7);
		Device3D::SetMaterialColor(D3DColor(0, 0, 1, 1));
		Device3D::DrawPrimitive(D3DPT_LINESTRIP, 13, 4);
		Device3D::RS_AntialiasLine(false);

		Device3D::Matrix_Project_Set(currentProj);
	}

	void UI3D::DrawGrid( int size, const D3DColor& color )
	{
		UI3D_internal::ReadyToDebug(color);

		size /= 2;
		SEGAN_CLAMP(size, 0, 100);
		Matrix matWorld;

		Device3D::SetVertexBuffer(0, UI3D_internal::GetGridVB(), SEGAN_SIZE_VERTEX_0);
		Device3D::SetVertexBuffer(1, NULL, 0);
		Device3D::SetVertexBuffer(2, NULL, 0);
		Device3D::SetVertexBuffer(3, NULL, 0);
		Device3D::SetVertexBuffer(4, NULL, 0);

		Device3D::RS_Alpha( 0 );
		Device3D::RS_AntialiasLine(true);
		Device3D::RS_ZEnabled(true);

		//  draw grid
		matWorld.Scale(1, 1, (float)size);
		Device3D::Matrix_World_Set(matWorld);
		Device3D::DrawPrimitive(D3DPT_LINELIST, 0, size*2);

		matWorld.Scale((float)size, 1, 1);
		Device3D::Matrix_World_Set(matWorld);
		Device3D::DrawPrimitive(D3DPT_LINELIST, 200, size*2);

		//  draw axises
		matWorld.Scale((float)size, 2, (float)size);
		Device3D::Matrix_World_Set(matWorld);
		Device3D::SetMaterialColor(D3DColor(1, 0, 0, 1));
		Device3D::DrawPrimitive(D3DPT_LINELIST, 400, 1);
		Device3D::SetMaterialColor(D3DColor(0, 0, 1, 1));
		Device3D::DrawPrimitive(D3DPT_LINELIST, 404, 1);
		Device3D::SetMaterialColor(D3DColor(0, 1, 0, 1));
		Device3D::DrawPrimitive(D3DPT_LINELIST, 402, 1);

		Device3D::RS_AntialiasLine(false);

	}

	bool UI3D::IntersectRect( Rect3D& rc, Matrix& mat, Ray& ray, float3& OUT hitPoint, bool twoSide /*= false*/ )
	{
		if ( twoSide)
		{
			Rect3D r3d0, r3d1;
			r3d0.Transform( rc, mat );

			r3d1.v0 = r3d0.v0;
			r3d1.v1 = r3d0.v3;
			r3d1.v2 = r3d0.v2;
			r3d1.v3 = r3d0.v1;

			return ray.Intersect_Rect3D( r3d0, &hitPoint, NULL ) || ray.Intersect_Rect3D( r3d1, &hitPoint, NULL );
		}
		else
		{
			Rect3D r3d;
			r3d.Transform( rc, mat );
			return ray.Intersect_Rect3D( r3d, &hitPoint, NULL );
		}
	}

	bool UI3D::IntersectCone( Matrix& mat, Ray& ray, float3& OUT hitPoint )
	{
		PDirect3DVertexBuffer vb = UI3D_internal::GetConeBuffer();
		if (!vb) return false;

		PVertex0 v = NULL;
		if ( SUCCEEDED( vb->Lock(0, 0, (void**)&v, 0 ) ) )
		{
			float3 v0, v1, v2;

			int vCount = CONE_VCOUNT/2 - 1;
			for (int i=1; i<vCount; i++)
			{
				v0.Transform( v[0].pos,		mat );
				v1.Transform( v[i].pos,		mat );
				v2.Transform( v[i+1].pos,	mat );

				if ( ray.Intersect_Triangle( v0, v1, v2, &hitPoint, NULL ) )
					return true;
			}

			vCount = CONE_VCOUNT - 1;
			int vs = CONE_VCOUNT/2;
			for (int i=vs; i<vCount; i++)
			{
				v0.Transform( v[vs].pos,	mat );
				v1.Transform( v[i].pos,		mat );
				v2.Transform( v[i+1].pos,	mat );

				if ( ray.Intersect_Triangle( v0, v1, v2, &hitPoint, NULL ) )
					return true;
			}

			vb->Unlock();
		}

		return false;
	}


} } // namespace sx { namespace d3d