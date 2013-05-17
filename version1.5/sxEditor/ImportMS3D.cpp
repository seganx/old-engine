#include "ImportMS3D.h"
#include "MS3DFile.h"
#include "ImportTexture.h"

using namespace sx;
using namespace sx::d3d;

struct cacheEntery
{
	ms3d_vertex_t	*	v;	//  vertex
	ms3d_triangle_t	*	t;	//  triangle
	UINT				i;	//  vertex index
	UINT				n;	//  index of normals


	UINT GetKey(void){
		
		WCHAR c[16];
		
		UINT p = UINT( (UINT64)v );

		PWORD w = (PWORD)&p;
		c[0] = w[0]==0 ? 1 : w[0];
		c[1] = w[1]==0 ? 1 : w[1];

		w = (PWORD)&t->tu[n];
		c[2] = w[0]==0 ? 1 : w[0];
		c[3] = w[1]==0 ? 1 : w[1];

		w = (PWORD)&t->tv[n];
		c[4] = w[0]==0 ? 1 : w[0];
		c[5] = w[1]==0 ? 1 : w[1];

		c[6] = t->smoothingGroup + 1;
		c[7] = 0;

		return sx::cmn::GetCRC32(c);
	}

	bool operator==(cacheEntery& en){
		if ( v != v ) return false;
		if ( t->smoothingGroup )
		{
			if ( t->smoothingGroup != en.t->smoothingGroup ) return false;
			t->vertexNormals[n][0] = ( t->vertexNormals[n][0] + en.t->vertexNormals[en.n][0] ) / 2;
			t->vertexNormals[n][1] = ( t->vertexNormals[n][1] + en.t->vertexNormals[en.n][1] ) / 2;
			t->vertexNormals[n][2] = ( t->vertexNormals[n][2] + en.t->vertexNormals[en.n][2] ) / 2;

			if ( t->tu[n] != en.t->tu[en.n] || t->tv[n] != en.t->tv[en.n] ) return false;
			return true;
		}
		else
		{
			return 
				t->tu[n] == en.t->tu[en.n] &&
				t->tv[n] == en.t->tv[en.n] &&
				t->vertexNormals[n][0] == en.t->vertexNormals[en.n][0] &&
				t->vertexNormals[n][1] == en.t->vertexNormals[en.n][1] &&
				t->vertexNormals[n][2] == en.t->vertexNormals[en.n][2] &&
				t->smoothingGroup == en.t->smoothingGroup;
		}
	}
};

void ms3dgroup_To_geometry3d(ms3d_group_t* group, CMS3DFile& ms3d, PGeometry3D gm)
{
	//  ms3d mesh containers
	Map<UINT, cacheEntery>	vertexCache;
	Array<cacheEntery>		vertices;
	Array<D3DFace>			faces;


	for (int i=0; i<group->numtriangles; i++)
	{
		ms3d_triangle_t* t = NULL;
		ms3d.GetTriangleAt( group->triangleIndices[i], &t );

		D3DFace	f;
		for (int iface=0; iface<3; iface++)
		{
			ms3d_vertex_t* v = NULL;
			ms3d.GetVertexAt( t->vertexIndices[ iface ], &v );

			cacheEntery entry;
			entry.i = vertices.Count();
			entry.n = iface;
			entry.v = v;
			entry.t = t;			

			cacheEntery pentry;
			UINT key = entry.GetKey();
			if (  vertexCache.Find(key, pentry)/*&& (*pentry)==entry*/ )
			{
				f.v[ iface ] = pentry.i;
			}
			else
			{
				f.v[ iface ] = entry.i;

				vertices.PushBack( entry );
				vertexCache.Insert( key, entry );
			}			
		}

		//  convert faces from right hand space to left hand
		UINT tmp = f.v[1];
		f.v[1] = f.v[2];
		f.v[2] = tmp;
		faces.PushBack( f );
	}
	if ( vertices.IsEmpty() || faces.IsEmpty() ) return;

	int numVert = vertices.Count();
	int numFace = faces.Count();

	gm->VB_Create( numVert, ms3d.GetNumJoints() > 0 );
	gm->IB_Create( numFace );

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
				if ( gm->VB_Lock(3, (PVertex&)v3) || !ms3d.GetNumJoints() )
				{
					for (int i=0; i<numVert; i++)
					{
						cacheEntery* pEntery;
						pEntery = &vertices[i];
						
						v0[i].pos.x = -pEntery->v->vertex[0];
						v0[i].pos.y =  pEntery->v->vertex[1];
						v0[i].pos.z =  pEntery->v->vertex[2];
						v1[i].nrm.x = -pEntery->t->vertexNormals[ pEntery->n ][0];
						v1[i].nrm.y =  pEntery->t->vertexNormals[ pEntery->n ][1];
						v1[i].nrm.z =  pEntery->t->vertexNormals[ pEntery->n ][2];
						v1[i].txc.x = pEntery->t->tu[ pEntery->n ];
						v1[i].txc.y = pEntery->t->tv[ pEntery->n ];
						v1[i].col0 = 0xffffffff;
						v1[i].col1 = 0xffffffff;
						v2[i].tng.x = 0.0f;
						v2[i].tng.y = 0.0f;
						v2[i].tng.z = 0.0f;
						v2[i].txc.x = v1[i].txc.x;
						v2[i].txc.y = v1[i].txc.y;

						if (v3)
						{
							v3[i].bi[0] = pEntery->v->boneId;
							v3[i].bi[1] = pEntery->v->boneId;
							v3[i].bi[2] = pEntery->v->boneId;
							v3[i].bi[3] = pEntery->v->boneId;

							v3[i].bw[0] = 1.0f;
							v3[i].bw[1] = 1.0f;
							v3[i].bw[2] = 1.0f;
							v3[i].bw[3] = 1.0f;
						}
					}
					gm->VB_UnLock(3);
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
		memcpy( f, &faces[0], faces.Count() * sizeof(D3DFace) );
	}
	gm->IB_UnLock();
}

void ms3dAnim_to_Animation( CMS3DFile& ms3d, PAnimation pAnim, int startFrame, int endFrame )
{
	for (int i=0; i < ms3d.GetNumJoints(); i++)
	{
		ms3d_joint_t* joint = NULL;
		ms3d.GetJointAt(i, &joint);

		str256 str = joint->name;
		sx::d3d::PAnimationJoint pJoint = pAnim->AddAnimJoint(str);
		pJoint->m_MaxTime	= (endFrame-startFrame) / ms3d.GetAnimationFPS();
		pJoint->m_Parent	= joint->parentName;

		pJoint->m_Base.position.x =  -joint->position[0];
		pJoint->m_Base.position.y =  joint->position[1];
		pJoint->m_Base.position.z =  joint->position[2];
		pJoint->m_Base.rotation.SetRotationPYR( joint->rotation[0], -joint->rotation[1], -joint->rotation[2] );
		
		Matrix matBase;
		pJoint->m_Base.rotation.GetRotationMatrix( matBase );
		matBase._41 = pJoint->m_Base.position.x;
		matBase._42 = pJoint->m_Base.position.y;
		matBase._43 = pJoint->m_Base.position.z;

		//  start capturing animations
		float startTime = startFrame / ms3d.GetAnimationFPS();
		float endTime = endFrame / ms3d.GetAnimationFPS();
		int numKeysPos = joint->numKeyFramesTrans;
		int numKeysRot = joint->numKeyFramesRot;
		AnimationKeyFrame lastKey = pJoint->m_Base;

		int kp=0; int kr=0;
		while ( kp < numKeysPos || kr < numKeysRot )
		{
			float curTimePos = kp < numKeysPos ? joint->keyFramesTrans[kp].time : 9999999.0f;
			float curTimeRot = kr < numKeysRot ? joint->keyFramesRot[kr].time : 9999999.0f;
			if ( !SEGAN_BETWEEN(curTimePos, startTime, endTime) ) curTimePos = 9999999.0f;
			if ( !SEGAN_BETWEEN(curTimeRot, startTime, endTime) ) curTimeRot = 9999999.0f;
			if ( curTimePos > 9999998.0f && curTimeRot > 9999998.0f ) {
				if ( kr < numKeysRot+1 ) kr++; 
				if ( kp < numKeysPos+1 ) kp++;
				continue;
			}

			if ( abs(curTimePos - curTimeRot) < 0.01f )
			{
				PAnimationKeyFrame pKey = pJoint->AddKeyFrame( curTimePos - startTime );

				pKey->rotation.SetRotationPYR( joint->keyFramesRot[kr].rotation[0], -joint->keyFramesRot[kr].rotation[1], -joint->keyFramesRot[kr].rotation[2] );
				pKey->rotation *= pJoint->m_Base.rotation;
				pKey->rotation.Normalize( pKey->rotation );
				lastKey.rotation = pKey->rotation;
				if ( kr < numKeysRot+1 ) kr++;

				pKey->position.x = -joint->keyFramesTrans[kp].position[0];
				pKey->position.y =  joint->keyFramesTrans[kp].position[1];
				pKey->position.z =  joint->keyFramesTrans[kp].position[2];
				pKey->position.Transform( pKey->position, matBase );
				lastKey.position = pKey->position;
				if ( kp < numKeysPos+1 ) kp++;
				
			}
			else if ( curTimePos < curTimeRot )
			{
				PAnimationKeyFrame pKey = pJoint->AddKeyFrame( curTimePos - startTime );

				//	interpolate between last rotation key and current rotation key
				int nkr = kr;
				while ( joint->keyFramesRot[nkr].time < curTimePos && nkr < numKeysRot+1 ) nkr++;
				sx::math::floatQ nextRotation;
				nextRotation.SetRotationPYR( joint->keyFramesRot[nkr].rotation[0], -joint->keyFramesRot[nkr].rotation[1], -joint->keyFramesRot[nkr].rotation[2] );
				nextRotation *= pJoint->m_Base.rotation;
				nextRotation.Normalize( nextRotation );
				float w = ( curTimePos - lastKey.time ) / ( joint->keyFramesRot[nkr].time - lastKey.time );
				pKey->rotation.SLerp( lastKey.rotation, nextRotation, w );


				pKey->position.x = -joint->keyFramesTrans[kp].position[0];
				pKey->position.y =  joint->keyFramesTrans[kp].position[1];
				pKey->position.z =  joint->keyFramesTrans[kp].position[2];
				pKey->position.Transform( pKey->position, matBase );
				lastKey.position = pKey->position;
				if ( kp < numKeysPos+1 ) kp++;
			}
			else
			{
				PAnimationKeyFrame pKey = pJoint->AddKeyFrame( curTimePos - startTime );

				pKey->rotation.SetRotationPYR( joint->keyFramesRot[kr].rotation[0], -joint->keyFramesRot[kr].rotation[1], -joint->keyFramesRot[kr].rotation[2] );
				pKey->rotation *= pJoint->m_Base.rotation;
				pKey->rotation.Normalize( pKey->rotation );
				lastKey.rotation = pKey->rotation;
				if ( kr < numKeysRot+1 ) kr++;

				//	interpolate between last position key and current position key
				int nkp = kp;
				while ( joint->keyFramesTrans[nkp].time < curTimeRot && nkp < numKeysPos+1 ) nkp++;
				sx::math::float3 nextpos;
				nextpos.x = -joint->keyFramesTrans[nkp].position[0];
				nextpos.y =  joint->keyFramesTrans[nkp].position[1];
				nextpos.z =  joint->keyFramesTrans[nkp].position[2];
				nextpos.Transform( nextpos, matBase );
				float w = ( curTimeRot - lastKey.time ) / ( joint->keyFramesTrans[nkp].time - lastKey.time );
				pKey->position.Lerp( lastKey.position, nextpos, w );
			}

		}
	} // for (int i=0; i < ms3d.GetNumJoints(); i++)
}

void ImportMS3DFile( const WCHAR* FileName, const UINT flag, OUT sx::core::PNode& node )
{
	if ( !SEGAN_SET_HAS(flag, IMPORT_MS3D_ANIMATION) && !SEGAN_SET_HAS(flag, IMPORT_MS3D_MODEL) ) return;

	if ( !sx::sys::FileExist(FileName) )
	{
		sxLog::Log(L"File '%s' could not found!", FileName);
		return;
	}

	CMS3DFile ms3d;
	if ( !ms3d.LoadFromFile(FileName) )
	{
		sxLog::Log(L"Can't load the file '%s'", FileName);
		return;
	}

	str1024 str = FileName;
	str.ExtractFileName();
	str.ExcludeFileExtension();

	if (!node) 
	{
		node = sx_new( sx::core::Node );
		node->SetName( str );
	}

	//  extract material
	if ( flag & IMPORT_MS3D_MODEL && ms3d.GetNumMaterials() )
	{
		for (int i=0; i<ms3d.GetNumMaterials(); i++)
		{
			ms3d_material_t* material = NULL;
			ms3d.GetMaterialAt(i, &material);

			if (material && material->texture)
			{
				str = material->texture;
				str.ExtractFileName();

				str1024 txFile = FileName;
				txFile.ExtractFilePath();
				txFile.MakePathStyle();
				txFile << str;

				ImportTexture(txFile);
			}							
		}
	}

	if ( (flag & IMPORT_MS3D_ANIMATION) && ms3d.GetNumJoints() )
	{
		str = node->GetName();
		str << L"@animator";
		sx::core::PAnimator animator = sx_new( sx::core::Animator );
		animator->SetName(str);
		animator->SetOwner(node);
		animator->AddOption(SX_ANIMATOR_PLAY);
		animator->AddOption(SX_ANIMATOR_LOOP);
		animator->SetSpeed( 1.0f );

		//  try to load animation description file
		str = FileName;
		str.ExcludeFileExtension();
		str << L".txt";
		cmn::StringList strList;
		strList.LoadFromFile( str );
		if ( strList.Count() )		//  animation description file founded
		{
			for (int l=0; l<strList.Count(); l++)
			{
				if ( !strList[l]->Text() ) continue;
				str = strList[l]->Text();

				int i=0;
				for ( ; i<str.Length() && ( str[i]==' ' || str[i]=='\t' ); i++ );

				if ( !isdigit( str[i] ) ) continue;
				str2048 token;
				for ( ; i<str.Length() && SEGAN_BETWEEN(str[i],'0','9'); i++) token << str[i];
				int startFrame = token.StrToInt(token);
				
				for ( ; i<str.Length() && ( str[i]==' ' || str[i]=='\t' ); i++ );
				if ( str[i++] != '-' ) continue;
				for ( ; i<str.Length() && ( str[i]==' ' || str[i]=='\t' ); i++ );

				if ( !isdigit( str[i] ) ) continue;
				token.Clear();
				for ( ; i<str.Length() && SEGAN_BETWEEN(str[i],'0','9'); i++) token << str[i];
				int endFrame = token.StrToInt(token);

				token.Clear();
				for (i++; i<str.Length(); i++) token << str[i];
				token.Trim();

				if ( !token.Length() ) continue;

				str = node->GetName(); 
				str << '@' << token << L".anim";
				
				sx::d3d::PAnimation pAnim = NULL;
				sx::d3d::Animation::Manager::Create(pAnim, NULL);

				ms3dAnim_to_Animation(ms3d, pAnim, startFrame, endFrame);

				// save the animation
				MemoryStream mem;
				pAnim->Save(mem);
				sx::sys::FileManager::File_Save(str, SEGAN_PACKAGENAME_ANIMATION, mem);
				sx::d3d::Animation::Manager::Release(pAnim);

				animator->AddAnimation(str);
				//animator->SetAnimationBySrc(str);
			}
		}
		else  //  animation description file not found
		{
			str = node->GetName();
			str << L".anim";

			//  create main animation
			sx::d3d::PAnimation mainAnim = NULL;
			sx::d3d::Animation::Manager::Create(mainAnim, NULL);
			ms3dAnim_to_Animation(ms3d, mainAnim, 0, ms3d.GetTotalFrames());

			// save the animation
			MemoryStream mem;
			mainAnim->Save(mem);
			sx::sys::FileManager::File_Save(str, SEGAN_PACKAGENAME_ANIMATION, mem);
			animator->SetAnimationBySrc(str);

			//  release created main animation
			sx::d3d::Animation::Manager::Release(mainAnim);

		}

		animator->SetAnimationByIndex(0);
	} // if (flag & IMPORT_MS3D_ANIMATION)
	
	for (int i=0; i<ms3d.GetNumGroups() && (flag & IMPORT_MS3D_MODEL); i++)
	{
		ms3d_group_t* group = NULL;
		ms3d.GetGroupAt(i, &group);
		if (group)
		{
			str = group->name;

			//  group found. create new mesh for the this group
			sx::core::PMesh mesh = sx_new( sx::core::Mesh );
			mesh->SetName( str );
			mesh->SetOwner( node );

			//  extract geometry from file
			sx::d3d::PGeometry geometry = NULL;
			sx::d3d::Geometry::Manager::Create(geometry, NULL);

			ms3dgroup_To_geometry3d( group, ms3d, geometry->GetRes(0) );
			
			geometry->GetRes(0)->WeldVertices();
 			geometry->GetRes(0)->Optimize();
 			geometry->GetRes(0)->ComputeTangents();
			//geometry->GetRes(0)->Weld_Optimize_ComputeNT( true );
			geometry->GetRes(0)->UpdateBoundingVolume();

			str = node->GetName(); str << '@' << group->name << L".gmt";
			geometry->GenerateSubLOD( 0.0f, 0.0f, str );

			//  save geometry data
			MemoryStream mem;
			geometry->Save(mem);
			sx::sys::FileManager::File_Save(str, SEGAN_PACKAGENAME_GEOMETRY, mem);
			sx::d3d::Geometry::Manager::Release(geometry);
			mesh->SetGeometry(str);

			//  extract material
			if (group->materialIndex >= 0)
			{
				ms3d_material_t* material = NULL;
				ms3d.GetMaterialAt(group->materialIndex, &material);

				if (material)
				{
					str = material->texture;
					str.ExtractFileName();
					
					str1024 txFile = FileName;
					txFile.ExtractFilePath();
					txFile.MakePathStyle();
					txFile << str;
					txFile.ExtractFileName();
					txFile.ExcludeFileExtension();
					txFile << L".txr";
					mesh->GetMaterial(0)->SetTexture(0, txFile);
				}							
			}

// 			str = node->GetName();
// 			str << '@' << mesh->GetName();
// 			str << L".msh";
// 
// 			mem.Clear();
// 			mesh->Save(mem);
// 			sx::sys::FileManager::File_Save(str, SEGAN_PACKAGENAME_DRAFT, mem);
		}
	}

	node->UpdateBoundingVolumes();
	
	//////////////////////////////////////////////////////////////////////////
	//	TEST
	sx::core::ArrayPNodeMember meshList;
	node->GetMembersByType(NMT_MESH, meshList, true);
	for (int i=0; i<meshList.Count(); i++)
	{
		sx::core::PMesh mesh = (sx::core::PMesh)meshList[i];
		if ( node->MemberTypeExist(NMT_ANIMATOR) )
			mesh->GetMaterial(0)->SetShader(L"anim_default.sfx");
		else
			mesh->GetMaterial(0)->SetShader(L"default.sfx");
	}
	//////////////////////////////////////////////////////////////////////////

// 	str = node->GetName();
// 	str << L".node";
// 	MemoryStream mem;
// 	node->Save(mem);
// 	sx::sys::FileManager::File_Save(str, SEGAN_PACKAGENAME_DRAFT, mem);
}