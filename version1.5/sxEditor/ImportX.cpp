#include "ImportX.h"
#include "ImportTexture.h"


#define X_HAS_NORMAL	0x00000001
#define X_HAS_TEXCRD0	0x00000002
#define X_HAS_COLOR0	0x00000004
#define X_HAS_COLOR1	0x00000008
#define X_HAS_TANGENT	0x00000010
#define X_HAS_TEXCRD1	0x00000020
#define X_HAS_BLNDWET	0x00000040
#define X_HAS_BLNDINX	0x00000080


struct xMesh
{
	LPD3DXMESH		mesh;
	D3DXMATERIAL*	mtrls;
	DWORD			mtrlsCount;
	DWORD*			adjacency;
	DWORD			option;


	LPD3DXBUFFER buffMaterial;
	LPD3DXBUFFER buffAdjacency;

	xMesh(void): mesh(0), mtrls(0), mtrlsCount(0), adjacency(0), option(0), buffMaterial(0), buffAdjacency(0) {}
	~xMesh(void){ 
		SEGAN_RELEASE_AND_NULL(buffMaterial);
		SEGAN_RELEASE_AND_NULL(buffAdjacency);
		SEGAN_RELEASE_AND_NULL(mesh);
	}
};


bool xmesh_LoadFile( OUT XMesh& _mesh, const WCHAR* fileName )
{
	if ( !sx::sys::FileExist(fileName) )
	{
		sxLog::Log(L"File '%s' could not found!", fileName);
		return false;
	}

	xMesh* mesh = sx_new( xMesh );

	//  load mesh from file
	HRESULT res = D3DXLoadMeshFromXW(
		fileName,
		D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM,
		sx::d3d::Device3D::GetDevice(),
		&mesh->buffAdjacency,
		&mesh->buffMaterial,
		NULL,
		&mesh->mtrlsCount,
		&mesh->mesh);

	if (FAILED(res))
	{
		sxLog::Log(L"Can't load mesh from file '%s'!", fileName);
		return false;
	}

	D3DXWELDEPSILONS epsilones; ZeroMemory(&epsilones, sizeof(epsilones));
	for (int i=0; i<sizeof(epsilones)/4; i++) ((float*)&epsilones)[i] = 0.01f;
	D3DXWeldVertices( 
		mesh->mesh,
		D3DXWELDEPSILONS_WELDPARTIALMATCHES,
		&epsilones,
		(DWORD*)mesh->buffAdjacency->GetBufferPointer(),
		(DWORD*)mesh->buffAdjacency->GetBufferPointer(),
		NULL, NULL);
	
	mesh->mtrls = (D3DXMATERIAL*)mesh->buffMaterial->GetBufferPointer();
	mesh->adjacency = (DWORD*)mesh->buffAdjacency->GetBufferPointer();

	//  retrieve mesh specification
	D3DVERTEXELEMENT9 elem[64]; ZeroMemory(elem, sizeof(elem));
	mesh->mesh->GetDeclaration(elem);
	for (int i=0; i<64; i++)
	{
		if (elem[i].Stream == 0xFF) break;

		switch (elem[i].Usage)
		{
		case D3DDECLUSAGE_NORMAL:		mesh->option |= X_HAS_NORMAL;		break;
		case D3DDECLUSAGE_TANGENT:		mesh->option |= X_HAS_TANGENT;		break;
		case D3DDECLUSAGE_BLENDWEIGHT:	mesh->option |= X_HAS_BLNDWET;		break;
		case D3DDECLUSAGE_BLENDINDICES:	mesh->option |= X_HAS_BLNDINX;		break;
		case D3DDECLUSAGE_COLOR:		mesh->option |= (elem[i].UsageIndex>0 ? X_HAS_COLOR1 : X_HAS_COLOR0);	break;
		case D3DDECLUSAGE_TEXCOORD:		mesh->option |= (elem[i].UsageIndex>0 ? X_HAS_TEXCRD1 : X_HAS_TEXCRD0);	break;
		}
	}

	_mesh = mesh;
	return true;
}

void xmesh_Optimize( XMesh _mesh )
{
	xMesh* mesh = (xMesh*)_mesh;

	LPD3DXMESH oMesh = NULL;
	mesh->mesh->Optimize(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
						 mesh->adjacency, mesh->adjacency, NULL, NULL, &oMesh);
	if (oMesh)
	{
		mesh->mesh->Release();
		mesh->mesh = oMesh;
	}
}

bool xmesh_Filter( XMesh _mesh )
{
	const D3DVERTEXELEMENT9 VrtxDecl[] = {
		{0, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0 },

		{0, 12,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0 },
		{0, 24,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0 },
		{0, 32,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0 },
		{0, 36,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		1 },

		{0, 40,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TANGENT,	0 },
		{0, 52,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	1 },

		{0, 60,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDWEIGHT,	0 },
		{0, 76,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },

		D3DDECL_END()
	};

	xMesh* mesh = (xMesh*)_mesh;

	LPD3DXMESH tmpMesh = NULL;
	HRESULT res = mesh->mesh->CloneMesh(mesh->mesh->GetOptions(), VrtxDecl,	sx::d3d::Device3D::GetDevice(),	&tmpMesh);
	SEGAN_RELEASE(mesh->mesh);

	if ( SUCCEEDED(res) )
	{
		mesh->mesh = tmpMesh;
		return true;
	}
	else return false;
}


bool xmesh_ComputeNormal( XMesh _mesh )
{
	xMesh* mesh = (xMesh*)_mesh;

	//if (mesh->option & X_HAS_NORMAL) return true;
	return SUCCEEDED( D3DXComputeNormals(mesh->mesh, mesh->adjacency) );
}

bool xmesh_ComputeTangent( XMesh _mesh )
{
	xMesh* mesh = (xMesh*)_mesh;
	//if (mesh->option & X_HAS_TANGENT) return true;

	LPD3DXMESH tmpMesh = NULL;
	HRESULT res = D3DXComputeTangentFrameEx(
		mesh->mesh, 
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

	SEGAN_RELEASE_AND_NULL(mesh->mesh);

	if ( SUCCEEDED(res) )
	{
		mesh->mesh = tmpMesh;
		return true;
	}
	else return false;
}

void x_To_geometry3d(xMesh& mesh, D3DXATTRIBUTERANGE& attrib, sx::d3d::PGeometry3D gm)
{
	//  lock source mesh
	PXVertex xv = NULL;
	PFace xf = NULL;
	if ( FAILED( mesh.mesh->LockVertexBuffer(0, (void**)&xv) ) ) return;		
	if ( FAILED( mesh.mesh->LockIndexBuffer(0, (void**)&xf) ) ) return;
	

	bool hasAnimation = (mesh.option & X_HAS_BLNDINX) && (mesh.option & X_HAS_BLNDWET);

	gm->VB_Create(attrib.VertexCount, hasAnimation);
	gm->IB_Create(attrib.FaceCount);

	//  first vertex buffer contain position
	PVertex0 v0 = NULL;
	if ( gm->VB_Lock(0, (PVertex&)v0) )
	{
		//  vertex buffer contain texture coordinates and normals
		PVertex1 v1 = NULL;
		if ( gm->VB_Lock(1, (PVertex&)v1) )
		{
			//  vertex buffer contain normal map info
			PVertex2 v2 = NULL;
			if ( gm->VB_Lock(2, (PVertex&)v2) )
			{
				//  vertex buffer contain animation data
				PVertex3 v3 = NULL;
				if ( gm->VB_Lock(3, (PVertex&)v3) || !hasAnimation )
				{
					//  index buffer contain indices data
					PFace f = NULL;
					if ( gm->IB_Lock(f) )
					{
						//  now traverse in the mesh and extract attributes
						for (UINT i=0; i<attrib.VertexCount; i++)
						{
							v0[i].pos = xv[i + attrib.VertexStart].pos;
							
							v1[i].nrm = xv[i + attrib.VertexStart].nrm;
							v1[i].txc = xv[i + attrib.VertexStart].txc0;
							v1[i].col0 = (mesh.option & X_HAS_COLOR0) ? xv[i + attrib.VertexStart].col0 : 0xffffffff;
							v1[i].col1 = (mesh.option & X_HAS_COLOR1) ? xv[i + attrib.VertexStart].col1 : 0xffffffff;

							v2[i].tng = xv[i + attrib.VertexStart].tng;
							v2[i].txc = xv[i + attrib.VertexStart].txc1;

							if (v3)
							{
								v3[i].bi[0] = xv->bi[0];
								v3[i].bi[1] = xv->bi[1];
								v3[i].bi[2] = xv->bi[2];
								v3[i].bi[3] = xv->bi[3];

								v3[i].bw[0] = xv->bw[0];
								v3[i].bw[1] = xv->bw[1];
								v3[i].bw[2] = xv->bw[2];
								v3[i].bw[3] = xv->bw[3];
							}
						}

						for (UINT i=0; i<attrib.FaceCount; i++)
						{
							f[i].v[0] = xf[i + attrib.FaceStart].v[0] - attrib.VertexStart;
							f[i].v[1] = xf[i + attrib.FaceStart].v[1] - attrib.VertexStart;
							f[i].v[2] = xf[i + attrib.FaceStart].v[2] - attrib.VertexStart;
						}

						gm->IB_UnLock();
					}
					gm->VB_UnLock(3);
				}
				gm->VB_UnLock(2);
			}
			gm->VB_UnLock(1);
		}
		gm->VB_UnLock(0);
	}

	//  unlock source mesh
	mesh.mesh->UnlockVertexBuffer();
	mesh.mesh->UnlockIndexBuffer();

}


bool ImportXToLibrary( const WCHAR* fileName, sx::core::PNode& node )
{
	if ( !sx::sys::FileExist(fileName) )
	{
		sxLog::Log(L"File '%s' could not found!", fileName);
		return false;
	}

#define FAILEDIMPORT(){\
	sxLog::Log(L"Loading mesh from file '%s' failed !", fileName);\
	sx_delete( mesh );\
	return false;}\

	xMesh* mesh = NULL;
	if ( !xmesh_LoadFile((XMesh&)mesh, fileName) )	FAILEDIMPORT();
	if ( !xmesh_Filter(mesh) )						FAILEDIMPORT();
	if ( !xmesh_ComputeNormal(mesh) )				FAILEDIMPORT();
	if ( !xmesh_ComputeTangent(mesh) )				FAILEDIMPORT();
	xmesh_Optimize(mesh);

	//  retrieve number of attributes in the mesh
	DWORD attribCount = 0;
	mesh->mesh->GetAttributeTable(NULL, &attribCount);
	if (attribCount<0) FAILEDIMPORT();
	D3DXATTRIBUTERANGE attrib[512]; ZeroMemory(attrib, sizeof(attrib));
	mesh->mesh->GetAttributeTable(attrib, NULL);

	//  everything is ready to import attributes to the node
	str1024 str = fileName;
	str.ExtractFileName();
	str.ExcludeFileExtension();
	if (!node) 
	{
		node = sx_new( sx::core::Node );
		node->SetName( str );
	}

	// traverse between attributes
	for (DWORD a=0; a<attribCount; a++)
	{
		str.Format(L"%s@part%d", node->GetName(), a);

		//  create new mesh for this attribute
		sx::core::PMesh meshMember = sx_new( sx::core::Mesh );
		meshMember->SetName( str );
		meshMember->SetOwner( node );

		//  extract geometry from mesh and save it
		str << L".gmt";
		sx::d3d::PGeometry geometry = NULL;
		sx::d3d::Geometry::Manager::Create(geometry, NULL);
		x_To_geometry3d( *mesh, attrib[a], geometry->GetRes(0) );
		geometry->GetRes(0)->UpdateBoundingVolume();
		MemoryStream mem;
		geometry->Save(mem);
		sx::sys::FileManager::File_Save(str, SEGAN_PACKAGENAME_GEOMETRY, mem);
		sx::d3d::Geometry::Manager::Release(geometry);

		//  update mesh
		meshMember->SetGeometry(str);
		meshMember->GetMaterial(0)->SetShader(L"default.sfx");

		//  extract material from mesh
		if ( mesh->mtrls[a].pTextureFilename )
		{
			str = mesh->mtrls[a].pTextureFilename;
			str.ExtractFileName();
			str1024	 txFile = fileName;
			txFile.ExtractFilePath();
			txFile << str;

			ImportTexture( txFile );

			txFile.ExtractFileName();
			txFile.ExcludeFileExtension();
			txFile << L".txr";
			meshMember->GetMaterial(0)->SetTexture(0, txFile);
		} 
	}

 	//  translate node to the correct position
  	node->UpdateBoundingVolumes();
 	float3 newPos = - node->GetSphere_local().center;
  
 	sx::core::ArrayPNodeMember meshList;
 	node->GetMembersByType(NMT_MESH, meshList, true);
  	for (int i=0; i<meshList.Count(); i++)
  	{
  		sx::core::PMesh mesh = (sx::core::PMesh)meshList[i];
  
  		sx::d3d::PGeometry gm = NULL;
  		if ( sx::d3d::Geometry::Manager::Create( gm, NULL ) )
  		{
 			PStream file;
 			if (sx::sys::FileManager::File_Open( mesh->GetGeometry(), SEGAN_PACKAGENAME_GEOMETRY, file))
 			{
 				gm->Load( *file );
 				sx::sys::FileManager::File_Close(file);
 
   				sx::d3d::Geometry3D::Translate( *(gm->GetRes(0)), newPos );
				gm->GetRes(0)->Optimize();
				gm->GetRes(0)->ComputeTangents();
   				gm->GetRes(0)->UpdateBoundingVolume();
   				gm->GenerateSubLOD( 0.0f, 0.0f, mesh->GetGeometry() );
 	  
   				//  save geometry data
   				MemoryStream mem;
   				gm->Save(mem);
   				sx::sys::FileManager::File_Save( mesh->GetGeometry(), SEGAN_PACKAGENAME_GEOMETRY, mem);
 			}
 			sx::d3d::Geometry::Manager::Release( gm );
  		}
  	}
 	node->UpdateBoundingVolumes();
	newPos = - newPos;
 	node->SetPosition(newPos);
 	node->UpdateBoundingVolumes();
 
	str = sx::sys::GetTempFolder();
	str.MakePathStyle();
	str <<  node->GetName() << L".node";
	sx::sys::FileStream file;
	if ( file.Open(str, FM_CREATE) )
	{
		node->Save(file);
		file.Close();
	}

	if ( file.Open(str, FM_OPEN_READ | FM_SHARE_READ))
	{
		node->Clear();
		node->Load(file);
		file.Close();
	}
 	return true;
}


