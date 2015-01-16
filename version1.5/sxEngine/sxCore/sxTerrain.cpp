#include "sxTerrain.h"
#include "sxNode.h"
#include "sxSettings.h"
#include "../sxCommon/sxCommon.h"
#include "../sxRender/sxRender.h"

#define TER_W		33			//  width of the terrain
#define TER_Q		32			//  number of quads
#define TER_D		1.0f		//  divide 1m*m square to sub square
#define TER_LOD		6			//  number of terrain LOD
#define TER_RAD		22.62741f	//  radius of each chunk

typedef Array<sx::core::PMaterialMan>	TerrainMatList;

//  vertex types that use in terrain patch
typedef struct TerrainVertex
{
	Vector3	pos;
	Vector3	nrm;
	Vector2	tx0;
	DWORD	cl0;
	DWORD	cl1;
	Vector3	tgn;
	Vector2	tx1;

} *PTerrainVertex;

typedef struct TerrainChunk
{
	UINT		vertStep;
	UINT		vertRow;
	UINT		faceCount;
	PD3DFace	faces;

	TerrainChunk(): vertStep(0), vertRow(0), faceCount(0), faces(0) {}
	~TerrainChunk() { sx_mem_free(faces); };

	void allocFaces(UINT numFace) {
		sx_mem_free( faces );
		faceCount = numFace;
		if ( faceCount>0 )
		{
			faces = (PD3DFace)sx_mem_alloc( faceCount * sizeof(D3DFace) );
			ZeroMemory( faces, faceCount * sizeof(D3DFace));
		}
		else faces = null;
	}

} *PTerrainChunk;

typedef struct TerrainLOD
{
	TerrainChunk	center;
	TerrainChunk	cornerFUL[4];		//  standard corners
	TerrainChunk	cornerLOD[4];		//  simplified corners

} *PTerrainLOD;

//  variables which used for manager
static Frustum					s_cameraFrsutum;			//  frustum of camera
static float					s_cameraFOV;				//  cos( FOV ) of camera
static float					s_LOD_Threshold = 6.0f;		//  threshold of changing LOD
static TerrainMatList			s_materials;					//  material group


//  some static variables
static const UINT TerrainFileID = MAKEFOURCC('T', 'E', 'R', 'R');
static sx::d3d::Geometry3D		s_terGeometry;				//  geometry for rendering terrain
static TerrainChunk				s_terFullIndices;			//  full indices of terrain use to intersection test
static TerrainLOD				s_terChunk[TER_LOD];		//  indices of sub LOD of terrain
static UINT						s_TerrainPartCount = 0;		//  number of terrain parts in the scene

//  data used for patch
static bool				s_patchUsed				= false;
static UINT				s_patchVertexCount		= 0;	
static UINT				s_patchFaceCount		= 0;
static PTerrainVertex	s_patchBuffVerts		= 0;
static PD3DFace			s_patchBuffFaces		= 0;
static int				s_patchBuffVertsSize	= 0;
static int				s_patchBuffFacesSize	= 0;


void CreatePatchBuffer(int numVert, int numFace)
{
	if ( !numVert && !numFace )
	{
		//  clear buffer
		sx_mem_free( s_patchBuffVerts );	s_patchBuffVerts = null;
		sx_mem_free( s_patchBuffFaces );	s_patchBuffFaces = null;

		s_patchBuffVertsSize = 0;
		s_patchBuffFacesSize = 0;
	}

	if ( numVert > s_patchBuffVertsSize )
	{
		numVert += (TER_W * TER_W) * 10;
		sx_mem_realloc( s_patchBuffVerts, numVert * sizeof(TerrainVertex) );
		s_patchBuffVertsSize = numVert;
	}

	if ( numFace > s_patchBuffFacesSize )
	{
		numFace += (TER_W * TER_W * 2) * 10;
		sx_mem_realloc( s_patchBuffFaces, numFace * sizeof(D3DFace) );
		s_patchBuffFacesSize = numFace;
	}

}

void AppendPatchIndexBuffer(TerrainChunk& chunk)
{
	if ( chunk.faceCount<0 ) return;

	//  allocate memory for patch faces
	CreatePatchBuffer(0, s_patchFaceCount + chunk.faceCount);

	//  copy faces
	for (UINT i=0; i<chunk.faceCount; i++)
	{
		PD3DFace tf = &( chunk.faces )[i];
		PD3DFace bf = &s_patchBuffFaces[s_patchFaceCount + i];

		int k = tf->v[0];
		int l = k / TER_W;
		int m = k % TER_W;
		bf->v[0] = s_patchVertexCount + (l * chunk.vertRow + m)/chunk.vertStep;

		k = tf->v[1];
		l = k / TER_W;
		m = k % TER_W;
		bf->v[1] = s_patchVertexCount + (l * chunk.vertRow + m)/chunk.vertStep;

		k = tf->v[2];
		l = k / TER_W;
		m = k % TER_W;
		bf->v[2] = s_patchVertexCount + (l * chunk.vertRow + m)/chunk.vertStep;
	}

	s_patchFaceCount += chunk.faceCount;
}


void TerrainInitializer()
{
	const int quadCountFull	= (TER_W-1);

	//  create full indices used for test intersection
	{
		const int faceCount	= int( quadCountFull * quadCountFull * 2 );
		s_terFullIndices.allocFaces( faceCount );

		int index = 0;
		PFace f = s_terFullIndices.faces;

		for ( int i=0; i<quadCountFull; i++ )
		{
			for ( int j=0; j<quadCountFull; j++ )
			{
				int baseUp = ( i * TER_W ) + j;
				int baseDn = ( (i+1) * TER_W ) + j;

				f[index].v[0] = baseUp;
				f[index].v[1] = baseUp + 1;
				f[index].v[2] = baseDn;
				index++;

				f[index].v[0] = baseDn;
				f[index].v[1] = baseUp + 1;
				f[index].v[2] = baseDn + 1;
				index++;

			}
		}
	}
	
	//  create terrain chunks used for rendering chunk
	for (int level=0; level<TER_LOD; level++)
	{

		//  fill main data of chunk
		if ( level == TER_LOD-1 )
		{
			s_terChunk[level].center.vertRow	= 2;
			s_terChunk[level].center.vertStep	= 32;
			s_terChunk[level].center.allocFaces( 2 );

			s_terChunk[level].center.faces[0].v[0] = 0;
			s_terChunk[level].center.faces[0].v[1] = quadCountFull;
			s_terChunk[level].center.faces[0].v[2] = quadCountFull * TER_W;

			s_terChunk[level].center.faces[1].v[0] = quadCountFull * TER_W;
			s_terChunk[level].center.faces[1].v[1] = quadCountFull;
			s_terChunk[level].center.faces[1].v[2] = quadCountFull * TER_W + quadCountFull;

		}
		else
		{
			//  compute this chunk properties
			const int vertStep		= sx::cmn::Power( 2, level );
			const int quadCountLOD	= (quadCountFull / vertStep) - 2;
			const int vertRow		= quadCountLOD + 3;

			//  update chunk properties
			s_terChunk[level].center.allocFaces( int( quadCountLOD * quadCountLOD * 2 ) );
			s_terChunk[level].center.vertStep	= vertStep;
			s_terChunk[level].center.vertRow	= vertRow;

			for (int i=0; i<4; i++)
			{
				s_terChunk[level].cornerFUL[i].allocFaces( int( (quadCountLOD+1) * 2 ) );
				s_terChunk[level].cornerFUL[i].vertStep	= vertStep;
				s_terChunk[level].cornerFUL[i].vertRow	= vertRow;

				s_terChunk[level].cornerLOD[i].allocFaces( quadCountLOD+1 );
				s_terChunk[level].cornerLOD[i].vertStep	= vertStep;
				s_terChunk[level].cornerLOD[i].vertRow	= vertRow;
			}
			
			int ic=0, fi0=0, fi1=0, fi2=0, fi3=0, li0=0, li1=0, li2=0, li3=0;
			PD3DFace fc = s_terChunk[level].center.faces;

 			PD3DFace f0 = s_terChunk[level].cornerFUL[0].faces;
 			PD3DFace f1 = s_terChunk[level].cornerFUL[1].faces;
 			PD3DFace f2 = s_terChunk[level].cornerFUL[2].faces;
 			PD3DFace f3 = s_terChunk[level].cornerFUL[3].faces;

			PD3DFace l0 = s_terChunk[level].cornerLOD[0].faces;
			PD3DFace l1 = s_terChunk[level].cornerLOD[1].faces;
			PD3DFace l2 = s_terChunk[level].cornerLOD[2].faces;
			PD3DFace l3 = s_terChunk[level].cornerLOD[3].faces;

			for ( int i=0; i<quadCountFull; i+=vertStep )
			{
				for ( int j=0; j<quadCountFull; j+=vertStep )
				{
					int vertUp		=  i * TER_W + j;
					int vertDn		=  (i+vertStep) * TER_W + j;
					int vertUpLeft	= vertUp + vertStep;
					int vertDnLeft	= vertDn + vertStep;

					//  fill FULL detail indices
 					if ( i==0 && j==0 )								//  top left corner
 					{
 						f0[fi0].v[0] = vertUp;
 						f0[fi0].v[1] = vertUpLeft;
 						f0[fi0].v[2] = vertDnLeft;
 						fi0++;
 
 						f3[fi3].v[0] = vertUp;
 						f3[fi3].v[1] = vertDnLeft;
 						f3[fi3].v[2] = vertDn;
 						fi3++;
 					}
 					else if ( i==0 && j==quadCountFull-vertStep )	//  top right corner
 					{
 						f0[fi0].v[0] = vertUp;
 						f0[fi0].v[1] = vertUpLeft;
 						f0[fi0].v[2] = vertDn;
 						fi0++;
 
 						f1[fi1].v[0] = vertDn;
 						f1[fi1].v[1] = vertUpLeft;
 						f1[fi1].v[2] = vertDnLeft;
 						fi1++;
 					}
 					else if ( i==quadCountFull-vertStep && j==0 )	//  bottom left corner
 					{
 						f2[fi2].v[0] = vertDn;
 						f2[fi2].v[1] = vertUpLeft;
 						f2[fi2].v[2] = vertDnLeft;
 						fi2++;
 
 						f3[fi3].v[0] = vertUp;
 						f3[fi3].v[1] = vertUpLeft;
 						f3[fi3].v[2] = vertDn;
 						fi3++;
 					}
 					else if ( i==quadCountFull-vertStep && j==quadCountFull-vertStep )	//  bottom right corner
 					{
 						f2[fi2].v[0] = vertUp;
 						f2[fi2].v[1] = vertDnLeft;
 						f2[fi2].v[2] = vertDn;
 						fi2++;
 
 						f1[fi1].v[0] = vertUp;
 						f1[fi1].v[1] = vertUpLeft;
 						f1[fi1].v[2] = vertDnLeft;
 						fi1++;
 					}
 					else if ( i==0 && j>0 )							//  top side
 					{
  						f0[fi0].v[0] = vertUp;
  						f0[fi0].v[1] = vertUpLeft;
  						f0[fi0].v[2] = vertDn;
  						fi0++;
  
  						f0[fi0].v[0] = vertDn;
  						f0[fi0].v[1] = vertUpLeft;
  						f0[fi0].v[2] = vertDnLeft;
  						fi0++;
 					}
 					else if ( i==quadCountFull-vertStep && j>0 )	// bottom side
					{
						f2[fi2].v[0] = vertUp;
						f2[fi2].v[1] = vertUpLeft;
						f2[fi2].v[2] = vertDn;
						fi2++;

						f2[fi2].v[0] = vertDn;
						f2[fi2].v[1] = vertUpLeft;
						f2[fi2].v[2] = vertDnLeft;
						fi2++;
					}
					else if ( i>0 && j==0 )							//	left side
 					{
  						f3[fi3].v[0] = vertUp;
  						f3[fi3].v[1] = vertUpLeft;
  						f3[fi3].v[2] = vertDn;
  						fi3++;
  
  						f3[fi3].v[0] = vertDn;
  						f3[fi3].v[1] = vertUpLeft;
  						f3[fi3].v[2] = vertDnLeft;
  						fi3++;
 					}
					else if ( i>0 && j==quadCountFull-vertStep )	//  right side
					{
						f1[fi1].v[0] = vertUp;
						f1[fi1].v[1] = vertUpLeft;
						f1[fi1].v[2] = vertDn;
						fi1++;

						f1[fi1].v[0] = vertDn;
						f1[fi1].v[1] = vertUpLeft;
						f1[fi1].v[2] = vertDnLeft;
						fi1++;
					}					
					else if ( i>0 && j>0 && i<quadCountFull-vertStep && j<quadCountFull-vertStep )
					{
						fc[ic].v[0] = vertUp;
						fc[ic].v[1] = vertUpLeft;
						fc[ic].v[2] = vertDn;
						ic++;

						fc[ic].v[0] = vertDn;
						fc[ic].v[1] = vertUpLeft;
						fc[ic].v[2] = vertDnLeft;
						ic++;

					}

					//  fill LOD detail indices
 					int vertUpleftleft	= vertUpLeft + vertStep;
 					int vertDnleftleft	= vertDnLeft + vertStep;
 					int vertDnDn		= (i+vertStep+vertStep) * TER_W + j;
					int vertDnDnLeft	= vertDnDn + vertStep;
 
 					if ( i==0 && li0<=quadCountLOD )		//  top side
 					{
						int evemOdd = j / vertStep;
 						if ( evemOdd%2==0 )
 						{
 							l0[li0].v[0] = vertUp;
 							l0[li0].v[1] = vertUpleftleft;
 							l0[li0].v[2] = vertDnLeft;
 							li0++;
 						}
						else
						{
							l0[li0].v[0] = vertDn;
							l0[li0].v[1] = vertUpLeft;
							l0[li0].v[2] = vertDnleftleft;
							li0++;
						}
 					}

					if ( i==quadCountFull-vertStep && li2<=quadCountLOD )		//  bottom side
					{
						int evemOdd = j / vertStep;
						if ( evemOdd%2!=0 )
						{
							l2[li2].v[0] = vertUp;
							l2[li2].v[1] = vertUpleftleft;
							l2[li2].v[2] = vertDnLeft;
							li2++;
						}
						else
						{
							l2[li2].v[0] = vertDn;
							l2[li2].v[1] = vertUpLeft;
							l2[li2].v[2] = vertDnleftleft;
							li2++;
						}
					}

					if ( j==0 && li3<=quadCountLOD )		//  left side
					{
						int evemOdd = i / vertStep;
						if ( evemOdd%2==0 )
						{
							l3[li3].v[0] = vertUp;
							l3[li3].v[1] = vertDnLeft;
							l3[li3].v[2] = vertDnDn;
							li3++;
						}
						else
						{
 							l3[li3].v[0] = vertUpLeft;
 							l3[li3].v[1] = vertDnDnLeft;
 							l3[li3].v[2] = vertDn;
 							li3++;
						}
					}

					if ( j==quadCountFull-vertStep && li1<=quadCountLOD )		//  right side
					{
						int evemOdd = i / vertStep;
						if ( evemOdd%2!=0 )
						{
							l1[li1].v[0] = vertUp;
							l1[li1].v[1] = vertDnLeft;
							l1[li1].v[2] = vertDnDn;
							li1++;
						}
						else
						{
							l1[li1].v[0] = vertUpLeft;
							l1[li1].v[1] = vertDnDnLeft;
							l1[li1].v[2] = vertDn;
							li1++;
						}
					}

				}
			}
			
		}//  fill main data of chunk


	}

}

void TerrainFinalizer()
{
	s_terFullIndices.allocFaces(0);

	//  clear levels
	for (int level=0; level<TER_LOD; level++)
	{
		s_terChunk[level].center.allocFaces(0);

		for (int i=0; i<4; i++)
		{
			s_terChunk[level].cornerFUL[i].allocFaces(0);
			s_terChunk[level].cornerLOD[i].allocFaces(0);
		}
	}

	//  clear buffer
	CreatePatchBuffer(0, 0);

	// clear API geometry buffer
	s_terGeometry.Cleanup();

	//  invalidate material textures
	for (int i=0; i<s_materials.Count(); i++)
	{
		s_materials[i]->Invalidate();
	}
}


namespace sx { namespace core {

	
	void CreateBuffer(int vertNum, int faceNum)
	{
		if ( vertNum > (int)s_terGeometry.GetVertexCount() )
		{
			vertNum += (TER_W * TER_W) * 10;
			s_terGeometry.VB_CreateByIndex( vertNum, 0, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
			s_terGeometry.VB_CreateByIndex( vertNum, 1, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
			s_terGeometry.VB_CreateByIndex( vertNum, 2, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
		}
		
		if ( faceNum > (int)s_terGeometry.GetFaceCount() )
		{
			faceNum += (TER_W * TER_W * 2) * 10;
			s_terGeometry.IB_Create( faceNum, SX_RESOURCE_DYNAMIC | SX_RESOURCE_CHECKLOST );
		}
	}

	Terrain::Terrain() : NodeMember(), m_GroupID(0)
	{
		m_Type = NMT_TERRAIN;
		m_Option = SX_MESH_SELECTABLE | SX_MESH_CASTSHADOW | SX_MESH_RECIEVESHADOW;
		
		const float Wdiv2 = (float)(TER_W-1)*TER_D*0.5f;
		m_pVertex0 = (PVertex0)sx_mem_alloc( TER_W*TER_W*sizeof(Vertex0) );
		m_pVertex1 = (PVertex1)sx_mem_alloc( TER_W*TER_W*sizeof(Vertex1) );
		m_pVertex2 = (PVertex2)sx_mem_alloc( TER_W*TER_W*sizeof(Vertex2) );

		for (int i=0; i<TER_W; i++)
		{
			for (int j=0; j<TER_W; j++)
			{
				int index = i*TER_W + j;

				m_pVertex0[index].pos = float3((float)i*TER_D - Wdiv2, 0, (float)j*TER_D-Wdiv2);
				m_pVertex1[index].nrm = float3(0, 1, 0);
				m_pVertex1[index].txc = float2((float)i/(float)TER_Q, (float)j/(float)TER_Q);
				m_pVertex1[index].col0 = 0xffffffff;
				m_pVertex1[index].col1 = 0xffffffff;
				m_pVertex2[index].tng = float3(0, 0, 1);
				m_pVertex2[index].txc = float2((float)i/(float)TER_Q, (float)j/(float)TER_Q);
			}
		}
		
		m_Box.Min.Set( -Wdiv2, -0.1f, -Wdiv2);
		m_Box.Max.Set( Wdiv2, 0.1f, Wdiv2);
		m_Sphere.ComputeByAABox(m_Box);

		s_TerrainPartCount++;
		if ( s_TerrainPartCount == 1 )
			TerrainInitializer();
	}

	Terrain::~Terrain()
	{
		s_TerrainPartCount--;
		if ( s_TerrainPartCount == 0 )
			TerrainFinalizer();			
	}
	
	FORCEINLINE void Terrain::SetOwner( PNode pOwner )
	{
		NodeMember::SetOwner( pOwner );

		//  update bounding volumes of owner
		if (m_Owner) m_Owner->UpdateBoundingVolumes();
	}

	void Terrain::Clear( void )
	{

	}

	void Terrain::Validate( int level )
	{
		for (int i=0; i<s_materials.Count(); i++)
		{
			s_materials[i]->Validate(0);
		}
	}

	void Terrain::Invalidate( void )
	{

	}

	void Terrain::Update( float elpsTime )
	{

	}

	void Terrain::Draw( DWORD flag )
	{
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
				d3d::Device3D::Matrix_World_Set(m_Owner->GetMatrix_world());

				if ( flag & SX_DRAW_WIRED)
					d3d::UI3D::DrawWiredSphere( m_Sphere, 0x00203f3f * ((flag & SX_DRAW_SELECTED) != 0) + 0xffafbfbf );
				else
					d3d::UI3D::DrawSphere( m_Sphere, 0x00203f3f * ((flag & SX_DRAW_SELECTED) != 0) + 0xffafbfbf );
			}
		}

	}

	void Terrain::DrawByViewParam( DWORD flag, float viewParam )
	{
		if ( !(flag & SX_DRAW_TERRAIN) || (m_Option & SX_MESH_INVISIBLE) || (flag & SX_DRAW_ALPHA) ) return;

		const int level	= ComputeLODValue( viewParam );

		//  verify that we are patching terrain nodes
		if ( s_patchUsed )
		{
			//  copy required faces to the path buffer
			{
				//  copy faces of center
				AppendPatchIndexBuffer( s_terChunk[level].center );

				//  predict which patch most be used
				float perDis = TER_W-1;

				// top
				float3 neighborPos = m_Owner->GetPosition_world() - float3(perDis,0,0);
				int neghborLevel = ComputeLODValue( cmn::ViewDistanceByFrustom( s_cameraFrsutum, s_cameraFOV, neighborPos, TER_RAD ) );
				if ( neghborLevel > level)
 					AppendPatchIndexBuffer( s_terChunk[level].cornerLOD[0] );
				else
					AppendPatchIndexBuffer( s_terChunk[level].cornerFUL[0] );

 				// bottom
 				neighborPos = m_Owner->GetPosition_world() + float3(perDis,0,0);
				neghborLevel = ComputeLODValue( cmn::ViewDistanceByFrustom( s_cameraFrsutum, s_cameraFOV, neighborPos, TER_RAD ) );
				if ( neghborLevel > level)
 					AppendPatchIndexBuffer( s_terChunk[level].cornerLOD[2] );
 				else
 					AppendPatchIndexBuffer( s_terChunk[level].cornerFUL[2] );
 
 				// left
 				neighborPos = m_Owner->GetPosition_world() - float3(0,0,perDis);
				neghborLevel = ComputeLODValue( cmn::ViewDistanceByFrustom( s_cameraFrsutum, s_cameraFOV, neighborPos, TER_RAD ) );
				if ( neghborLevel > level)
 					AppendPatchIndexBuffer( s_terChunk[level].cornerLOD[3] );
 				else
 					AppendPatchIndexBuffer( s_terChunk[level].cornerFUL[3] );
 
 				// right
 				neighborPos = m_Owner->GetPosition_world() + float3(0,0,perDis);
				neghborLevel = ComputeLODValue( cmn::ViewDistanceByFrustom( s_cameraFrsutum, s_cameraFOV, neighborPos, TER_RAD ) );
				if ( neghborLevel > level)
 					AppendPatchIndexBuffer( s_terChunk[level].cornerLOD[1] );
 				else
 					AppendPatchIndexBuffer( s_terChunk[level].cornerFUL[1] );

			} //  copy required faces to the path buffer


			//  copy required vertices to the path buffer
			{
				const int vertStep	= sx::cmn::Power( 2, level );
				const int vertRow	= (TER_W-1) / vertStep + 1;

				CreatePatchBuffer(s_patchVertexCount + vertRow * vertRow, 0);

				//  copy vertices to patch buffer
				for ( int i=0; i<TER_W; i += vertStep )
				{
					for ( int j=0; j<TER_W; j += vertStep )
					{
						int k = i * TER_W + j;
						int l = (i * vertRow + j)/vertStep + s_patchVertexCount;

						s_patchBuffVerts[l].pos.x	= m_pVertex0[k].pos.x + m_Owner->GetPosition_world().x;
						s_patchBuffVerts[l].pos.y	= m_pVertex0[k].pos.y + m_Owner->GetPosition_world().y;
						s_patchBuffVerts[l].pos.z	= m_pVertex0[k].pos.z + m_Owner->GetPosition_world().z;

						s_patchBuffVerts[l].nrm		= m_pVertex1[k].nrm;
						s_patchBuffVerts[l].tx0		= m_pVertex1[k].txc;
						s_patchBuffVerts[l].cl0		= m_pVertex1[k].col0;
						s_patchBuffVerts[l].cl1		= m_pVertex1[k].col1;

						s_patchBuffVerts[l].tgn		= m_pVertex2[k].tng;
						s_patchBuffVerts[l].tx1		= m_pVertex2[k].txc;
					}
				}

				s_patchVertexCount	+= vertRow * vertRow;

			}//  copy required vertices to the path buffer

		}
		
	}

	UINT Terrain::MsgProc( UINT msgType, void* data )
	{
		switch (msgType)
		{
		case MT_ACTIVATE:
			{
				Validate( 0 );
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
				if ( !SEGAN_SET_HAS(ray->type, NMT_TERRAIN) || !SEGAN_SET_HAS(m_Option, SX_MESH_SELECTABLE) ) return msgType;
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

				// intersection to box happened. now we should traverse Terrain triangles to find exact point
				float3 v0, v1, v2;
				PVertex0 v = m_pVertex0;
				PFace f = s_terFullIndices.faces;
				const Matrix& mat = m_Owner->GetMatrix_world();

				for (UINT i=0; i<s_terFullIndices.faceCount; i++)
				{
					//important : now we have to transform vertices by animated / static matrix of this Terrain ?
					// well currently we just test with static Terrains
					v0.Transform( v[f[i].v0].pos, mat );
					v1.Transform( v[f[i].v1].pos, mat );
					v2.Transform( v[f[i].v2].pos, mat );

					if ( ray->ray.Intersect_Triangle( v0, v1, v2, &hitPoint, &hitNorm ) )
					{
						float dis = hitPoint.Distance( ray->ray.pos );
						ray->pushToResults(dis, hitPoint, hitNorm, m_Owner, this);
					}
				}// for (int i=0; i<cFace ...
			}
			break;
		}

		return msgType;
	}

	void Terrain::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, TerrainFileID);

		// write version
		int version = 1;
		SEGAN_STREAM_WRITE(stream, version);

		//  save default things
		NodeMember::Save( stream );

		// write bounding box and sphere
		SEGAN_STREAM_WRITE(stream, m_Box);
		SEGAN_STREAM_WRITE(stream, m_Sphere);
	}

	void Terrain::Load( Stream& stream )
	{
		Clear();

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != TerrainFileID)
		{
			sxLog::Log(L"Incompatible file format for loading Terrain !");
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
		}
	}

	FORCEINLINE int Terrain::ComputeLODValue( float viewParam )
	{
		if ( s_LOD_Threshold < 4.0f ) s_LOD_Threshold = 4.0f;
		int lod = int( sqrt( viewParam * s_LOD_Threshold ) / s_LOD_Threshold );
		SEGAN_CLAMP(lod, 0, TER_LOD-1);
		return lod;
	}

	UINT Terrain::GetVertexCount( void )
	{
		return TER_W*TER_W;
	}

	UINT Terrain::GetFaceCount( void )
	{
		return s_terFullIndices.faceCount;
	}

	PVertex Terrain::GetVertices( int index /*= 0*/ )
	{
		switch (index)
		{
		case 0: return m_pVertex0;
		case 1: return m_pVertex1;
		case 2: return m_pVertex2;
		}
		return NULL;
	}

	const PFace Terrain::GetFaces( void )
	{
		return s_terFullIndices.faces;
	}

	//////////////////////////////////////////////////////////////////////////
	//	MANAGER
	//////////////////////////////////////////////////////////////////////////
	void Terrain::Manager::BeginPatch( const Frustum& cameraFrustum, const float cameraFOV )
	{
		s_cameraFrsutum = cameraFrustum;
		s_cameraFOV = cameraFOV;

		s_patchUsed = true;
		s_patchVertexCount = 0;
		s_patchFaceCount = 0;

		if( s_materials.Count() < 1 )
		{
			//  create one default material group
			s_materials.PushBack( sx_new( sx::core::MaterialMan ) );
		}
	}

	void Terrain::Manager::EndPatch( DWORD flag )
	{
		s_patchUsed = false;

		if ( s_patchFaceCount && s_patchVertexCount )
		{
			CreateBuffer(s_patchVertexCount, s_patchFaceCount);

			PVertex0 v0 = NULL;
			if ( s_terGeometry.VB_Lock(0, (PVertex&)v0) )
			{
				PVertex1 v1 = NULL;
				if ( s_terGeometry.VB_Lock(1, (PVertex&)v1) )
				{
					PVertex2 v2 = NULL;
					if ( s_terGeometry.VB_Lock(2, (PVertex&)v2) )
					{
						PD3DFace f = NULL;
						if ( s_terGeometry.IB_Lock(f) )
						{
							//  copy vertices to API geometry
							for (UINT i=0; i<s_patchVertexCount; i++)
							{
								v0[i].pos = s_patchBuffVerts[i].pos;
								v1[i].nrm = s_patchBuffVerts[i].nrm;
								v1[i].txc = s_patchBuffVerts[i].tx0;
								v1[i].col0 = s_patchBuffVerts[i].cl0;
								v1[i].col1 = s_patchBuffVerts[i].cl1;
								v2[i].tng = s_patchBuffVerts[i].tgn;
								v2[i].txc = s_patchBuffVerts[i].tx1;
							}

							// copy faces to API geometry
							for (UINT i=0; i<s_patchFaceCount; i++)
							{
								f[i] = s_patchBuffFaces[i];
							}
						
							s_terGeometry.IB_UnLock();
						}
						s_terGeometry.VB_UnLock(2);
					}
					s_terGeometry.VB_UnLock(1);
				}
				s_terGeometry.VB_UnLock(0);
			}

			s_terGeometry.SetToDevice();

			//  draw base of terrain
			{
				d3d::Device3D::Matrix_World_Set( math::MTRX_IDENTICAL );
				s_materials[0]->Get(0)->SetOption( SX_MATERIAL_CULLING | SX_MATERIAL_ZENABLE /*| SX_MATERIAL_OPTION_ZWRITING*/ );
				s_materials[0]->Get(0)->SetToDevice(flag);
				d3d::Device3D::DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, s_patchVertexCount, 0, s_patchFaceCount );
			}			

			//  draw other layer
			if ( s_materials[0]->Count() > 1 )
			{
				Matrix matWorld = math::MTRX_IDENTICAL;		matWorld._42 = 0.01f;
				d3d::Device3D::Matrix_World_Set( matWorld );

				for (int i=1; i<s_materials[0]->Count(); i++)
				{
					s_materials[0]->Get(i)->SetOption( SX_MATERIAL_CULLING | SX_MATERIAL_ZENABLE | SX_MATERIAL_ALPHABLEND );
					s_materials[0]->Get(i)->SetToDevice(flag);
					d3d::Device3D::DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, s_patchVertexCount, 0, s_patchFaceCount );
				}
			}

		}
	}

	float& Terrain::Manager::LOD_Threshold( void )
	{
		return s_LOD_Threshold;
	}

	sx::core::PMaterialMan Terrain::Manager::GetMaterial( int index )
	{
		if ( index < 0 || index >= s_materials.Count() ) return NULL;
		return s_materials[index];
	}

	void Terrain::Manager::ClearrAll( void )
	{
		//  invalidate material textures
		for (int i=0; i<s_materials.Count(); i++)
		{
			sx_delete_and_null( s_materials[i] );
		}
		s_materials.Clear();
	}

	void Terrain::Manager::Save( Stream& strm )
	{
		SEGAN_STREAM_WRITE(strm, TerrainFileID);

		// write version
		int version = 2;
		SEGAN_STREAM_WRITE(strm, version);

		//  save materials
		int n = s_materials.Count();
		SEGAN_STREAM_WRITE(strm, n);

		for (int i=0; i<n; i++)
			s_materials[i]->Save(strm);

		// save LOD threshold
		SEGAN_STREAM_WRITE(strm, s_LOD_Threshold);

	}

	void Terrain::Manager::Load( Stream& strm )
	{
		sx_callstack();

		ClearrAll();

		UINT id = 0;
		SEGAN_STREAM_READ(strm, id);
		if (id != TerrainFileID)
		{
			sxLog::Log(L"Incompatible file format for loading Terrain extra info!");
			return;
		}

		// read version
		int version = 0;
		SEGAN_STREAM_READ(strm, version);

		if ( version >= 1 )
		{
			//  read materials
			int n = 0;
			SEGAN_STREAM_READ(strm, n);

			for (int i=0; i<n; i++)
			{
				PMaterialMan mtrl = sx_new( sx::core::MaterialMan );
				mtrl->Load(strm);
				s_materials.PushBack( mtrl );
			}
		}

		if ( version >= 2 )
		{
			// read LOD threshold
			SEGAN_STREAM_READ(strm, s_LOD_Threshold);
		}

	}
}}  //  namespace sx { namespace core {