#include "sxGeometry3DTool.h"
#include "sxDevice3D.h"
#include "../sxSystem/sxLog.h"

//  vertex declaration for use in D3DX mesh simplifier function
const D3DVERTEXELEMENT9 d3dxVrtxDecl[] = {
	{0, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0 },
	{0, 12,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,		0 },
	{0, 24,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0 },
	{0, 32,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,			0 },
	{0, 36,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,			1 },
	{0, 40,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TANGENT,		0 },
	{0, 52,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		1 },
	{0, 60,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDWEIGHT,	0 },
	{0, 76,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },
	D3DDECL_END()
};

//  vertex types that use in simplifier function
struct d3dxVertex
{
	Vector3	pos;
	Vector3	nrm;
	Vector2	tx0;
	DWORD	cl0;
	DWORD	cl1;
	Vector3	tgn;
	Vector2	tx1;
	Vector4	blw;	//  blend weights
	DWORD	bli;	//  blend indices
};

namespace sx { namespace d3d 
{

	//////////////////////////////////////////////////////////////////////////
	//  D3DXMESH WRAPPER
	//////////////////////////////////////////////////////////////////////////
	struct d3dxMesh
	{
		LPD3DXMESH	mesh;
		PDWORD		pAdjacency;

		d3dxMesh(): mesh(0), pAdjacency(0) {}

		d3dxMesh(UINT numVert, UINT numFace): mesh(0), pAdjacency(0)
		{
			D3DXCreateMesh( numFace, numVert, D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM, d3dxVrtxDecl, Device3D::GetDevice(),	&mesh );
		}

		d3dxMesh(PGeometry3D gm3D): mesh(0), pAdjacency(0)
		{
			//  create a new D3DXMESH and fill it's buffer
			D3DXCreateMesh(
				gm3D->GetFaceCount(), 
				gm3D->GetVertexCount(), 
				D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM,
				d3dxVrtxDecl,
				Device3D::GetDevice(),
				&mesh
				);
			if ( !mesh ) return;

			//  new fill the new D3DXMESH by content of the in geometry
			if ( !CopyGeometryToD3DXMesh(gm3D) )
			{
				SEGAN_RELEASE_AND_NULL(mesh);
				return;
			}

			//  create Adjacency buffer
			pAdjacency = (PDWORD)sx_mem_alloc( gm3D->GetFaceCount() * sizeof(DWORD) * 4 );

			//  call generate adjacency function to fill the buffer
			if( FAILED( mesh->GenerateAdjacency(EPSILON, pAdjacency) ) )
			{
				sx_mem_free(pAdjacency);
				SEGAN_RELEASE_AND_NULL(mesh);
				return;		
			}	

			//  clean the mesh to prepare it for simplifier
			LPD3DXMESH pTempMesh = NULL;
			if (SUCCEEDED( D3DXCleanMesh(D3DXCLEAN_BACKFACING, mesh, pAdjacency, &pTempMesh, pAdjacency, NULL) ))
			{
				if (mesh != pTempMesh)
				{
					mesh->Release();
					mesh = pTempMesh;
				}
			}
		}

		~d3dxMesh()
		{
			sx_mem_free(pAdjacency);
			SEGAN_RELEASE_AND_NULL(mesh);
		}

		bool CopyGeometryToD3DXMesh(PGeometry3D gm)
		{
			int vCount = (int)gm->GetVertexCount();
			d3dxVertex* vsrc = NULL;
			if ( SUCCEEDED( mesh->LockVertexBuffer(0, (LPVOID*)&vsrc) ) )
			{
				//  at first copy vertex buffer 0
				PD3DVertex0 v0 = NULL;
				if (gm->VB_Lock(0, (PD3DVertex&)v0))
				{
					for(int i=0; i<vCount; i++)
					{
						vsrc[i].pos = v0[i].pos;
					}
					gm->VB_UnLock(0);
				}
				else return false;

				//  now copy vertex buffer 1
				PD3DVertex1 v1 = NULL;
				if (gm->VB_Lock(1, (PD3DVertex&)v1))
				{
					for(int i=0; i<vCount; i++)
					{
						vsrc[i].nrm = v1[i].nrm;
						vsrc[i].tx0 = v1[i].txc;
						vsrc[i].cl0 = v1[i].col0;
						vsrc[i].cl1 = v1[i].col1;
					}
					gm->VB_UnLock(1);
				}
				else return false;

				//  now copy vertex buffer 2
				PD3DVertex2 v2 = NULL;
				if (gm->VB_Lock(2, (PD3DVertex&)v2))
				{
					for(int i=0; i<vCount; i++)
					{
						vsrc[i].tgn = v2[i].tng;
						vsrc[i].tx1 = v2[i].txc;
					}
					gm->VB_UnLock(2);
				}
				else return false;

				//  now copy vertex buffer 3
				PD3DVertex3 v3 = NULL;
				if (gm->VB_Lock(3, (PD3DVertex&)v3))
				{
					for(int i=0; i<vCount; i++)
					{
						memcpy(&vsrc[i].blw, v3[i].bw, 12);
						memcpy(&vsrc[i].bli, v3[i].bi, 4);
					}
					gm->VB_UnLock(3);
				}

				mesh->UnlockVertexBuffer();
			}
			else return false;

			UINT fcount= gm->GetFaceCount();
			PDWORD isrc = NULL;
			if ( SUCCEEDED(mesh->LockIndexBuffer(0, (LPVOID*)&isrc)) )
			{
				PDWORD ind = NULL;
				if ( gm->IB_Lock(ind) )
				{
					memcpy( isrc, ind, fcount * sizeof(DWORD) * 3 );

					gm->IB_UnLock();
				}
				mesh->UnlockIndexBuffer();
			}
			else return false;

			//  prepare mesh to find himself
			PDWORD attrib = NULL;
			if ( SUCCEEDED(mesh->LockAttributeBuffer(0, &attrib)) )
			{
// 				for (UINT i=0; i<fcount; i++)
// 				{
// 					attrib[i]=0;
// 				}
				memset( attrib, 0, fcount * sizeof(DWORD) );

				mesh->UnlockAttributeBuffer();		
			}
			else return false;

			return true;
		}

		bool CopyD3DXMeshToGeometry(PGeometry3D gm, bool animated)
		{
			//  prepare out geometry to fill by new mesh
			const int vCount = (int)mesh->GetNumVertices();

	//		sxLog::Log( L"created vertices : %d and faces : %d", vCount, mesh->GetNumFaces() );

			gm->Cleanup();
			gm->VB_Create( vCount, animated );
			gm->IB_Create( mesh->GetNumFaces() );

			d3dxVertex* vsrc = NULL;
			if ( SUCCEEDED( mesh->LockVertexBuffer(0, (LPVOID*)&vsrc) ) )
			{
				//  at first copy vertex buffer 0
				PD3DVertex0 v0 = NULL;
				if (gm->VB_Lock(0, (PD3DVertex&)v0))
				{
					for(int i=0; i<vCount; i++)
					{
						v0[i].pos = vsrc[i].pos;
					}
					gm->VB_UnLock(0);
				}
				else return false;

				//  now copy vertex buffer 1
				PD3DVertex1 v1 = NULL;
				if (gm->VB_Lock(1, (PD3DVertex&)v1))
				{
					for(int i=0; i<vCount; i++)
					{
						v1[i].nrm  = vsrc[i].nrm;
						v1[i].txc  = vsrc[i].tx0;
						v1[i].col0 = vsrc[i].cl0;
						v1[i].col1 = vsrc[i].cl1;
					}
					gm->VB_UnLock(1);
				}
				else return false;

				//  now copy vertex buffer 2
				PD3DVertex2 v2 = NULL;
				if (gm->VB_Lock(2, (PD3DVertex&)v2))
				{
					for(int i=0; i<vCount; i++)
					{
						v2[i].tng = vsrc[i].tgn;
						v2[i].txc = vsrc[i].tx1;
					}
					gm->VB_UnLock(2);
				}
				else return false;

				//  now copy vertex buffer 3
				PD3DVertex3 v3 = NULL;
				if (gm->VB_Lock(3, (PD3DVertex&)v3))
				{
					for(int i=0; i<vCount; i++)
					{
						memcpy(v3[i].bw, &vsrc[i].blw, 12);
						memcpy(v3[i].bi, &vsrc[i].bli, 4);
					}
					gm->VB_UnLock(3);
				}

				mesh->UnlockVertexBuffer();
			}
			else return false;

			int fsize = gm->GetFaceCount() * 12;
			PDWORD isrc = NULL;
			if ( SUCCEEDED(mesh->LockIndexBuffer(0, (LPVOID*)&isrc)) )
			{
				PDWORD ind = NULL;
				if (gm->IB_Lock(ind))
				{
					memcpy(ind, isrc, fsize);
					gm->IB_UnLock();
				}
				mesh->UnlockIndexBuffer();
			}
			else return false;

			return true;
		}
	};

	
	//////////////////////////////////////////////////////////////////////////
	//	GEOMETRY TOOL
	//////////////////////////////////////////////////////////////////////////
	void GeometryTool::WeldVertices( IN_OUT PGeometry3D pInGeometry, bool weldPartial /*= true*/ )
	{
		if (!Device3D::GetDevice() || !pInGeometry ) return;

		d3dxMesh xMesh(pInGeometry);

		// Perform a weld to try and remove excess vertices.
		// Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
		D3DXWELDEPSILONS Epsilons;
		ZeroMemory( &Epsilons, sizeof(D3DXWELDEPSILONS) );
		Epsilons.Diffuse = 1.0f;
		Epsilons.Specular = 1.0f;

		D3DXWeldVertices(
			xMesh.mesh,
			weldPartial ? D3DXWELDEPSILONS_WELDPARTIALMATCHES : D3DXWELDEPSILONS_WELDALL,
			&Epsilons,
			xMesh.pAdjacency,
			xMesh.pAdjacency,
			NULL,
			NULL
			);

		D3DVertexBufferDesc desc;
		bool animated = pInGeometry->VB_GetDesc(3, desc);
		xMesh.CopyD3DXMeshToGeometry(pInGeometry, animated);
	}

	void GeometryTool::ComputeNormals( IN_OUT PGeometry3D pInGeometry )
	{
		if (!Device3D::GetDevice() || !pInGeometry ) return;

		d3dxMesh xMesh(pInGeometry);

		//  compute normals
		D3DXComputeNormals(xMesh.mesh, xMesh.pAdjacency);

		D3DVertexBufferDesc desc;
		bool animated = pInGeometry->VB_GetDesc(3, desc);
		xMesh.CopyD3DXMeshToGeometry(pInGeometry, animated);

	}

	void GeometryTool::ComputeTangents( IN_OUT PGeometry3D pInGeometry )
	{
		if (!Device3D::GetDevice() || !pInGeometry ) return;

		d3dxMesh xMesh(pInGeometry);

		//  compute tangent
		LPD3DXMESH tmpMesh = NULL;
		HRESULT res = D3DXComputeTangentFrameEx(
			xMesh.mesh, 
			D3DDECLUSAGE_TEXCOORD,
			0,
			D3DDECLUSAGE_TANGENT,
			0,
			D3DX_DEFAULT,
			0,
			D3DDECLUSAGE_NORMAL,
			0,
			0,
			NULL,
			-1,
			0,
			-1,
			&tmpMesh,
			NULL);

		if ( SUCCEEDED(res) )
		{
			SEGAN_RELEASE_AND_NULL(xMesh.mesh);
			xMesh.mesh = tmpMesh;
		}

		D3DVertexBufferDesc desc;
		bool animated = pInGeometry->VB_GetDesc(3, desc);
		xMesh.CopyD3DXMeshToGeometry(pInGeometry, animated);

	}

	void GeometryTool::Optimize( IN_OUT PGeometry3D pInGeometry )
	{
		if (!Device3D::GetDevice() || !pInGeometry ) return;

		d3dxMesh xMesh(pInGeometry);

		//  optimize geometry by reorders faces to remove unused vertices and faces and reorders faces to increase the cache hit rate of vertex caches
		xMesh.mesh->OptimizeInplace(
			D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_DEVICEINDEPENDENT,
			xMesh.pAdjacency,
			xMesh.pAdjacency,
			NULL,
			NULL
			);

		D3DVertexBufferDesc desc;
		bool animated = pInGeometry->VB_GetDesc(3, desc);
		xMesh.CopyD3DXMeshToGeometry(pInGeometry, animated);
	}

	void GeometryTool::Weld_Optimize_ComputeNT( IN_OUT PGeometry3D pInGeometry, bool weldPartial /*= true*/ )
	{
		if (!Device3D::GetDevice() || !pInGeometry ) return;

		d3dxMesh xMesh(pInGeometry);
		if ( !xMesh.mesh ) return;

		// Perform a weld to try and remove excess vertices.
		// Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
		D3DXWELDEPSILONS Epsilons;
		ZeroMemory( &Epsilons, sizeof(D3DXWELDEPSILONS) );
		Epsilons.Diffuse = 1.0f;
		Epsilons.Specular = 1.0f;

		D3DXWeldVertices(
			xMesh.mesh,
			weldPartial ? D3DXWELDEPSILONS_WELDPARTIALMATCHES : D3DXWELDEPSILONS_WELDALL,
			&Epsilons,
			xMesh.pAdjacency,
			xMesh.pAdjacency,
			NULL,
			NULL
			);

		//  optimize geometry by reorders faces to remove unused vertices and faces and reorders faces to increase the cache hit rate of vertex caches
		xMesh.mesh->OptimizeInplace(
			D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_DEVICEINDEPENDENT,
			xMesh.pAdjacency,
			xMesh.pAdjacency,
			NULL,
			NULL
			);

		//  compute normals
		D3DXComputeNormals(xMesh.mesh, xMesh.pAdjacency);

		//  compute tangent
		LPD3DXMESH tmpMesh = NULL;
		HRESULT res = D3DXComputeTangentFrameEx(
			xMesh.mesh, 
			D3DDECLUSAGE_TEXCOORD,
			0,
			D3DDECLUSAGE_TANGENT,
			0,
			D3DX_DEFAULT,
			0,
			D3DDECLUSAGE_NORMAL,
			0,
			0,
			NULL,
			-1,
			0,
			-1,
			&tmpMesh,
			NULL);
		if ( SUCCEEDED(res) )
		{
			SEGAN_RELEASE_AND_NULL(xMesh.mesh);
			xMesh.mesh = tmpMesh;
		}

		D3DVertexBufferDesc desc;
		bool animated = pInGeometry->VB_GetDesc(3, desc);
		xMesh.CopyD3DXMeshToGeometry(pInGeometry, animated);
	}

	void GeometryTool::Simplify( IN PGeometry3D pInGeometry, OUT PGeometry3D pOutGeometry, UINT MinFace, const WCHAR* userLog)
	{
 		if ( !Device3D::GetDevice() || !pOutGeometry || !pInGeometry ) return;

		if ( !MinFace )
		{
			pOutGeometry->Cleanup();
			return;
		}

 		if ( MinFace < 32 )
 			MinFace = UINT( (float)pInGeometry->GetFaceCount() * 0.25f );
 		
		if ( MinFace < 32 )
		{
			pOutGeometry->Cleanup();
			return;
		}
	 
		d3dxMesh xMesh(pInGeometry);

		String strLog;
		strLog.Format( L"Preparing to simplify geometry [ %s ] \r\n", userLog );

 		//  clean the mesh to prepare it for simplifier
		LPD3DXBUFFER pErrors = NULL;
 		LPD3DXMESH pTempMesh = NULL;
 		if (SUCCEEDED( D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION, xMesh.mesh, xMesh.pAdjacency, &pTempMesh, xMesh.pAdjacency, &pErrors) ))
 		{
 			if (xMesh.mesh != pTempMesh)
 			{
 				SEGAN_RELEASE_AND_NULL(xMesh.mesh);
 				xMesh.mesh = pTempMesh;
 			}

			strLog << L"Geometry cleaned successfully.\r\n";
 		}
		else if ( pErrors )
		{
			strLog << L"Cleaning geometry failed du to \r\n" << (char*)pErrors->GetBufferPointer();
			SEGAN_RELEASE_AND_NULL(pErrors);
		}
	 
 		// Verify validity of mesh for simplification
 		if( FAILED( D3DXValidMesh( xMesh.mesh, xMesh.pAdjacency, &pErrors ) ) && pErrors )
 		{
			strLog << L"Validating geometry failed du to \r\n" << (char*)pErrors->GetBufferPointer();
			SEGAN_RELEASE_AND_NULL(pErrors);
 		}
		else
		{
			strLog << L"Geometry validated successfully.\r\n";
		}
	 
		
		//  call the D3DX simplifier function to create a simplified mesh
		LPD3DXMESH outMesh = NULL;
 		strLog << L"calling SimplifyMesh() for generate " ;//<< (int)MinFace << L" faces from " << (int)pInGeometry->GetFaceCount() << L" faces.\r\n";
		HRESULT hres = D3DXSimplifyMesh(xMesh.mesh, xMesh.pAdjacency, NULL, NULL, (DWORD)MinFace, D3DXMESHSIMP_FACE, &outMesh);
 		if ( SUCCEEDED( hres ) && outMesh )
		{
			strLog << L"Simplifying geometry successfully done with ";// << (int)outMesh->GetNumFaces() << L" faces from " << (int)xMesh.mesh->GetNumFaces() << L" faces.\r\n";
			SEGAN_RELEASE_AND_NULL( xMesh.mesh );
			xMesh.mesh = outMesh;
		}
		else
		{
			strLog << L"Simplifying geometry failed.\r\n";
		}
	 
		D3DVertexBufferDesc desc;
		bool animated = pInGeometry->VB_GetDesc(3, desc);
		xMesh.CopyD3DXMeshToGeometry(pOutGeometry, animated);

		sxLog::Log( strLog );
	}


} } // namespace sx { namespace d3d