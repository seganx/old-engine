/********************************************************************
	created:	2010/10/09
	filename: 	sxTask3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some tasks for work on Direct3D device
				to control the device behavior in mulithreaded mode.

				NOTE: DO NOT CALL THESE TASK FROM MAIN THREAD !!!!
*********************************************************************/
#ifndef GUARD_sxTask3D_HEADER_FILE
#define GUARD_sxTask3D_HEADER_FILE

#include "sxTypes3D.h"
#include "sxGeometry3D.h"
#include "sxTexture3D.h"
#include "../sxSystem/sxSystem.h"

//  hold geometry data that will transport between threads
typedef struct TaskGeometryData
{
	PD3DVertex	vb0;
	PD3DVertex	vb1;
	PD3DVertex	vb2;
	PD3DVertex	vb3;
	PD3DFace	ib0;

	int			numVrtx;
	int			numFaces;
	bool		animated;

	TaskGeometryData(): vb0(0), vb1(0), vb2(0), vb3(0), ib0(0), numVrtx(0), numFaces(0), animated(false) {}
} *PTaskGeometryData;

//  hold texture data that will transport between threads
typedef struct TaskTextureData
{
	D3DResourceType	rType;
	D3DFormat		rFotmat;
	int				createWidth;
	int				levelToLock;
	D3DLockedRect	rect[6];

	TaskTextureData() : rType(D3DRTYPE_TEXTURE), rFotmat(D3DFMT_DXT5), createWidth(0) {
		ZeroMemory(rect, sizeof(D3DLockedRect)*6);
	}
} *PTaskTextureData;


namespace sx { namespace d3d
{
	/*
	This task will create buffers and lock geometry buffers in main thread.
	NOTE: DO NOT CALL THIS TASK FROM MAIN THREAD !!!!
	*/
	class SEGAN_API TaskGeometry3DCreateLock : public sx::sys::TaskBase
	{
	public:
		TaskGeometry3DCreateLock(bool FreeOnTerminate);
		void SetData(PGeometry3D pgmtr3d, PTaskGeometryData pData, HANDLE hEventHandle);
		void Execute(void);

	private:
		PGeometry3D			m_pGeometry;
		PTaskGeometryData	m_pData;
		HANDLE				m_hEvent;
	};

	/*
	This task will unlock geometry buffers in main thread.
	NOTE: DO NOT CALL THIS TASK FROM MAIN THREAD !!!!
	*/
	class SEGAN_API TaskGeometry3DUnlock : public sx::sys::TaskBase
	{
	public:
		TaskGeometry3DUnlock(bool FreeOnTerminate);
		void SetData(PGeometry3D pgmtr3d, HANDLE hEventHandle);
		void Execute(void);

	private:
		PGeometry3D m_pGeometry;
		HANDLE		m_hEvent;
	};

	/*
	This task will cleanup geometry buffers in main thread.
	NOTE: DO NOT CALL THIS TASK FROM MAIN THREAD !!!!
	*/
	class SEGAN_API TaskGeometry3DCleanup : public sx::sys::TaskBase
	{
	public:
		TaskGeometry3DCleanup(bool FreeOnTerminate);
		void SetData(PGeometry3D pGmtr, HANDLE hEventHandle);
		void Execute(void);

	private:
		PGeometry3D	m_pGeometry;
		HANDLE		m_hEvent;
	};

	/*
	This task will create buffers and lock texture buffers in main thread.
	NOTE: DO NOT CALL THIS TASK FROM MAIN THREAD !!!!
	*/
	class SEGAN_API TaskTexture3DCreateLock : public sx::sys::TaskBase
	{
	public:
		TaskTexture3DCreateLock(bool FreeOnTerminate);
		void SetData(PTexture3D ptxur3d, PTaskTextureData pData, HANDLE hEventHandle);
		void Execute(void);

	private:
		PTexture3D			m_pTexture;
		PTaskTextureData	m_pData;
		HANDLE				m_hEvent;
	};

	/*
	This task will unlock a texture buffers in main thread.
	NOTE: DO NOT CALL THIS TASK FROM MAIN THREAD !!!!
	*/
	class SEGAN_API TaskTexture3DUnlock : public sx::sys::TaskBase
	{
	public:
		TaskTexture3DUnlock(bool FreeOnTerminate);
		void SetData(PTexture3D ptxur3d, int level, HANDLE hEventHandle);
		void Execute(void);

	private:
		PTexture3D	m_pTexture;
		int			m_iLevel;
		HANDLE		m_hEvent;
	};

	/*
	This task will cleanup texture buffers in main thread.
	NOTE: DO NOT CALL THIS TASK FROM MAIN THREAD !!!!
	*/
	class SEGAN_API TaskTexture3DCleanup : public sx::sys::TaskBase
	{
	public:
		TaskTexture3DCleanup(bool FreeOnTerminate);
		void SetData(PTexture3D ptxur3d, HANDLE hEventHandle);
		void Execute(void);

	private:
		PTexture3D	m_pTexture;
		HANDLE		m_hEvent;
	};

} } // namespace sx { namespace d3d


#endif	//	GUARD_sxTask3D_HEADER_FILE