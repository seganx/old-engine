/********************************************************************
	created:	2010/10/05
	filename: 	sxGeometry.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Geometry interface that holds geometry's
				data plus connection geometry data to I/O stream and also
				contain simple internal manager.
				The Geometry abstract class can't be initialized or create 
				directly. Use PGeometry type and Geometry::Manager to create
				or release usable geometries.
*********************************************************************/
#ifndef GUARD_sxGeometry_HEADER_FILE
#define GUARD_sxGeometry_HEADER_FILE

#include "sxRender_def.h"
#include "sxGeometry3D.h"
#include "../sxSystem/sxSystem.h"

namespace sx { namespace d3d {

	/*
	Geometry abstract class can't be initialized or create directly.
	Use PGeometry type and Geometry::Manager to create or release usable geometries.
	*/
	typedef class Geometry *PGeometry;

	/*
	Geometry abstract class can't be initialized or create directly.
	Use PGeometry type and Geometry::Manager to create or release usable geometries.
	*/
	class SEGAN_API Geometry
	{
	public:
		//! return the ID of this resource
		virtual DWORD GetID(void) = 0;

		//! set the source address of the resource. this call will update ID by CRC32 algorithm
		virtual void  SetSource(const WCHAR* srcAddress) = 0;

		//! return the source address
		virtual const WCHAR* GetSource(void) = 0;

		//! set additional options
		virtual void  SetOption(DWORD op) = 0;

		//! get current option
		virtual DWORD GetOption(void) = 0;

		//! return the pointer of d3d resource object
		virtual d3d::PGeometry3D GetRes(int level) = 0;

		//! generate automatically sub LODs from top level LOD by a simple algorithm.
		virtual void GenerateSubLOD(const float percent1, const float percent2, const WCHAR* userLog) = 0;

		//! return number of vertices in this level
		virtual UINT GetVertexCount(int level) = 0;

		//! return number of triangles in this level
		virtual UINT GetFaceCount(int level) = 0;

		//! return reference to the bounding box
		virtual AABox& GetBoundingBox(void) = 0;

		//! return reference to the bounding sphere
		virtual Sphere& GetBoundingSphere(void) = 0;

		//! return the index of the level that which can be sit to device and return -1 if geometry not valid
		virtual int Validate(int level) = 0;

		//! set the current resources to the device and return the index of the level that have been sit and return -1 if geometry not valid
		virtual int SetToDevice(int level) = 0;

		//! set the current resources to the device and return the index of the level that have been sit and return -1 if geometry not valid
		virtual int SetToDevice(int level, bool vb0, bool vb1, bool vb2, bool vb3, bool ib) = 0;

		/*!
		Set resource valid and increase activated counter.
		LOD = 0; load top level LOD. if sub levels was empty then load all sub levels
		LOD = n; load specifyed LOD of level n. if sub levels was empty then load all sub levels
		*/
		virtual void Activate(int LOD = 0) = 0;

		//! decrease activated counter and cleanup resources when activated counter became zero
		virtual void Deactivate() = 0;

		//! cleanup any external resources
		virtual void Cleanup(void) = 0;

		//! save the current object to the stream
		virtual void Save(Stream& stream, PResourceFileDetails pDetails = NULL) = 0;

		//! load the current object from stream
		virtual void Load(Stream& stream) = 0;

		//////////////////////////////////////////////////////////////////////////
		//  INTERNAL MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:

			/*! 
			return true if geometry of the 'src' is exist in the manager and throw out founded one.
			NOTE: this function DO NOT increase the internal reference counter of the object. so releasing
			the object after the work finished may cause to destroy the object.
			*/
			static bool Exist(OUT PGeometry& pGmtry, const WCHAR* src);
			
			/*!
			search for geometry of the src. if not exist in manager, create and return new one.
			NOTE: this function increase the internal reference counter of the object. so release
			the object after the work finished.
			*/
			static bool Get(OUT PGeometry& pGmtry, const WCHAR* src);

			//! create a new geometry independent of the others.
			static bool Create(OUT PGeometry& pGmtry, const WCHAR* src);

			//! decrease the internal reference counter and destroy the geometry if reference counter became zero.
			static void Release(PGeometry& pGmtry);

			//! clear all geometries in the manager
			static void ClearAll(void);

			//! specify to load geometries in other threads
			static bool& LoadInThread(void);

			//! get first iterator of geometry container
			static Map<UINT, sx::d3d::PGeometry>::Iterator& GetFirst(void);
		};

	};

} } // namespace sx { namespace d3d


//////////////////////////////////////////////////////////////////////////
//	SOME HELPER CLASSES
//////////////////////////////////////////////////////////////////////////
class DrivedGeometry;

//! use to load geometry data in multi threaded mode
class DrivedTaskGeometryLoader : public sx::sys::TaskBase
{
public:
	DrivedTaskGeometryLoader();
	void Execute(void);
	int				m_LOD;
	DrivedGeometry*	m_geometry;
};

//! use to cleanup geometry data in multi threaded mode
class DrivedTaskGeometryCleanup : public sx::sys::TaskBase
{
public:
	DrivedTaskGeometryCleanup();
	void Execute(void);
	DrivedGeometry*	m_geometry;
};

//! use to destroy geometry data in multi threaded mode
class DrivedTaskGeometryDestroy : public sx::sys::TaskBase
{
public:
	DrivedTaskGeometryDestroy();
	void Execute(void);
	DrivedGeometry*	m_geometry;
};

#endif // GUARD_sxGeometry_HEADER_FILE