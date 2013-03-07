#include "ImportOBJ.h"
#include "ImportTexture.h"


//////////////////////////////////////////////////////////////////////////
//		 SOME GLOBAL VARIABLES
int								g_meshCount = 0;
ImportModelOptions				g_Options;
String							g_FileName;

//  main containers
Array<float3>			g_arrVP;		//	array of vertex positions
Array<float3>			g_arrVN;		//	array of vertex normals
Array<float2>			g_arrVT;		//  array of vertex texture coordinates


struct objVertex
{
	pfloat3	pos;
	pfloat3	nrm;
	pfloat2	tex;

	UINT GetKey(void){

		char c[64];
		memcpy( &c[0], pos, 12 );
		memcpy( &c[12], nrm, 12 );
		memcpy( &c[24], tex, 8 );
		c[32] = 0;
		c[33] = 0;
		c[34] = 0;
		c[35] = 0;

		for ( int i=0; i<32; i++ )
		{
			if( c[i] == 0 )
				c[i] = 1;
		}

		return sx::cmn::GetCRC32( (WCHAR*)c );
	}

	bool operator==(objVertex& vrtx){
		return pos == vrtx.pos && nrm == vrtx.nrm && tex == vrtx.tex;
	}
};


struct objFace
{
	int v[4];
	int t[4];
	int n[4];

	bool operator==(objFace& fce){
		return false;
	}
};

struct objMtl
{
	str512		name;
	str512		map_kd;
	str512		map_bump;
	bool		imported;
};
Array<objMtl>		g_arrMTL;		//  array of material


struct objMesh
{
	String						name;
	objMtl*						mtl;

	Array<objFace>		arrF;

	objMesh(): mtl(NULL) {}
};

struct objNode
{
	String						name;
	float3						pos;
	Array<objMesh*>	meshes;

	~objNode()
	{
		Clear();
	}

	void Clear(void)
	{
		name.Clear();
		pos.Set(0,0,0);
		while( meshes.Count() )
		{
			sx_delete( meshes[0] );
			meshes.RemoveByIndex(0);
		}
	}

	objMesh* AddMesh(const WCHAR* meshName)
	{
		objMesh* mesh = NULL;

		if ( meshes.IsEmpty() )
		{
			mesh = sx_new( objMesh );
			mesh->name = meshName;
			meshes.PushBack( mesh );
			return mesh;
		}

		mesh = GetLastMesh();
		if ( mesh->arrF.IsEmpty() )
		{
			mesh->name = meshName;
			return mesh;
		}

		mesh = sx_new( objMesh );
		mesh->name = meshName;
		meshes.PushBack( mesh );
		return mesh;
	}

	objMesh* GetLastMesh(void)
	{
		if ( meshes.IsEmpty() )
		{
			return AddMesh( name );
		}
		else
		{
			return meshes[ meshes.Count()-1 ];
		}
	}
};

const WCHAR* MakeName(const WCHAR* baseName, const WCHAR* name)
{
	if ( name ) return name;

	static String			res;
	if ( baseName )
		res = baseName;
	else
		res = L"OBJObject";

	static int icounter = 0;
	res << icounter++;

	return res;
}

void objMesh_to_geometry(objMesh* om, sx::d3d::PGeometry3D gm)
{
	//  OBJ mesh containers
	Map<UINT, UINT>		vertexCache;
	Array<objVertex>	vertices;
	Array<D3DFace>		faces;

	objVertex ov; ZeroMemory(&ov, sizeof(objVertex));
	for (int i=0; i<om->arrF.Count(); i++)
	{
		objFace * of = &om->arrF[i];

		D3DFace f;
		for (int iface=0; iface<3; iface++)
		{
			ov.pos = & g_arrVP[ of->v[ iface ] ];
			ov.nrm = & g_arrVN[ of->n[ iface ] ];
			ov.tex = & g_arrVT[ of->t[ iface ] ];

			UINT index = 0;
			UINT key = ov.GetKey();
			if ( !vertexCache.Find(key, index) )
			{
				index = vertices.Count();
				vertices.PushBack( ov );
				vertexCache.Insert( key, index );
			}

			f.v[ iface ] = index;
		}
		faces.PushBack( f );
	}
	if ( vertices.IsEmpty() || faces.IsEmpty() ) return;
	if ( ( abs(ov.nrm->x) + abs(ov.nrm->y) + abs(ov.nrm->z) ) < EPSILON ) 
		g_Options.flag |= IMPORT_MODEL_RENORMAL;

	gm->VB_Create( vertices.Count() );
	gm->IB_Create( faces.Count() );

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
				//  now traverse in the mesh and extract attributes
				for (int i=0; i<vertices.Count(); i++)
				{
					objVertex * ov = &vertices[i];

					v0[i].pos.x = -ov->pos->x;
					v0[i].pos.y =  ov->pos->y;
					v0[i].pos.z =  ov->pos->z;

					v1[i].nrm.x = -ov->nrm->x;
					v1[i].nrm.y =  ov->nrm->y;
					v1[i].nrm.z =  ov->nrm->z;

					v1[i].txc.x = ov->tex->x;
					v1[i].txc.y = ov->tex->y;
					v1[i].col0 = 0xffffffff;
					v1[i].col1 = 0xffffffff;

					v2[i].tng = float3(0,0,0);
					v2[i].txc.x = ov->tex->x;
					v2[i].txc.y = ov->tex->y;
				}

				gm->VB_UnLock(2);
			}
			gm->VB_UnLock(1);
		}
		gm->VB_UnLock(0);
	}

	//  index buffer contain indices data
	PFace f = NULL;
	if ( gm->IB_Lock(f) )
	{
		for (int i=0; i<faces.Count(); i++)
		{
			f[i].v[0] = faces[i].v[0];
			f[i].v[1] = faces[i].v[2];
			f[i].v[2] = faces[i].v[1];
		}

		gm->IB_UnLock();
	}
}

void AppendToNode(objNode* obnode, sx::core::PNode node)
{
	node->SetName( obnode->name );
	node->SetPosition( obnode->pos );

	for (int i=0; i<obnode->meshes.Count(); i++ )
	{
		objMesh* obmesh = obnode->meshes[i];
		if ( obmesh->arrF.IsEmpty() ) continue;

		sx::core::PMesh	mesh = sx_new( sx::core::Mesh );
		mesh->SetOwner( node );

		if ( obmesh->name.Text() )
			mesh->SetName( obmesh->name );
		else
			mesh->SetName( MakeName(obnode->name, obmesh->name) );
		
		sx::d3d::PGeometry geometry = NULL;
		sx::d3d::Geometry::Manager::Create(geometry, NULL);

		//  copy mesh to the geometry
		objMesh_to_geometry( obmesh, geometry->GetRes(0) );
		geometry->GetRes(0)->WeldVertices();
		geometry->GetRes(0)->Optimize();
		geometry->GetRes(0)->UpdateBoundingVolume();

		//  save geometry
		String		 str;
		if ( mesh->m_Name.Find(L"cameraGuard") )
			str << g_Options.prefix << g_FileName << '@' << mesh->GetName() << g_meshCount++ << L".gmt";
		else
			str << g_Options.prefix << g_FileName << '@' << mesh->GetName() << L".gmt";

		MemoryStream mem;
		geometry->Save(mem);
		sx::sys::FileManager::File_Save(str, SEGAN_PACKAGENAME_GEOMETRY, mem);
		sx::d3d::Geometry::Manager::Release(geometry);
		mesh->SetGeometry( str );
		
		//  import material
		if ( obmesh->mtl )
		{
			if ( !obmesh->mtl->imported )
			{
				
				if ( obmesh->mtl->map_kd.Text() ) 
				{
					str512 txFileName = obmesh->mtl->map_kd;
					txFileName.ExtractFileName();
					txFileName.ExcludeFileExtension();
					txFileName << L".txr";

					txFileName.Insert( g_Options.prefix );

					if ( g_Options.flag & IMPORT_MODEL_TEXTURES || !sx::sys::FileManager::File_Exist(txFileName, SEGAN_PACKAGENAME_TEXTURE) )
						ImportTexture( obmesh->mtl->map_kd );

					//  verify that texture is loaded
					if ( !sx::sys::FileManager::File_Exist(txFileName, SEGAN_PACKAGENAME_TEXTURE) )
					{
						str1024 tmp = obmesh->mtl->map_kd;
						tmp.ExtractFileName();
						ImportTexture( tmp );
					}

					obmesh->mtl->map_kd = txFileName.Text();
				}

				if ( obmesh->mtl->map_bump.Text() )
				{
					str512 txFileName = obmesh->mtl->map_bump;
					txFileName.ExtractFileName();
					txFileName.ExcludeFileExtension();
					txFileName << L".txr";

					txFileName.Insert( g_Options.prefix );

					if ( g_Options.flag & IMPORT_MODEL_TEXTURES || !sx::sys::FileManager::File_Exist(txFileName, SEGAN_PACKAGENAME_TEXTURE) )
						ImportTexture( obmesh->mtl->map_bump );

					//  verify that texture is loaded
					if ( !sx::sys::FileManager::File_Exist(txFileName, SEGAN_PACKAGENAME_TEXTURE) )
					{
						str1024 tmp = obmesh->mtl->map_bump;
						tmp.ExtractFileName();
						ImportTexture( tmp );
					}

					obmesh->mtl->map_bump << txFileName.Text();
				}

				obmesh->mtl->imported = true;
			}

			mesh->GetMaterial(0)->SetTexture( 0, obmesh->mtl->map_kd	);
			mesh->GetMaterial(0)->SetTexture( 1, obmesh->mtl->map_bump	);

			if ( g_Options.shaderName )
				mesh->GetMaterial(0)->SetShader( g_Options.shaderName );
			else if ( obmesh->mtl->map_bump.Text() )
				mesh->GetMaterial(0)->SetShader( L"default_bump_specular.sfx" );
			else
				mesh->GetMaterial(0)->SetShader( L"default.sfx" );
		}
		else
		{
			if ( g_Options.shaderName )
				mesh->GetMaterial(0)->SetShader( g_Options.shaderName );
			else
				mesh->GetMaterial(0)->SetShader( L"default.sfx" );
		}

	}
}

void ImportOBJFile( const WCHAR* FileName, OUT sx::core::ArrayPNode& nodeList, ImportModelOptions* importOption )
{
	if ( !FileName || !importOption ) return;
	sx::cmn::StringList srcOBJ;
	srcOBJ.LoadFromFile( FileName );
	if ( srcOBJ.Count() < 2 ) return;
	sx::cmn::StringToker toker;

	//  prepare directory path
	String	curDir = sx::sys::GetCurrentFolder();
	str1024 filepath = FileName;
	filepath.ExtractFilePath();
	sx::sys::SetCurrentFolder( filepath );


	g_Options = *importOption;
	g_FileName = FileName;
	g_FileName.ExtractFileName();
	g_FileName.ExcludeFileExtension();

	g_arrVP.Clear();
	g_arrVN.Clear();
	g_arrVT.Clear();
	g_arrMTL.Clear();

	//  load objects
	objNode obnode; obnode.pos.Set(0,0,0);
	for (int lineIndex = 0; lineIndex<srcOBJ.Count(); lineIndex++)
	{
		toker.SetString( srcOBJ[lineIndex].Text() );
		str1024 tmp;
		do 
		{
			tmp = toker.Next();

			if ( tmp == L"#" )	//  ignore comments
			{
				break;
			}
			else if ( tmp == L"mtllib" )
			{
				String		 mtlLib;
				String		 mtlLibFile;
				while ( toker.LookatNext().Length() )
					mtlLibFile << toker.Next().Text();
				sx::cmn::String_Load( mtlLib, mtlLibFile );
				sx::cmn::StringToker mtlToker;
				mtlToker.SetString( mtlLib );
				objMtl mtlNew;
				do 
				{
					tmp = mtlToker.Next();
					if ( tmp == L"newmtl" )
					{
						//  add to material array
						if ( mtlNew.name.Text() )
						{
							g_arrMTL.PushBack( mtlNew );
							mtlNew.name.Clear();
							mtlNew.map_kd.Clear();
							mtlNew.map_bump.Clear();
						}

						mtlNew.name = mtlToker.Next().Text();
					}
					else if ( tmp == L"map_Kd" || tmp == L"map_Ka" )
					{
						WCHAR* c = mtlToker.GetCurChar();
						while ( *c != '\n' && *c != '\r' && *c != 0 )
						{
							mtlNew.map_kd.Append( *c );
							c++;
						}
						mtlNew.map_kd.Trim();
						if ( mtlNew.map_kd.Length() && mtlNew.map_kd[0]=='"' )
							mtlNew.map_kd.Delete( mtlNew.map_kd.Length() - 1 );

						//mtlNew.map_kd = String		::ExtractFileName( mtlNew.map_kd );
					}
					else if ( tmp == L"map_bump" || tmp == L"bump" || tmp == L"map_Ks" )
					{
						WCHAR* c = mtlToker.GetCurChar();
						while ( *c != '\n' && *c != '\r' && *c != 0 )
						{
							mtlNew.map_bump.Append( *c );
							c++;
						}
						mtlNew.map_bump.Trim();
						if ( mtlNew.map_bump.Length() && mtlNew.map_bump[0]=='"' )
							mtlNew.map_bump.Delete( mtlNew.map_bump.Length() - 1 );

						//mtlNew.map_kd = String		::ExtractFileName( mtlNew.map_kd );
					}

				} while ( tmp.Length() );

				//  add last material
				if ( mtlNew.name.Text() )
					g_arrMTL.PushBack( mtlNew );
			}
			else if ( tmp == L"v" )
			{
				float3 v;
				v.x = toker.Next().ToFloat();
				v.y = toker.Next().ToFloat();
				v.z = toker.Next().ToFloat();
				g_arrVP.PushBack( v );
			}
			else if ( tmp == L"vt" )
			{
				float tx = toker.Next().ToFloat(); 
				float ty = toker.Next().ToFloat();

				float2 vt ( tx , 1.0f - ty );
				g_arrVT.PushBack( vt );
			}
			else if ( tmp == L"vn" )
			{
				float3 vn;
				vn.x = toker.Next().ToFloat();
				vn.y = toker.Next().ToFloat();
				vn.z = toker.Next().ToFloat();
				g_arrVN.PushBack( vn );
			}
			else if ( tmp == L"g" )
			{
				//  if current node is full save it and prepare OBJ node for new object
				if ( obnode.meshes.Count() && obnode.meshes[0]->arrF.Count() )
				{
					//  save node
					if ( g_Options.flag & IMPORT_MODEL_ONE_NODE )
					{
						obnode.name = g_FileName;
						if ( nodeList.IsEmpty() )
							nodeList.PushBack( sx_new( sx::core::Node ) );

						AppendToNode(&obnode, nodeList[0]);
					}
					else
					{
						obnode.name = MakeName( g_FileName , obnode.name );
						sx::core::PNode sxNode = sx_new( sx::core::Node );
						AppendToNode(&obnode, sxNode);
						nodeList.PushBack( sxNode );
					}

					//  clear node
					obnode.Clear();
				}

				//	new OBJ node founded
				obnode.name = toker.Next().Text();
			}
			else if ( tmp == L"o" )
			{
				obnode.AddMesh( toker.Next().Text() );
			}
			else if ( tmp == L"usemtl" )
			{
				tmp = toker.Next();
				for (int i=0; i<g_arrMTL.Count(); i++)
				{
					if (tmp == g_arrMTL[i].name)
					{
						obnode.GetLastMesh()->mtl = &g_arrMTL[i];
						break;
					}
				}
			}
			else if ( tmp == L"f" )
			{
				objMesh* mesh = obnode.GetLastMesh();

				//  collect faces
				objFace F;
				ZeroMemory(&F, sizeof(objFace));
				for (int i=0; i<4; i++)
				{
					F.v[i] = sx_abs_i( toker.Next().ToInt() ) - 1;
					tmp = toker.LookatNext();	if ( tmp != '/' ) continue;
					tmp = toker.Next();
					F.t[i] = sx_abs_i( toker.Next().ToInt() ) - 1;
					tmp = toker.LookatNext();	if ( tmp != '/' ) continue;
					tmp = toker.Next();
					F.n[i] = sx_abs_i( toker.Next().ToInt() ) - 1;
				}

				//  prepare face as triangle
				{
					objFace f1;
					f1.v[0] = F.v[0];	f1.t[0] = F.t[0];	f1.n[0] = F.n[0];
					f1.v[1] = F.v[1];	f1.t[1] = F.t[1];	f1.n[1] = F.n[1];
					f1.v[2] = F.v[2];	f1.t[2] = F.t[2];	f1.n[2] = F.n[2];
					f1.v[3] = 0;		f1.t[3] = 0;		f1.n[3] = 0;
					mesh->arrF.PushBack( f1 );
				}

				if ( F.v[3] > -1 ) // face is a quad
				{
					objFace f2;
					f2.v[0] = F.v[0];	f2.t[0] = F.t[0];	f2.n[0] = F.n[0];
					f2.v[1] = F.v[2];	f2.t[1] = F.t[2];	f2.n[1] = F.n[2];
					f2.v[2] = F.v[3];	f2.t[2] = F.t[3];	f2.n[2] = F.n[3];
					f2.v[3] = 0;		f2.t[3] = 0;		f2.n[3] = 0;
					mesh->arrF.PushBack( f2 );
				}
			}


		} while ( tmp.Length() );
	}

	//  append final node to the node list
	if ( obnode.meshes.Count() && obnode.meshes[0]->arrF.Count() )
	{
		//  save node
		if ( g_Options.flag & IMPORT_MODEL_ONE_NODE )
		{
			obnode.name = g_FileName;
			if ( nodeList.IsEmpty() )
				nodeList.PushBack( sx_new( sx::core::Node ) );

			AppendToNode(&obnode, nodeList[0]);
		}
		else
		{
			obnode.name = MakeName( g_FileName, obnode.name );
			sx::core::PNode sxNode = sx_new( sx::core::Node );
			AppendToNode(&obnode, sxNode);
			nodeList.PushBack( sxNode );
		}
	}

	sx::sys::SetCurrentFolder( curDir );


	for (int nodeIndex=0; nodeIndex<nodeList.Count(); nodeIndex++)
	{
		sx::core::PNode node = nodeList[nodeIndex];

		//  translate node to the correct position
		node->UpdateBoundingVolumes();
		float3 newPos = (g_Options.flag & IMPORT_MODEL_CENTERPV) ? - node->GetSphere_local().center : sx::math::VEC3_ZERO;

		sx::core::ArrayPNodeMember meshList;
		node->GetMembersByType(NMT_MESH, meshList, true);
		for (int i=0; i<meshList.Count(); i++)
		{
			sx::core::PMesh mesh = (sx::core::PMesh)meshList[i];

			//  apply mesh options
			if ( g_Options.meshOption & SX_MESH_CASTSHADOW )
				mesh->AddOption( SX_MESH_CASTSHADOW );
			else
				mesh->RemOption( SX_MESH_CASTSHADOW );

			if ( g_Options.meshOption & SX_MESH_RECIEVESHADOW )
				mesh->AddOption( SX_MESH_RECIEVESHADOW );
			else
				mesh->RemOption( SX_MESH_RECIEVESHADOW );

			if ( g_Options.meshOption & SX_MESH_INVISIBLE )
				mesh->AddOption( SX_MESH_INVISIBLE );
			else
				mesh->RemOption( SX_MESH_INVISIBLE );

// 			if ( g_Options.meshOption & SX_MESH_SELECTABLE )
// 				mesh->AddOption( SX_MESH_SELECTABLE );
// 			else
// 				mesh->RemOption( SX_MESH_SELECTABLE );

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
					if ( g_Options.flag & IMPORT_MODEL_RENORMAL )
					{
						gm->GetRes(0)->ComputeNormals();
					}
					gm->GetRes(0)->ComputeTangents();
					gm->GetRes(0)->UpdateBoundingVolume();
					gm->GenerateSubLOD( g_Options.precisionLOD1, g_Options.precisionLOD2, mesh->GetGeometry() );

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

		// finally save and load
		MemoryStream memFile;
		node->Save(memFile);
		node->Clear();
		memFile.SetPos(0);
 		node->Load(memFile);
		
		float f=0;
		node->MsgProc( MT_ACTIVATE, &f );
	} // for (int nodeIndex=0; nodeIndex<nodeList.Count(); nodeIndex++)
}

void ExportOBJFile( const WCHAR* FileName, OUT sx::core::ArrayPNode& nodeList )
{
#define NEWLINE	L"\r\n"
#define FLUSHTOFILE(){\
	for ( int i=0; i<srcOBJ.Length(); i++ )\
	{\
		char c = (char)srcOBJ[i];\
		fileOBJ.Write( &c,  1 );\
	}\
	srcOBJ.Clear();\
	for ( int i=0; i<srcMTL.Length(); i++ )\
	{\
		char c = (char)srcMTL[i];\
		fileMTL.Write( &c,  1 );\
	}\
	srcMTL.Clear();\
}\

	String	fileNameOBJ = FileName; fileNameOBJ.ExcludeFileExtension();
	String	fileNameMTL = fileNameOBJ.Text();
	fileNameOBJ << L".obj";
	fileNameMTL << L".mtl";

	sx::sys::FileStream fileOBJ;
	sx::sys::FileStream fileMTL;

	if ( !fileOBJ.Open( fileNameOBJ, FM_CREATE ) )
	{
		sxLog::Log( L"Can't create file on %s !", fileNameOBJ);
		return;
	}

	if ( !fileMTL.Open( fileNameMTL, FM_CREATE ) )
	{
		sxLog::Log( L"Can't create file on %s !", fileNameMTL);
		fileOBJ.Close();
		return;
	}

	
	str2048	srcMTL;
	str2048	srcOBJ;

	srcOBJ << L"# SeganX engine OBJ Exporter v0.1b - www.seganx.com - sajad.b@gmail.com" << NEWLINE;
	srcOBJ << L"# File Created: " << sx::sys::GetDate() << L"  " << sx::sys::GetTime() << NEWLINE << NEWLINE;
	srcMTL = srcOBJ.Text();

	FLUSHTOFILE();

	str1024 tmp = fileNameMTL;	tmp.ExtractFileName();
	srcOBJ << NEWLINE << L"mtllib " << tmp.Text() << NEWLINE << NEWLINE;

	FLUSHTOFILE();

	int vertexCount = 0;
	int meshCount = 0;
	sx::cmn::StringObjectList textureCount;

	for (int i=0; i<nodeList.Count(); i++)
	{
		sx::core::PNode node = nodeList[i];

// 		meshCount++;
// 		srcOBJ << NEWLINE << L"o " << (node->GetName() ? node->GetName() : L"NoNamedGroup") << meshCount << NEWLINE;
// 		FLUSHTOFILE();

		sx::core::ArrayPNodeMember memberList;
		node->GetMembersByType( NMT_MESH, memberList, true );
		//node->GetMembersByType( NMT_TERRAIN, memberList, true );
		for (int j=0; j<memberList.Count(); j++)
		{
			meshCount++;
			sx::core::PNodeMember member = memberList[j];
			srcOBJ << NEWLINE << L"g " << (member->GetName() ? member->GetName() : L"NoNamed") << meshCount << NEWLINE << NEWLINE;

			FLUSHTOFILE();

			//////////////////////////////////////////////////////////////////////////
			//  export material
			str128	mtlName = member->GetName();
			if ( !mtlName.Text() ) mtlName = L"Default";
			
			srcOBJ << L"usemtl " << mtlName.Text() << NEWLINE;

			srcMTL << NEWLINE << L"newmtl " << mtlName << NEWLINE;
			srcMTL << L"\tKa 1.0000 1.0000 1.0000" << NEWLINE;
			srcMTL << L"\tKd 1.0000 1.0000 1.0000" << NEWLINE;
			srcMTL << L"\tKs 0.0000 0.0000 0.0000" << NEWLINE;
			srcMTL << L"\tKe 0.0000 0.0000 0.0000" << NEWLINE;

			FLUSHTOFILE();

			sx::d3d::PMaterial pMtrl = NULL;
			if ( member->GetType() == NMT_MESH )
				pMtrl = sx::core::PMesh(member)->GetActiveMaterial();
			else
				pMtrl = sx::core::Terrain::Manager::GetMaterial(0)->GetActiveMaterial();

			if ( pMtrl->GetTexture(0) )
			{
				sx::d3d::PTexture texture = NULL;
				if ( sx::d3d::Texture::Manager::Exist(texture, pMtrl->GetTexture(0)) )
				{
					str1024	 textureName;
					int iobj = textureCount.IndexOfObject(texture);
					if ( iobj < 0 )
					{
						str1024 fileName = FileName;
						fileName.ExtractFilePath();
						fileName.MakePathStyle();
						fileName << pMtrl->GetTexture(0);
						fileName.ExcludeFileExtension();
						textureName = texture->SaveToImageFile( fileName, NULL );
						textureCount.PushBack( textureName, texture );
					}
					else
					{
						textureName = textureCount[iobj].text.Text();
					}
					
					if ( textureName.Text() )
					{
						textureName.ExtractFileName();
						srcMTL << L"\tmap_Ka " << textureName.Text() << NEWLINE;
						srcMTL << L"\tmap_Kd " << textureName.Text() << NEWLINE;

						FLUSHTOFILE();
					}
				}
			}

			if ( pMtrl->GetTexture(1) )
			{
				sx::d3d::PTexture texture = NULL;
				if ( sx::d3d::Texture::Manager::Exist(texture, pMtrl->GetTexture(1)) )
				{
					String		 textureName;
					int iobj = textureCount.IndexOfObject(texture);
					if ( iobj < 0 )
					{
						str1024 fileName = FileName;
						fileName.ExtractFilePath();
						fileName.MakePathStyle();
						fileName << pMtrl->GetTexture(1);
						fileName.ExcludeFileExtension();
						textureName = texture->SaveToImageFile( fileName, NULL );
						textureCount.PushBack( textureName, texture );
					}
					else
					{
						textureName = textureCount[iobj].text.Text();
					}

					if ( textureName.Text() )
					{
						textureName.ExtractFileName();
						srcMTL << L"\tmap_Ks " << textureName.Text() << NEWLINE;
						srcMTL << L"\tmap_bump " << textureName.Text() << NEWLINE;

						FLUSHTOFILE();
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//	export geometry
			UINT					nvrtx = 0;
			UINT					nface = 0;
			PD3DVertex0				v0 = NULL;
			PD3DVertex1				v1 = NULL;
			PD3DFace				fc  = NULL;
			sx::d3d::PGeometry3D	gm3d = NULL;

			//  open geometry
			if ( member->GetType() == NMT_MESH )
			{
				sx::d3d::PGeometry geometry = NULL;
				if ( sx::d3d::Geometry::Manager::Exist(geometry, sx::core::PMesh(member)->GetGeometry()) )
				{
					gm3d = geometry->GetRes(0);
					if ( gm3d->VB_IsEmpty() )	gm3d = geometry->GetRes(1);
					if ( gm3d->VB_IsEmpty() )	gm3d = geometry->GetRes(2);

					nvrtx = gm3d->GetVertexCount();
					nface = gm3d->GetFaceCount();
					gm3d->VB_Lock(0, (PD3DVertex&)v0);
					gm3d->VB_Lock(1, (PD3DVertex&)v1);
					gm3d->IB_Lock(fc);
				}
			}
			else
			{
				sx::core::PTerrain terr = sx::core::PTerrain(member);
				nvrtx = terr->GetVertexCount();
				nface = terr->GetFaceCount();
				v0 = (PVertex0)terr->GetVertices(0);
				v1 = (PVertex1)terr->GetVertices(1);
				fc = terr->GetFaces();
			}

			if ( v0 )
			{
				float3 v;
				for (UINT k=0; k<nvrtx; k++)
				{
					v.Transform( v0[k].pos, member->GetOwner()->GetMatrix_world() );
					str256 tmp; tmp.Format( L"v %f %f %f" NEWLINE, -v.x, v.y, v.z );
					srcOBJ << tmp;

					FLUSHTOFILE();

				}
			}

			if ( v1 )
			{
				float2 vt;
				for (UINT k=0; k<nvrtx; k++)
				{
					vt.Set( v1[k].txc.x, 1.0f - v1[k].txc.y );
					str256 tmp; tmp.Format( L"vt %f %f" NEWLINE, vt.x, vt.y );
					srcOBJ << tmp;

					FLUSHTOFILE();

				}

				float3 vn;
				for (UINT k=0; k<nvrtx; k++)
				{
					vn.Transform_Norm( v1[k].nrm, member->GetOwner()->GetMatrix_world() );
					str256 tmp; tmp.Format( L"vn %f %f %f" NEWLINE, -vn.x, vn.y, vn.z );
					srcOBJ << tmp;

					FLUSHTOFILE();

				}
			}

			if ( fc )
			{
				D3DFace of;
				for (UINT k=0; k<nface; k++)
				{
					of.v[0] = fc[k].v[0] + vertexCount + 1;
					of.v[1] = fc[k].v[1] + vertexCount + 1;
					of.v[2] = fc[k].v[2] + vertexCount + 1;
					srcOBJ << L"f " << (int)of.v[0] << L"/" << (int)of.v[0] << L"/" << (int)of.v[0];
					srcOBJ << L" "  << (int)of.v[2] << L"/" << (int)of.v[2] << L"/" << (int)of.v[2];
					srcOBJ << L" "  << (int)of.v[1] << L"/" << (int)of.v[1] << L"/" << (int)of.v[1] << NEWLINE;

					FLUSHTOFILE();

				}
			}

			if ( member->GetType() == NMT_MESH  && gm3d )
			{
				gm3d->VB_UnLock(0);
				gm3d->VB_UnLock(1);
				gm3d->IB_UnLock();
			}

			vertexCount += nvrtx;

		} // for (int j=0; j<meshList.Count(); j++)

	}// for (int i=0; i<nodeList.Count(); i++)



	FLUSHTOFILE();

	fileOBJ.Close();
	fileMTL.Close();
}