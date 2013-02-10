/********************************************************************
	created:	2011/09/28
	filename: 	sxTerrain.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class of a node based terrain
*********************************************************************/
#ifndef GUARD_sxTerrain_HEADER_FILE
#define GUARD_sxTerrain_HEADER_FILE

#include "sxNodeMember.h"
#include "sxMaterialMan.h"
#include "../sxRender/sxGeometry.h"


namespace sx { namespace core {

	//! the simple node based terrain object. a scene can made by more that one terrain objects
	class SEGAN_API Terrain : public NodeMember
	{

		SEGAN_STERILE_CLASS(Terrain);

	public:

		Terrain();
		virtual ~Terrain();

		//! set an owner for this member. this will send SetOwner message to the MsgProc
		void SetOwner(PNode pOwner);

		//! clear current content
		void Clear(void);

		//! this will call when the owner's "Validate" message received
		void Validate(int level);

		//! this will call when the owner's "Invalidate" message received
		void Invalidate(void);

		//!	this will call when the owner's "Update" function called
		void Update(float elpsTime);

		//! draw this Terrain depend on entry flag
		void Draw(DWORD flag);

		//! draw with LOD depend on view parameter. ignore draw for so far Terrain
		void DrawByViewParam(DWORD flag, float viewParam);

		//! use to communicate by this member
		UINT MsgProc(UINT msgType, void* data);

		//! save this member to the stream
		void Save(Stream& stream);

		//! load data of this member from the stream
		void Load(Stream& stream);

		//! return level of detail value depend on view parameter
		int ComputeLODValue(float viewParam);

		//! return number of vertices in this terrain
		UINT GetVertexCount(void);

		//! return number of faces in this terrain
		UINT GetFaceCount(void);
		
		//! return pointer to the vertex buffer of terrain
		PVertex GetVertices(int index = 0);

		//! return pointer to the face buffer of terrain
		const PFace GetFaces(void);

	private:
		math::AABox		m_Box;				//  bounding box of the Terrain
		math::Sphere	m_Sphere;			//  bounding sphere of the Terrain
		DWORD			m_GroupID;			//  id of group of this part of terrain

		//  vertices
		PVertex0		m_pVertex0;			//  vertices of this terrain
		PVertex1		m_pVertex1;			//  vertices of this terrain
		PVertex2		m_pVertex2;			//  vertices of this terrain

	public:
		class SEGAN_API Manager
		{
		public:

			//! use begin/end patch to patch terrain nodes in the scene
			static void BeginPatch(const Frustum& cameraFrustum, const float cameraFOV);

			//! use begin/end patch to patch terrain nodes in the scene
			static void EndPatch(DWORD flag);

			//! reference to the LOD threshold parameter. default value is about 7.0
			static float& LOD_Threshold(void);

			//! return specified material group
			static PMaterialMan GetMaterial(int index);

			//! clean up created resources
			static void ClearrAll(void);

			//! save terrain extra informations
			static void Save(Stream& strm);

			//! load terrain extra informations
			static void Load(Stream& strm);
		};

	};
	typedef Terrain *PTerrain;


}}	//  namespace sx { namespace core {

#endif	//	GUARD_sxTerrain_HEADER_FILE
