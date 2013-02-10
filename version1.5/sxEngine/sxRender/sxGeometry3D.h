/********************************************************************
	created:	2010/08/27
	filename: 	sxGeometry.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Geometry3D class that holds vertices 
				and indices. sxGeometry3D is an advanced management of 
				geometry for SeganX Engine.
*********************************************************************/
#ifndef GUARD_sxGeometry3D_HEADER_FILE
#define GUARD_sxGeometry3D_HEADER_FILE

#include "sxResource3D.h"

namespace sx { namespace d3d
{
	//! a geometry class that represent Direct3D geometry buffer resource
	class SEGAN_API Geometry3D
	{
		SEGAN_STERILE_CLASS(Geometry3D);

	public:
		Geometry3D(void);
		~Geometry3D(void);

		//! create vertex buffer by given vertex count
		bool VB_Create(UINT VrtxNum, bool AnimationVertexBuffer = false);

		//! create vertex buffer by specified index of vertex stream
		bool VB_CreateByIndex(UINT VrtxNum, int index, DWORD SX_RESOURCE_ flag = 0);

		//! get description of the current vertex buffer
		bool VB_GetDesc(int index, D3DVertexBufferDesc& vbDesc);

		//! lock the vertex buffer by given index
		bool VB_Lock(int index, PD3DVertex& vrtcs, DWORD lockFlag = 0);

		//! unlock specified vertex buffer
		void VB_UnLock(int index);

		//! cleanup vertex buffer
		void VB_Cleanup(void);

		//! create index buffer by number of faces
		bool IB_Create(UINT FaceNum, DWORD SX_RESOURCE_ flag = 0);

		//! get description of current index buffer
		bool IB_GetDesc(D3DIndexBufferDesc& ibDesc);

		//! lock the index buffer by indices
		bool IB_Lock(PDWORD& indices, DWORD lockFlag = 0);

		//! unlock the index buffer by faces
		bool IB_Lock(PD3DFace& faces, DWORD lockFlag = 0);

		//! unlock the index buffer
		void IB_UnLock(void);

		//! cleanup the index buffer
		void IB_Cleanup(void);

		//! update bounding box and bounding sphere of the geometry
		void UpdateBoundingVolume(void);

		//! reference to the bounding box
		math::AABox& BoundingBox(void);

		//! reference to the bounding sphere
		math::Sphere& BoundingSphere(void);

		//! set this geometry to device
		void SetToDevice(void);

		//! set this geometry to device
		void SetToDevice(bool vb0, bool vb1, bool vb2, bool vb3, bool ib);

		//! return the number of vertex
		UINT  GetVertexCount(void);

		//! return the number of faces
		UINT  GetFaceCount(void);

		//! cleanup all vertices and indices
		void Cleanup(void);

		//! return true if the geometry vertex container has no buffer
		bool VB_IsEmpty(int index = 0);

		//! return true if the geometry index container has no buffer
		bool IB_IsEmpty(void);

		//! save the current object to the stream
		void Save(Stream& stream);

		//! load the current object from stream
		void Load(Stream& stream);

		//! weld vertices
		void WeldVertices(bool Partial = true);

		//! compute normals
		void ComputeNormals(void);

		//! compute tangents
		void ComputeTangents(void);

		//! Optimize by removing back faces and vertex cache reordering
		void Optimize(void);

		//! weld vertices and optimize geometry and compute normals and tangents
		void Weld_Optimize_ComputeNT(bool weldPartial = true);

	private:

		math::AABox				m_Box;			//  Bounding box of the geometry
		math::Sphere			m_Sphere;		//  Bounding sphere of the geometry
		UINT					m_VrtxCount;	//  Number of vertices's
		UINT					m_FaceCount;	//  Number of faces
		bool					m_IsLocked[5];	//  used to locking buffers

		PDirect3DVertexBuffer	m_VB[4];	//  Geometry vertex buffer
		PDirect3DIndexBuffer	m_IB;		//  Geometry index buffer

	public:
		static void BoundBox_Calcul(OUT AABox& outBox, PD3DVertex0 v, UINT numVertex);
		static void BoundBox_Transf(OUT AABox& outBox, AABox& inBox, Matrix& m);
		static void BoundSph_Calcul(OUT Sphere& outSph, AABox& inBox);
		static void BoundSph_Calcul(OUT Sphere& outSph, PD3DVertex0 v, UINT numVertex);

		//! refill out geometry by simplified main geometry
		static void Simplify(IN Geometry3D& InGeometry, OUT Geometry3D& OutGeometry, float Percent, const WCHAR* userLog);

		//! translate all vertices in the geometry. vertex.pos += addPos;
		static void Translate(IN_OUT Geometry3D& geometry, float3 addPos);
	};
	typedef Geometry3D *PGeometry3D;

} } // namespace sx { namespace d3d

#endif	//	GUARD_sxGeometry3D_HEADER_FILE
