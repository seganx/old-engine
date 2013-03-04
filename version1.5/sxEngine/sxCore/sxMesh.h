/********************************************************************
	created:	2010/11/06
	filename: 	sxMesh.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the renderable class of the engine.
				this class contain any options and resources needed to
				draw a 3D object.
*********************************************************************/
#ifndef GUARD_sxMesh_HEADER_FILE
#define GUARD_sxMesh_HEADER_FILE

#include "sxNodeMember.h"
#include "sxMaterialMan.h"
#include "../sxRender/sxGeometry.h"


namespace sx { namespace core {

	// forward declaration
	typedef class Mesh *PMesh;

	//! the main renderable class of the engine. this class contain any options and resources needed to draw a 3D object. 
	class SEGAN_API Mesh : public NodeMember
	{
		friend class Renderer;
		friend class Pipeline_Forward;
		friend class Pipeline_Deferred;

		SEGAN_STERILE_CLASS(Mesh);

	public:
		Mesh();
		virtual ~Mesh();

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

		//! draw this mesh depend on entry flag
		void Draw(DWORD flag);

		//! draw width specified LOD
		void DrawLOD(DWORD flag, int LOD);

		//! draw with LOD depend on view parameter. ignore draw for so far mesh
		void DrawByViewParam(DWORD flag, float viewParam);

		//! use to communicate by this member
		UINT MsgProc(UINT msgType, void* data);

		//! save this member to the stream
		void Save(Stream& stream);

		//! load data of this member from the stream
		void Load(Stream& stream);

		//! set geometry resource for this mesh
		void SetGeometry(const WCHAR* srcAddress);

		//! get geometry resource of this mesh
		const WCHAR* GetGeometry(void);

		//! return reference to material manager
		MaterialMan& Material(void);

		//! add new material
		sx::d3d::PMaterial AddMaterial(void);

		//! remove a material
		void RemoveMaterial(int index);

		//! get a material by index. return null if no material found
		d3d::PMaterial GetMaterial(int index);

		//! return the number of materials
		int	GetMaterialCount(void);

		//! active the render material by index. active material 0 if index be in invalid range
		void SetActiveMaterial(int index);

		//! return the index of material which is currently activated
		int	GetActiveMaterialIndex(void);

		//! return pointer to the current active material
		d3d::PMaterial GetActiveMaterial(void);

		/*! return level of detail value depend on view parameter. 	*/
		int ComputeLodValue(float viewParam);

		//! set new scale for the mesh
		void SetScale( const float x, const float y, const float z );

	public:
		float3			m_scale;			//	scale of the mesh
		math::AABox		m_Box;				//  bounding box of the mesh
		math::Sphere	m_Sphere;			//  bounding sphere of the mesh
		d3d::PGeometry	m_Geometry;			//  geometry of this mesh

		MaterialMan		m_Material;			//  materials of this mesh
		
		DWORD			reserved;			//  use to some internal options and operation

	public:
		//////////////////////////////////////////////////////////////////////////
		//	MESH MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:
			//! use begin/end patch to patch meshes in the scene
			static void BeginPatch(DWORD flag);

			//! use begin/end patch to patch meshes in the scene
			static void EndPatch(DWORD flag);

			//! coefficient of minimum volume of meshes in screen space which can display
			static float& MinimumVolume(void);

			//! coefficient of view parameter in LOD selection for meshes
			static float& LODRange(void);
		};
	};


}}	//  namespace sx { namespace core {

#endif	//	GUARD_sxMesh_HEADER_FILE