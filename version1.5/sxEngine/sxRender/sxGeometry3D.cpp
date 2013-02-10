#include "sxGeometry3D.h"
#include "sxDevice3D.h"
#include "sxGeometry3DTool.h"
#include "../sxSystem/sxSystem.h"


static const UINT geometry3DFileID = MAKEFOURCC('G', 'M', 'T', 'R');

namespace sx { namespace d3d
{
	Geometry3D::Geometry3D( void ) : m_VrtxCount(0), m_FaceCount(0), m_IB(NULL)
	{
		ZeroMemory(m_IsLocked, 5*sizeof(BYTE));
		for (int i=0; i<4; m_VB[i] = NULL, i++);
	}

	Geometry3D::~Geometry3D( void )
	{
		VB_Cleanup();
		IB_Cleanup();
	}

	bool Geometry3D::VB_Create( UINT VrtxNum, bool AnimationVertexBuffer )
	{
		//  cleanup current vertices
		VB_Cleanup();

		m_VrtxCount = VrtxNum;

		if (VrtxNum<1) 
			return true;

		if ( !Resource3D::GetVertexBuffer(VrtxNum, SEGAN_SIZE_VERTEX_0, m_VB[0]) )
		{
			VB_Cleanup();
			return false;
		}

		if ( !Resource3D::GetVertexBuffer(VrtxNum, SEGAN_SIZE_VERTEX_1, m_VB[1]) )
		{
			VB_Cleanup();
			return false;
		}

		if ( !Resource3D::GetVertexBuffer(VrtxNum, SEGAN_SIZE_VERTEX_2, m_VB[2]) )
		{
			VB_Cleanup();
			return false;
		}

		if ( AnimationVertexBuffer )
		{
			if ( !Resource3D::GetVertexBuffer(VrtxNum, SEGAN_SIZE_VERTEX_3, m_VB[3]) )
			{
				VB_Cleanup();
				return false;
			}
		}

		return true;
	}

	bool Geometry3D::VB_CreateByIndex( UINT VrtxNum, int index, DWORD SX_RESOURCE_ flag /*= 0*/ )
	{
		if ( index<0 || index>3 ) 
			return false;

		m_VrtxCount = VrtxNum;

		if ( VrtxNum < 1 )
			return true;

		Resource3D::ReleaseVertexBuffer( m_VB[index] );
		switch (index)
		{
		case 0:	return Resource3D::CreateVertexBuffer(VrtxNum * SEGAN_SIZE_VERTEX_0, m_VB[0], flag);
		case 1:	return Resource3D::CreateVertexBuffer(VrtxNum * SEGAN_SIZE_VERTEX_1, m_VB[1], flag);
		case 2:	return Resource3D::CreateVertexBuffer(VrtxNum * SEGAN_SIZE_VERTEX_2, m_VB[2], flag);
		case 3:	return Resource3D::CreateVertexBuffer(VrtxNum * SEGAN_SIZE_VERTEX_3, m_VB[3], flag);
		}

		return false;
	}

	bool Geometry3D::VB_GetDesc( int index, D3DVertexBufferDesc& vbDesc )
	{
		if (!m_VB[index]) return false;

		m_VB[index]->GetDesc(&vbDesc);

		switch (index)
		{
		case 0: vbDesc.Size = m_VrtxCount * SEGAN_SIZE_VERTEX_0; break;
		case 1: vbDesc.Size = m_VrtxCount * SEGAN_SIZE_VERTEX_1; break;
		case 2: vbDesc.Size = m_VrtxCount * SEGAN_SIZE_VERTEX_2; break;
		case 3: vbDesc.Size = m_VrtxCount * SEGAN_SIZE_VERTEX_3; break;
		}
		
		return true;
	}

	bool Geometry3D::VB_Lock( int index, PD3DVertex& vrtcs, DWORD lockFlag /*= 0*/ )
	{
		if ( m_VB[index] && !m_IsLocked[index] )
		{
			D3DVertexBufferDesc desc;
			m_VB[index]->GetDesc(&desc);
			
			if ( desc.Usage & D3DUSAGE_DYNAMIC )
				m_IsLocked[index] = SUCCEEDED( m_VB[index]->Lock(0, 0, (void**)(&vrtcs), D3DLOCK_DISCARD) );
			else
				m_IsLocked[index] = SUCCEEDED( m_VB[index]->Lock(0, 0, (void**)(&vrtcs), lockFlag) );

			return m_IsLocked[index];
		}

#ifdef SEGAN_LOG_LEVEL3
		else
		{
			if ( !m_VB[index] )
				sxLog::Logger() << L"lock VB failed because VB is not exist!";
			if ( m_IsLocked[index] )
				sxLog::Logger() << L"lock VB failed because VB was locked!";
		}
#endif
		
		return false;
	}

	void Geometry3D::VB_UnLock( int index )
	{
		if (m_VB[index] && m_IsLocked[index])
		{
#ifdef SEGAN_LOG_LEVEL3
			if ( FAILED( m_VB[index]->Unlock() ) )
				sxLog::Logger() << L"unlock VB failed !";
#else
			m_VB[index]->Unlock();
#endif				
			m_IsLocked[index] = false;
		}
#ifdef SEGAN_LOG_LEVEL3
		else
		{
			if ( !m_VB[index] )
				sxLog::Logger() << L"unlock VB failed because VB is not exist!";
			if ( !m_IsLocked[index] )
				sxLog::Logger() << L"unlock VB failed because VB was unlocked!";
		}
#endif
	}

	void Geometry3D::VB_Cleanup( void )
	{	
		VB_UnLock(0);
		VB_UnLock(1);
		VB_UnLock(2);
		VB_UnLock(3);

		Resource3D::ReleaseVertexBuffer(m_VB[0]);
		Resource3D::ReleaseVertexBuffer(m_VB[1]);
		Resource3D::ReleaseVertexBuffer(m_VB[2]);
		Resource3D::ReleaseVertexBuffer(m_VB[3]);

		m_VrtxCount=0;
	}

	bool Geometry3D::IB_Create( UINT FaceNum, DWORD SX_RESOURCE_ flag /*= 0*/ )
	{
		//  cleanup current indices
		IB_Cleanup();

		m_FaceCount = FaceNum;

		bool res = flag ? Resource3D::CreateIndexBuffer(FaceNum * 12, m_IB, flag) : Resource3D::GetIndexBuffer(FaceNum, m_IB);
		if (!res)
		{
#ifdef SEGAN_LOG_LEVEL3
			String logText;
			logText << L"sorry! I do my best but IB does not created !";
			sxLog::Logger() << logText;
#endif
			IB_Cleanup();
			return false;
		}

		return true;
	}

	bool Geometry3D::IB_GetDesc( D3DIndexBufferDesc& ibDesc )
	{
		if (!m_IB) 
			return false;
		m_IB->GetDesc(&ibDesc);
		ibDesc.Size = m_FaceCount*12;
		return true;
	}

	bool Geometry3D::IB_Lock( PD3DFace& faces, DWORD lockFlag /*= 0*/ )
	{
		if (m_IB && !m_IsLocked[4] && SUCCEEDED(m_IB->Lock(0, 0, (void**)(&faces), lockFlag)))
		{
			m_IsLocked[4] = true;
			return true;
		}
		return false;
	}

	bool Geometry3D::IB_Lock( PDWORD& indices, DWORD lockFlag /*= 0*/ )
	{
		if (m_IB && !m_IsLocked[4] && SUCCEEDED(m_IB->Lock(0, 0, (void**)(&indices), lockFlag)))
		{
			m_IsLocked[4] = true;
			return true;
		}
		return false;
	}

	void Geometry3D::IB_UnLock( void )
	{
		if (m_IB && m_IsLocked[4])
		{
			m_IB->Unlock();
			m_IsLocked[4] = false;
		}
	}

	void Geometry3D::IB_Cleanup( void )
	{
		IB_UnLock();
		Resource3D::ReleaseIndexBuffer(m_IB);

		m_FaceCount	= 0;
	}
	
	void Geometry3D::UpdateBoundingVolume( void )
	{
		PVertex0 pv = NULL;
		if ( VB_Lock(0, (PVertex&)pv) )
		{
			BoundBox_Calcul(m_Box, pv, m_VrtxCount);
			BoundSph_Calcul(m_Sphere, m_Box);

			VB_UnLock(0);
		}
	}

	FORCEINLINE math::AABox& Geometry3D::BoundingBox( void )
	{
		return m_Box;
	}

	FORCEINLINE math::Sphere& Geometry3D::BoundingSphere( void )
	{
		return m_Sphere;
	}

	FORCEINLINE void Geometry3D::SetToDevice( void )
	{
		m_IsLocked[0] ? Device3D::SetVertexBuffer(0, 0, 0) : Device3D::SetVertexBuffer(0, m_VB[0], SEGAN_SIZE_VERTEX_0);
		m_IsLocked[1] ? Device3D::SetVertexBuffer(1, 0, 0) : Device3D::SetVertexBuffer(1, m_VB[1], SEGAN_SIZE_VERTEX_1);
		m_IsLocked[2] ? Device3D::SetVertexBuffer(2, 0, 0) : Device3D::SetVertexBuffer(2, m_VB[2], SEGAN_SIZE_VERTEX_2);
		m_IsLocked[3] ? Device3D::SetVertexBuffer(3, 0, 0) : Device3D::SetVertexBuffer(3, m_VB[3], SEGAN_SIZE_VERTEX_3);
		m_IsLocked[4] ? Device3D::SetIndexBuffer(NULL) : Device3D::SetIndexBuffer(m_IB);
	}

	FORCEINLINE void Geometry3D::SetToDevice( bool vb0, bool vb1, bool vb2, bool vb3, bool ib )
	{
		if (vb0) m_IsLocked[0] ? Device3D::SetVertexBuffer(0, 0, 0) : Device3D::SetVertexBuffer(0, m_VB[0], SEGAN_SIZE_VERTEX_0);
		if (vb1) m_IsLocked[1] ? Device3D::SetVertexBuffer(1, 0, 0) : Device3D::SetVertexBuffer(1, m_VB[1], SEGAN_SIZE_VERTEX_1);
		if (vb2) m_IsLocked[2] ? Device3D::SetVertexBuffer(2, 0, 0) : Device3D::SetVertexBuffer(2, m_VB[2], SEGAN_SIZE_VERTEX_2);
		if (vb3) m_IsLocked[3] ? Device3D::SetVertexBuffer(3, 0, 0) : Device3D::SetVertexBuffer(3, m_VB[3], SEGAN_SIZE_VERTEX_3);
		if (ib)  m_IsLocked[4] ? Device3D::SetIndexBuffer(NULL) : Device3D::SetIndexBuffer(m_IB);
	}

	FORCEINLINE UINT Geometry3D::GetVertexCount( void )
	{
		return m_VrtxCount;
	}

	FORCEINLINE UINT Geometry3D::GetFaceCount( void )
	{
		return m_FaceCount;
	}

	void Geometry3D::Cleanup( void )
	{
		VB_Cleanup();
		IB_Cleanup();
	}

	FORCEINLINE bool Geometry3D::VB_IsEmpty( int index /*= 0*/ )
	{
		return ( m_VB[index] == NULL );
	}

	FORCEINLINE bool Geometry3D::IB_IsEmpty( void )
	{
		return ( m_IB == NULL );
	}

	void Geometry3D::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, geometry3DFileID);

		int ver = 1;
		SEGAN_STREAM_WRITE(stream, ver);

		GeometryDesc desc; ZeroMemory(&desc, sizeof(GeometryDesc));
		desc.Box = m_Box;
		desc.Sphere = m_Sphere;
		desc.FaceCount[0] = m_FaceCount;
		desc.VertexCount[0] = m_VrtxCount;
		desc.Option = m_VB[3]==NULL ? 0 : SX_GEOMETRY_OPTION_ANIMATED;

		SEGAN_STREAM_WRITE(stream, desc);

		MemoryStream mem;

		//  write each vertex buffer of geometry
		for (int i=0; i<4; i++)
		{
			VB_UnLock(i);	// be sure that geometry is unlocked

			D3DVertexBufferDesc vbdesc;
			UINT n = VB_GetDesc(i, vbdesc) ? vbdesc.Size : 0;

			void* p;
			if (VB_Lock( i, (PD3DVertex&)p ))
			{
				mem.Write(&n, sizeof(UINT));
				mem.Write(p, n);
				VB_UnLock(i);
			}
			else
			{
				n=0;
				mem.Write(&n, sizeof(UINT));
			}
		}

		//  write index buffer to the stream
		IB_UnLock();	// be sure that geometry is unlocked

		D3DIndexBufferDesc ibdesc;
		UINT n = IB_GetDesc(ibdesc) ? ibdesc.Size : 0;

		void* p;
		if ( IB_Lock( (PDWORD&)p ) )
		{
			mem.Write(&n, sizeof(UINT));
			mem.Write(p, n);
			IB_UnLock();
		}
		else
		{
			n=0;
			mem.Write(&n, sizeof(UINT));
		}

		//  compress memory to the stream
		mem.Seek(ST_BEGIN);
		sx::sys::ZCompressStream(mem, stream);
	}

	void Geometry3D::Load( Stream& stream )
	{
		Cleanup();

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if( id != geometry3DFileID )
		{
			sxLog::Log(L"Incompatible file format for loading Geometry3D !");
			return;
		}

		int ver = 0;
		SEGAN_STREAM_READ(stream, ver);
		if ( ver == 1 )
		{

			GeometryDesc desc; ZeroMemory(&desc, sizeof(GeometryDesc));
			SEGAN_STREAM_READ(stream, desc);

			VB_Create( desc.VertexCount[0], SEGAN_SET_HAS(desc.Option, SX_GEOMETRY_OPTION_ANIMATED) );
			IB_Create( desc.FaceCount[0] );

			//  decompress stream to the memory
			MemoryStream mem;
			sx::sys::ZDecompressStream(stream, mem);
			mem.Seek(ST_BEGIN);

			//  load each vertex buffer data from stream to the specified geometry
			for (int i=0; i<4; i++)
			{
				UINT n;
				mem.Read(&n, sizeof(UINT));

				//  check the file rectification
				UINT m = GetVertexCount();
				if(i==0) m *= SEGAN_SIZE_VERTEX_0;
				if(i==1) m *= SEGAN_SIZE_VERTEX_1;
				if(i==2) m *= SEGAN_SIZE_VERTEX_2;
				if(i==3) m *= SEGAN_SIZE_VERTEX_3;

				if (n && n==m)
				{
					VB_UnLock(i);	// be sure that geometry is unlocked

					void* p;
					if ( VB_Lock( i, (PD3DVertex&)p ) )
					{
						mem.Read(p, n);
						VB_UnLock(i);
					}
					else
					{
						Cleanup();
						sxLog::Log(L"Loading vertex buffer for Geometry3D Object has been failed !");
						return;
					}
				}
			}

			//  read index buffer from the stream
			UINT n;
			mem.Read(&n, sizeof(UINT));
			if (n && n==GetFaceCount()*12)
			{
				IB_UnLock(); // be sure that geometry is unlocked

				void* p;
				if ( IB_Lock( (PDWORD&)p ) )
				{
					mem.Read(p, n);
					IB_UnLock();
				}
				else
				{
					sxLog::Log(L"Loading index buffer for Geometry3D Object has been failed !");
					Cleanup();
					return;
				}
			}

		}
	}

	//////////////////////////////////////////////////////////////////////////
	//  STATIC FUNCTIONS
	//////////////////////////////////////////////////////////////////////////
	void Geometry3D::BoundBox_Calcul( OUT AABox& outBox, PD3DVertex0 v, UINT numVertex )
	{
		outBox.Min =  sx::math::VEC3_MAX;
		outBox.Max = -sx::math::VEC3_MAX;

		for (UINT i=0; i<numVertex; i++)
		{
			if ( v->x < outBox.Min.x ) outBox.Min.x = v->x;
			if ( v->y < outBox.Min.y ) outBox.Min.y = v->y;
			if ( v->z < outBox.Min.z ) outBox.Min.z = v->z;

			if ( v->x >= outBox.Max.x ) outBox.Max.x = v->x;
			if ( v->y >= outBox.Max.y ) outBox.Max.y = v->y;
			if ( v->z >= outBox.Max.z ) outBox.Max.z = v->z;

			v++;
		}
	}

	void Geometry3D::BoundSph_Calcul( OUT Sphere& outSph, AABox& inBox )
	{
		outSph.center = inBox.Min + inBox.Max;
		outSph.center *= 0.5f;

		float3 a = inBox.Max - inBox.Min;
		outSph.radius = a.Length() / 2;
	}

	void Geometry3D::BoundSph_Calcul( OUT Sphere& outSph, PD3DVertex0 v, UINT numVertex )
	{
		AABox b;
		BoundBox_Calcul(b, v, numVertex);
		BoundSph_Calcul(outSph, b);

	}

	void Geometry3D::Simplify( IN Geometry3D& InGeometry, OUT Geometry3D& OutGeometry, float Percent, const WCHAR* userLog )
	{
		float fcount = (float)InGeometry.GetFaceCount() *  Percent;
		GeometryTool::Simplify( &InGeometry, &OutGeometry, int(fcount), userLog );
	}

	void Geometry3D::Translate( IN_OUT Geometry3D& geometry, float3 addPos )
	{
		PVertex0 v = NULL;
		if ( geometry.VB_Lock(0, (PVertex&)v) )
		{
			int i=0, n=geometry.GetVertexCount();
			while (i<n)
			{
				v->pos.x += addPos.x;
				v->pos.y += addPos.y;
				v->pos.z += addPos.z;

				v++;
				i++;
			}
			geometry.VB_UnLock(0);
		}
	}

	void Geometry3D::WeldVertices( bool Partial /*= true*/ )
	{
		GeometryTool::WeldVertices(this, Partial);
	}

	void Geometry3D::ComputeNormals( void )
	{
		GeometryTool::ComputeNormals(this);
	}

	void Geometry3D::ComputeTangents( void )
	{
		GeometryTool::ComputeTangents(this);
	}

	void Geometry3D::Optimize( void )
	{
		GeometryTool::Optimize(this);
	}

	void Geometry3D::Weld_Optimize_ComputeNT( bool weldPartial /*= true*/ )
	{
		GeometryTool::Weld_Optimize_ComputeNT(this, weldPartial);
	}


} } // namespace sx { namespace d3d

