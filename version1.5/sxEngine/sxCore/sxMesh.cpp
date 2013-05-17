#include "sxMesh.h"
#include "sxNode.h"
#include "../sxCommon/sxCommon.h"
#include "../sxRender/sxRender.h"

/*
NOTE:	in this class I use a member named 'reserved' to do some operation.
		first byte of reserved used to store validation level contain 0,1,2 ...
		second byte of reserved used to store validation count. validation count must be less that 3

		the other bytes will describe later
*/
#define VALIDATION_LEVEL	SEGAN_1TH_BYTEOF(reserved)
#define VALIDATION_COUNT	SEGAN_2TH_BYTEOF(reserved)
#define VALIDATION_NONE		0xff


//  some static variables
static const UINT meshFileID = MAKEFOURCC('M', 'E', 'S', 'H');

//  data used for patch
static sx::math::Matrix			s_patchMatrix[50];						//  matrices of patch

static PDirect3DVertexBuffer	s_patchVGAVB_pos			= 0;		//  VGA memory vertex buffer for storing position
static PDirect3DVertexBuffer	s_patchVGAVB_ind			= 0;		//  VGA memory vertex buffer for storing indices
static PDirect3DIndexBuffer		s_patchVGAIB				= 0;		//  VGA memory index buffer for storing faces
static UINT						s_patchVGAVertCount			= 0;		//  number of vertices in VGA patch buffer
static UINT						s_patchVGAFaceCount			= 0;		//  number of faces in VGA patch buffer

static pfloat					s_patchSYSVB_pos			= 0;		//  system memory vertex buffer for storing position
static PBYTE					s_patchSYSVB_ind			= 0;		//  system memory vertex buffer for storing indices
static PFace					s_patchSYSIB				= 0;		//  system memory index buffer for storing faces
static UINT						s_patchSYSVertCount			= 0;		//  number of vertices in system patch buffer
static UINT						s_patchSYSFaceCount			= 0;		//  number of faces in system patch buffer

static UINT						s_patchCount				= 0;		//  number of patch must be less that 51
static bool						s_patchUsed					= false;	//	verify to use patch system

//  some management variables
static int						s_meshCount = 0;
static float					s_minimumVolume = 0.15f;
static float					s_lodRange = 4.5;


//////////////////////////////////////////////////////////////////////////
//	STATIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////
void CreateSYSPatchBuffer(UINT numVerts, UINT numFaces)
{
	if ( numVerts > s_patchSYSVertCount )
	{
		s_patchSYSVertCount = numVerts;
		sx_mem_realloc( s_patchSYSVB_pos, numVerts );
		sx_mem_realloc( s_patchSYSVB_ind, numVerts );
	}

	if ( numFaces > s_patchSYSFaceCount )
	{
		s_patchSYSFaceCount = numFaces;
		sx_mem_realloc( s_patchSYSIB, numFaces * 12 );
		sx::d3d::Resource3D::ReleaseIndexBuffer( s_patchVGAIB );
		sx::d3d::Resource3D::CreateIndexBuffer( numFaces * 12, s_patchVGAIB, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
	}
}

void CreateVGAPatchBuffer(UINT numVerts, UINT numFaces)
{
	if ( numVerts > s_patchVGAVertCount )
	{
		s_patchVGAVertCount = numVerts;
		sx::d3d::Resource3D::ReleaseVertexBuffer( s_patchVGAVB_pos );
		sx::d3d::Resource3D::CreateVertexBuffer( numVerts * SEGAN_SIZE_VERTEX_0, s_patchVGAVB_pos, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
		sx::d3d::Resource3D::ReleaseVertexBuffer( s_patchVGAVB_ind );
		sx::d3d::Resource3D::CreateVertexBuffer( numVerts, s_patchVGAVB_ind, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
	}

	if ( numFaces > s_patchVGAFaceCount )
	{
		s_patchVGAFaceCount = numFaces;
		sx::d3d::Resource3D::ReleaseIndexBuffer( s_patchVGAIB );
		sx::d3d::Resource3D::CreateIndexBuffer( numFaces * 12, s_patchVGAIB, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
	}
}

void DestroyPatchBuffers(void)
{
	s_patchSYSFaceCount = 0;
	s_patchSYSVertCount = 0;
	sx_mem_free( s_patchSYSVB_pos );	s_patchSYSVB_pos = null;
	sx_mem_free( s_patchSYSVB_ind );	s_patchSYSVB_ind = null;
	sx_mem_free( s_patchSYSIB );		s_patchSYSIB = null;

	s_patchVGAFaceCount = 0;
	s_patchVGAVertCount = 0;
	sx::d3d::Resource3D::ReleaseVertexBuffer( s_patchVGAVB_pos );
	sx::d3d::Resource3D::ReleaseVertexBuffer( s_patchVGAVB_ind );
	sx::d3d::Resource3D::ReleaseIndexBuffer( s_patchVGAIB );
}

namespace sx { namespace core {

	Mesh::Mesh() : NodeMember()
		, m_scale(1,1,1)
		, m_Geometry(NULL)
		, reserved(0)
	{
		m_Type = NMT_MESH;
		m_Option = SX_MESH_SELECTABLE | SX_MESH_CASTSHADOW | SX_MESH_RECIEVESHADOW;
		
		m_Box.Zero();
		m_Sphere.Zero();

		VALIDATION_LEVEL = VALIDATION_NONE;
		VALIDATION_COUNT = 0;

		s_meshCount++;
	}

	Mesh::~Mesh()
	{
		sx_callstack();

		Clear();

		s_meshCount--;
		if ( s_meshCount == 0 )
			DestroyPatchBuffers();
	}

	FORCEINLINE void Mesh::SetOwner( PNode pOwner )
	{
		NodeMember::SetOwner( pOwner );

		//  update bounding volumes of owner
		if (m_Owner) m_Owner->UpdateBoundingVolumes();

	}

	void Mesh::Clear( void )
	{
		Invalidate();

		m_Material.Clear();

		d3d::Geometry::Manager::Release(m_Geometry);

		m_scale.Set( 1, 1, 1 );
		m_Box.Zero();
		m_Sphere.Zero();
		
		//  update bounding volumes of owner
		if (m_Owner) m_Owner->UpdateBoundingVolumes();
	}

	void Mesh::Validate( int level )
	{
		if ( m_Geometry && level < VALIDATION_LEVEL )
		{
			VALIDATION_LEVEL = level;
			VALIDATION_COUNT += 1;

			m_Geometry->Activate(level);
			m_Material.Validate(level);
		}
	}

	void Mesh::Invalidate( void )
	{
		if ( m_Geometry && VALIDATION_LEVEL != VALIDATION_NONE )
		{
			VALIDATION_LEVEL = VALIDATION_NONE;
			while (VALIDATION_COUNT)
			{
				m_Geometry->Deactivate();
				m_Material.Invalidate();

				VALIDATION_COUNT -= 1;
			}
		}
	}

	void Mesh::Update( float elpsTime )
	{

	}

	void Mesh::Draw( DWORD flag )
	{
		if ( !m_Geometry ) return;

		DWORD matoption = m_Material.GetActiveMaterial()->GetOption();
		bool flagAlpha = SEGAN_SET_HAS( flag, SX_DRAW_ALPHA );
		bool mtrlAlpha = SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHABLEND ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAADD ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAMUL );
		if ( !(m_Option & SX_MESH_INVISIBLE) && flag & SX_DRAW_MESH && flagAlpha==mtrlAlpha )
		{
			int lod = m_Geometry->SetToDevice(0);
			if ( lod > -1 )
			{
				//  check mesh options
				if ( !(m_Option & SX_MESH_RECIEVESHADOW) )
					SEGAN_SET_REM( flag, SX_SHADER_SHADOW );

				Matrix matScale;
				matScale.Scale( m_scale.x, m_scale.y, m_scale.z );
				matScale.Multiply( matScale, m_Owner->GetMatrix_world() );
				d3d::Device3D::Matrix_World_Set( matScale );
				m_Material.SetToDevice(flag);
				d3d::Device3D::DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_Geometry->GetVertexCount(lod), 0, m_Geometry->GetFaceCount(lod));
			}
		}

		//  additional draw
		if ( flag & SX_DRAW_DEBUG )
		{
			sx::d3d::Device3D::RS_Alpha( 0 );

			if ( flag & SX_DRAW_BOUNDING_BOX )
			{
				d3d::Device3D::Matrix_World_Set(m_Owner->GetMatrix_world());

				if ( flag & SX_DRAW_WIRED)
					d3d::UI3D::DrawWiredAABox( m_Box, 0x00203f3f * ((flag & SX_DRAW_SELECTED) != 0) + 0xffafbfbf );
				else
					d3d::UI3D::DrawAABox( m_Box, 0x00203f3f * ((flag & SX_DRAW_SELECTED) != 0) + 0xffafbfbf );
			}

			if ( flag & SX_DRAW_BOUNDING_SPHERE )
			{
				if ( flag & SX_DRAW_WIRED)
				{
					//d3d::UI3D::DrawWiredSphere( m_Sphere, 0x00203f3f * ((flag & SX_DRAW_SELECTED) != 0) + 0xffafbfbf );
					Matrix matView;
					d3d::Device3D::Matrix_View_Get( matView );
					matView.Inverse( matView );
					Sphere worldSphere;
					worldSphere.Transform( m_Sphere, m_Owner->GetMatrix_world() );
					Matrix matScale;
					matScale.Scale( m_scale.x, m_scale.y, m_scale.z );
					matView.Multiply( matScale, matView );
					matView.SetTranslation(worldSphere.x, worldSphere.y, worldSphere.z );
					d3d::UI3D::DrawCircle( matView, m_Sphere.r, 0x00203f3f * ((flag & SX_DRAW_SELECTED) != 0) + 0xffafbfbf );
				}
				else
				{
					Matrix matScale;
					matScale.Scale( m_scale.x, m_scale.y, m_scale.z );
					matScale.Multiply( matScale, m_Owner->GetMatrix_world() );
					d3d::Device3D::Matrix_World_Set( matScale );
					d3d::UI3D::DrawSphere( m_Sphere, 0x00203f3f * ((flag & SX_DRAW_SELECTED) != 0) + 0xffafbfbf );
				}
			}
		}

	}

	void Mesh::DrawLOD( DWORD flag, int LOD )
	{
		if ( !m_Geometry || (m_Option & SX_MESH_INVISIBLE) ) return;

		DWORD matoption = m_Material.GetActiveMaterial()->GetOption();
		bool flagAlpha = SEGAN_SET_HAS( flag, SX_DRAW_ALPHA );
		bool mtrlAlpha = SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHABLEND ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAADD ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAMUL );

		if ( flag & SX_DRAW_MESH && flagAlpha==mtrlAlpha )
		{
			int lod = m_Geometry->SetToDevice( LOD );
			if ( lod > -1 )
			{
				//  check mesh options
				if ( !(m_Option & SX_MESH_RECIEVESHADOW) )
					SEGAN_SET_REM( flag, SX_SHADER_SHADOW );

				Matrix matScale;
				matScale.Scale( m_scale.x, m_scale.y, m_scale.z );
				matScale.Multiply( matScale, m_Owner->GetMatrix_world() );
				d3d::Device3D::Matrix_World_Set( matScale );
				m_Material.SetToDevice(flag);
				d3d::Device3D::DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_Geometry->GetVertexCount(lod), 0, m_Geometry->GetFaceCount(lod));
			}
		}
	}

	void Mesh::DrawByViewParam( DWORD flag, float viewParam )
	{
		//  verify the flag
		if ( !(flag & SX_DRAW_MESH) )		return;
		if ( m_Option & SX_MESH_INVISIBLE ) return;

		//  verify material option
		DWORD matoption = m_Material.GetActiveMaterial()->GetOption();
		bool flagAlpha = SEGAN_SET_HAS( flag, SX_DRAW_ALPHA );
		bool mtrlAlpha = SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHABLEND ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAADD ) || SEGAN_SET_HAS(matoption, SX_MATERIAL_ALPHAMUL );
		if ( flagAlpha != mtrlAlpha ) return;
		
		//  verify geometry size
		if ( !m_Geometry )	return;
		float r = m_Geometry->GetBoundingSphere().r * 2.0f;
		r /= viewParam > EPSILON ? viewParam : EPSILON;
		if ( r < s_minimumVolume ) return;
		
		//  verify geometry LOD
		int lod = ComputeLodValue(viewParam);
		lod = m_Geometry->Validate(lod);
		if ( lod < 0 ) return;
		
		//  verify to use patching system
		if ( s_patchUsed )
		{
			//  append mesh to the patch
			d3d::PGeometry3D gm = m_Geometry->GetRes(lod);

			PVertex0 v0 = NULL;
			if ( gm->VB_Lock(0, (PVertex&)v0, D3DLOCK_READONLY) )
			{
				PD3DFace fc = NULL;
				if ( gm->IB_Lock(fc, D3DLOCK_READONLY) )
				{
					UINT vcount = gm->GetVertexCount();
					UINT fcount = gm->GetFaceCount();
					UINT voffset = s_patchSYSVertCount;
					UINT foffset = s_patchSYSFaceCount;

					//  enlarge buffers
					CreateSYSPatchBuffer( voffset + vcount, foffset + fcount );
					pfloat sysVP = &s_patchSYSVB_pos[voffset];
					PBYTE  sysVI = &s_patchSYSVB_ind[voffset];
					PFace  sysFC = &s_patchSYSIB[foffset];

					//  copy buffers
					memcpy( sysVP, v0, vcount * SEGAN_SIZE_VERTEX_0 );
					memcpy( sysFC, fc, fcount * sizeof(Face) );
					memset( sysFC, s_patchCount, vcount );

					s_patchCount++;

					gm->IB_UnLock();
				}

				gm->VB_UnLock(0);
			}
		}
		else 	//  draw this mesh normally
		{
			Matrix matScale;
			matScale.Scale( m_scale.x, m_scale.y, m_scale.z );
			matScale.Multiply( matScale, m_Owner->GetMatrix_world() );
			d3d::Device3D::Matrix_World_Set( matScale );

			//  check mesh options
			if ( !(m_Option & SX_MESH_RECIEVESHADOW) )
				SEGAN_SET_REM( flag, SX_SHADER_SHADOW );

			//  set geometry to device
			if ( flag & SX_SHADER_ZDEPTH )
				m_Geometry->SetToDevice(lod, true, true, false, true, true);
			else
				m_Geometry->SetToDevice(lod);

			//  set material to device
			m_Material.SetToDevice(flag);

			//  draw triangles
			d3d::Device3D::DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_Geometry->GetVertexCount(lod), 0, m_Geometry->GetFaceCount(lod));
		}
	}

	UINT Mesh::MsgProc( UINT msgType, void* data )
	{
		switch (msgType)
		{
		case MT_ACTIVATE:
			{
				float d = *((float*)data);
				Validate( ComputeLodValue(d) );
			}
			break;

		case MT_DEACTIVATE:
			{
				Invalidate();
			}
			break;

		case MT_COVERBOX:
			{
				(static_cast<PAABox>(data))->CoverAA( m_Box );
			}
			break;

		case MT_GETBOX_LOCAL:
			{
				*(static_cast<PAABox>(data)) = m_Box;
				return 0;
			}
			break;

		case MT_GETBOX_WORLD:
			{
				(static_cast<POBBox>(data))->Transform( m_Box, m_Owner->GetMatrix_world() );
				return 0;
			}
			break;

		case MT_GETSPHERE_LOCAL:
			{
				*(static_cast<PSphere>(data)) = m_Sphere;
				return 0;
			}
			break;

		case MT_GETSPHERE_WORLD:
			{
				(static_cast<PSphere>(data))->Transform( m_Sphere, m_Owner->GetMatrix_world() );
				return 0;
			}
			break;

		case MT_INTERSECT_RAY:
			{
				// test ray intersection and return quickly if test failed
				msg_IntersectRay* ray = static_cast<msg_IntersectRay*>(data);
				if ( !SEGAN_SET_HAS(ray->type, NMT_MESH) || !SEGAN_SET_HAS(m_Option, SX_MESH_SELECTABLE) ) return msgType;
				if ( ray->name && m_Name != ray->name )	return msgType;

				float3 hitPoint, hitNorm;

				//  test sphere
				Sphere sphere;
				if (m_Owner)	sphere.Transform( m_Sphere, m_Owner->GetMatrix_world() );
				else			sphere = m_Sphere;
				if ( !ray->ray.Intersect_Sphere( sphere, &hitPoint, &hitNorm ) ) return msgType;
				if ( ray->depthLevel == msg_IntersectRay::SPHERE )
				{
					float dis = hitPoint.Distance( ray->ray.pos );
					ray->pushToResults(dis, hitPoint, hitNorm, m_Owner, this);
					return msgType;
				}

				//  test box
				OBBox box;
				if (m_Owner)	box.Transform( m_Box, m_Owner->GetMatrix_world() );
				else			box.OBBox::OBBox(m_Box);
				if ( !ray->ray.Intersect_OBBox( box, &hitPoint, &hitNorm ) ) return msgType;
				if ( ray->depthLevel == msg_IntersectRay::BOX )
				{
					float dis = hitPoint.Distance( ray->ray.pos );
					ray->pushToResults(dis, hitPoint, hitNorm, m_Owner, this);
					return msgType;
				}

				// intersection to box happened. now we should traverse mesh triangles to find exact point
				if ( !m_Geometry )
				{
					sxLog::Log( L"Mesh [%s] has no geometry !!" , GetName() );
					return msgType;
				}
				int lod = m_Geometry->Validate( 0 );
				d3d::PGeometry3D gm = ( lod >= 0 ) ? m_Geometry->GetRes( lod ) : NULL;
				
				PVertex0 v = NULL;
				if ( gm && gm->VB_Lock(0, (PVertex&)v) )
				{
					PFace f = NULL;
					if ( gm->IB_Lock(f) )
					{
						float3 v0, v1, v2;
						int cFace = gm->GetFaceCount();
						Matrix mat;
						mat.Scale( m_scale.x, m_scale.y, m_scale.z );
						mat.Multiply( mat, m_Owner->GetMatrix_world() );

						for (int i=0; i<cFace; i++)
						{
							//important : now we have to transform vertices by animated / static matrix of this mesh ?
							// well currently we just test with static meshes
							v0.Transform( v[ f[i].v0 ].pos, mat );
							v1.Transform( v[ f[i].v1 ].pos, mat );
							v2.Transform( v[ f[i].v2 ].pos, mat );

							if ( ray->ray.Intersect_Triangle( v0, v1, v2, &hitPoint, &hitNorm ) )
							{
								float dis = ray->ray.pos.Distance( hitPoint );
								ray->pushToResults(dis, hitPoint, hitNorm, m_Owner, this);
							}
						}// for (int i=0; i<cFace ...

						gm->IB_UnLock();
					}
					gm->VB_UnLock(0);
				}
			}
			break;

		case MT_MESH:
			{
				msg_Mesh *msgMesh = static_cast<msg_Mesh*>(data);

				if ( msgMesh->name && m_Name != msgMesh->name )	return msgType;

				if ( msgMesh->remOption & SX_MESH_INVISIBLE )		RemOption( SX_MESH_INVISIBLE );
				if ( msgMesh->remOption & SX_MESH_SELECTABLE )		RemOption( SX_MESH_SELECTABLE );
				if ( msgMesh->remOption & SX_MESH_CASTSHADOW )		RemOption( SX_MESH_CASTSHADOW );
				if ( msgMesh->remOption & SX_MESH_RECIEVESHADOW )	RemOption( SX_MESH_RECIEVESHADOW );

				if ( msgMesh->addOption & SX_MESH_INVISIBLE )		AddOption( SX_MESH_INVISIBLE );
				if ( msgMesh->addOption & SX_MESH_SELECTABLE )		AddOption( SX_MESH_SELECTABLE );
				if ( msgMesh->addOption & SX_MESH_CASTSHADOW )		AddOption( SX_MESH_CASTSHADOW );
				if ( msgMesh->addOption & SX_MESH_RECIEVESHADOW )	AddOption( SX_MESH_RECIEVESHADOW );

				if ( msgMesh->matIndex > -1 )
					SetActiveMaterial( msgMesh->matIndex );
				else
					msgMesh->matIndex = m_Material.GetActiveMaterialIndex();

				msgMesh->material = GetActiveMaterial();

			}
			break;

		case MT_MESH_COUNT:
			{
				msg_Mesh_Count *msgmeshes = static_cast<msg_Mesh_Count*>(data);

				if ( msgmeshes->name && m_Name != msgmeshes->name )	return msgType;

				msgmeshes->meshes[msgmeshes->numMeshes] = this;
				msgmeshes->numMeshes++;
			}
			break;
		}

		return msgType;
	}

	void Mesh::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, meshFileID);

		// write version
		int version = 3;
		SEGAN_STREAM_WRITE(stream, version);

		//  save default things
		NodeMember::Save( stream );

		// write bounding box and sphere
		SEGAN_STREAM_WRITE(stream, m_Box);
		SEGAN_STREAM_WRITE(stream, m_Sphere);

		// write geometry resource
		String str;
		if (m_Geometry)	str = m_Geometry->GetSource();
		cmn::String_Save(str, &stream);
		
		// write materials and their contents
		m_Material.Save(stream);

		// version 3
		SEGAN_STREAM_WRITE(stream, m_scale);
		
	}

	void Mesh::Load( Stream& stream )
	{
		//Clear();

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != meshFileID)
		{
			sxLog::Log(L"Incompatible file format for loading mesh !");
			return;
		}

		// read version
		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version == 1)
		{
			//  load default things
			NodeMember::Load( stream );

			// read bounding box and sphere
			SEGAN_STREAM_READ(stream, m_Box);
			SEGAN_STREAM_READ(stream, m_Sphere);

			// read geometry resource
			String str;
			cmn::String_Load(str, &stream);
			SetGeometry(str);		

			// read number of materials and their contents
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			m_Material[0]->Load(stream);
			for (int i=1; i<n; i++)
				m_Material.Add()->Load(stream);
			
			//  finally read the active material index
			int ActiveMaterial = 0;
			SEGAN_STREAM_READ(stream, ActiveMaterial);
			m_Material.SetActiveMaterial(ActiveMaterial);
		}

		if (version == 2)
		{
			//  load default things
			NodeMember::Load( stream );

			// read bounding box and sphere
			SEGAN_STREAM_READ(stream, m_Box);
			SEGAN_STREAM_READ(stream, m_Sphere);

			// read geometry resource
			String str;
			cmn::String_Load(str, &stream);
			SetGeometry(str);		

			// read materials and their contents
			m_Material.Load(stream);
			
		}

		if (version == 3)
		{
			//  load default things
			NodeMember::Load( stream );

			// read bounding box and sphere
			SEGAN_STREAM_READ(stream, m_Box);
			SEGAN_STREAM_READ(stream, m_Sphere);

			// read geometry resource
			String str;
			cmn::String_Load(str, &stream);
			SetGeometry(str);		

			// read materials and their contents
			m_Material.Load(stream);

			SEGAN_STREAM_READ(stream, m_scale);
			SetScale( m_scale.x, m_scale.y, m_scale.z );
		}
	}

	void Mesh::SetGeometry( const WCHAR* srcAddress )
	{
		if (srcAddress && m_Geometry && (wcscmp(m_Geometry->GetSource(), srcAddress) == 0)) 
			return;

		d3d::PGeometry GM = NULL;
		if (d3d::Geometry::Manager::Get(GM, srcAddress))
		{
			if (VALIDATION_LEVEL != VALIDATION_NONE)
			{
				for (int i=0; i<VALIDATION_COUNT; i++)
				{
					GM->Activate( VALIDATION_LEVEL );
				}
			}
		}

		if (m_Geometry)
		{
			if (VALIDATION_LEVEL != VALIDATION_NONE)
			{
				for (int i=0; i<VALIDATION_COUNT; i++)
				{
					m_Geometry->Deactivate();
				}
			}

			d3d::Geometry::Manager::Release(m_Geometry);
		}
		m_Geometry = GM;

		//  update the owners bounding volumes
		if (m_Geometry)
		{
			m_Box =		m_Geometry->GetBoundingBox();
			m_Sphere =	m_Geometry->GetBoundingSphere();
		}
		else
		{
			m_Box.Zero();
			m_Sphere.Zero();
		}

		if (m_Owner) m_Owner->UpdateBoundingVolumes();
	}

	FORCEINLINE MaterialMan& Mesh::Material( void )
	{
		return m_Material;
	}

	FORCEINLINE const WCHAR* Mesh::GetGeometry( void )
	{
		if (m_Geometry)
			return m_Geometry->GetSource();
		else
			return NULL;
	}

	FORCEINLINE sx::d3d::PMaterial Mesh::AddMaterial( void )
	{
		return m_Material.Add();
	}

	FORCEINLINE void Mesh::RemoveMaterial( int index )
	{
		m_Material.Remove(index);
	}

	FORCEINLINE sx::d3d::PMaterial Mesh::GetMaterial( int index )
	{
		return m_Material.Get(index);
	}

	FORCEINLINE int Mesh::GetMaterialCount( void )
	{
		return m_Material.Count();
	}

	FORCEINLINE void Mesh::SetActiveMaterial( int index )
	{
		m_Material.SetActiveMaterial(index);
	}

	FORCEINLINE int Mesh::GetActiveMaterialIndex( void )
	{
		return m_Material.GetActiveMaterialIndex();
	}

	FORCEINLINE d3d::PMaterial Mesh::GetActiveMaterial( void )
	{
		return m_Material.GetActiveMaterial();
	}

	FORCEINLINE int Mesh::ComputeLodValue( float viewParam )
	{
		if ( !m_Geometry ) return 0;

		float lodStep = s_lodRange * m_Geometry->GetBoundingSphere().r / 3;

		return ( viewParam < lodStep ? 0 : ( viewParam < lodStep*3 ? 1 : 2 ) );
	}

	void Mesh::SetScale( const float x, const float y, const float z )
	{
		m_scale.Set( x, y, z );

		if ( m_Geometry )
		{
			m_Box = m_Geometry->GetBoundingBox();
			m_Sphere =	m_Geometry->GetBoundingSphere();
		}
		else
		{
			m_Box.Zero();
			m_Sphere.Zero();
		}

		m_Box.Max.x *= x;
		m_Box.Max.y *= y;
		m_Box.Max.z *= z;
		m_Box.Min.x *= x;
		m_Box.Min.y *= y;
		m_Box.Min.z *= z;
		m_Sphere.radius *= sx_max_3f( x, y, z );

		if ( m_Owner )
			m_Owner->UpdateBoundingVolumes();
	}


	//////////////////////////////////////////////////////////////////////////
	//	MESH MANAGER
	//////////////////////////////////////////////////////////////////////////
	void Mesh::Manager::BeginPatch( DWORD flag )
	{

	}

	void Mesh::Manager::EndPatch( DWORD flag )
	{

	}

	float& Mesh::Manager::MinimumVolume( void )
	{
		return s_minimumVolume;
	}

	float& Mesh::Manager::LODRange( void )
	{
		return s_lodRange;
	}

}}  //  namespace sx { namespace core {