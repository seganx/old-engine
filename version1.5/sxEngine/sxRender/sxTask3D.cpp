#include "sxTask3D.h"

namespace sx { namespace d3d
{

	//////////////////////////////////////////////////////////////////////////
	TaskGeometry3DCreateLock::TaskGeometry3DCreateLock( bool FreeOnTerminate ): TaskBase(FreeOnTerminate) {}
	void TaskGeometry3DCreateLock::SetData( PGeometry3D pgmtr3d, PTaskGeometryData pData, HANDLE hEventHandle )
	{
		m_pGeometry = pgmtr3d;
		m_pData		= pData;
		m_hEvent	= hEventHandle;
		ResetEvent(m_hEvent);
	}

	void TaskGeometry3DCreateLock::Execute( void )
	{
		// verify that this task is in main thread
		if ((BYTE)m_flag != 0)
		{
			sx::sys::TaskManager::AddTask(this, 0);
			return;
		}

		//  create a new geometry
		m_pGeometry->VB_Create(m_pData->numVrtx, m_pData->animated);
		m_pGeometry->IB_Create(m_pData->numFaces);

		//  new try to lock each buffers
		m_pGeometry->VB_Lock(0, m_pData->vb0);
		m_pGeometry->VB_Lock(1, m_pData->vb1);
		m_pGeometry->VB_Lock(2, m_pData->vb2);
		m_pGeometry->VB_Lock(3, m_pData->vb3);
		m_pGeometry->IB_Lock( m_pData->ib0 );

#ifdef SEGAN_LOG_LEVEL3
		sxLog::Logger() << L"geometry locked";
#endif

		//  set signal
		SetEvent(m_hEvent);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	TaskGeometry3DUnlock::TaskGeometry3DUnlock( bool FreeOnTerminate ): TaskBase(FreeOnTerminate) {}
	void TaskGeometry3DUnlock::SetData( PGeometry3D pgmtr3d, HANDLE hEventHandle )
	{
		m_pGeometry = pgmtr3d;
		m_hEvent	= hEventHandle;
		ResetEvent(m_hEvent);
	}

	void TaskGeometry3DUnlock::Execute( void )
	{
		// verify that this task is in main thread
		if ((BYTE)m_flag != 0)
		{
			sx::sys::TaskManager::AddTask(this, 0);
			return;
		}

		//  close all opened buffers
		m_pGeometry->VB_UnLock(0);
		m_pGeometry->VB_UnLock(1);
		m_pGeometry->VB_UnLock(2);
		m_pGeometry->VB_UnLock(3);
		m_pGeometry->IB_UnLock();

#ifdef SEGAN_LOG_LEVEL3
		sxLog::Logger() << L"geometry unlocked";
#endif

		//  set signal
		SetEvent(m_hEvent);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	TaskGeometry3DCleanup::TaskGeometry3DCleanup( bool FreeOnTerminate ): TaskBase(FreeOnTerminate) {}
	void TaskGeometry3DCleanup::SetData( PGeometry3D pGmtr, HANDLE hEventHandle )
	{
		m_pGeometry = pGmtr;
		m_hEvent	= hEventHandle;
		ResetEvent(m_hEvent);
	}

	void TaskGeometry3DCleanup::Execute( void )
	{
		// verify that this task is in main thread
		if ((BYTE)m_flag != 0)
		{
			sx::sys::TaskManager::AddTask(this, 0);
			return;
		}

		//  cleanup geometry
		m_pGeometry->Cleanup();

		//  set signal
		SetEvent(m_hEvent);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	TaskTexture3DCreateLock::TaskTexture3DCreateLock( bool FreeOnTerminate ): TaskBase(FreeOnTerminate) {}
	void TaskTexture3DCreateLock::SetData( PTexture3D ptxur3d, PTaskTextureData pData, HANDLE hEventHandle )
	{
		m_pTexture	= ptxur3d;
		m_pData		= pData;
		m_hEvent	= hEventHandle;
		ResetEvent(m_hEvent);
	}

	void TaskTexture3DCreateLock::Execute( void )
	{
		// verify that this task is in main thread
		if ((BYTE)m_flag != 0)
		{
			sx::sys::TaskManager::AddTask(this, 0);
			return;
		}

		//  create a new texture resource
		if ( !m_pTexture->GetD3DTexture() )
			m_pTexture->CreateTexture(m_pData->rType, m_pData->createWidth, m_pData->rFotmat);

		//  now try to lock level buffer
		m_pTexture->Lock(m_pData->levelToLock, m_pData->rect);

		//  set signal
		SetEvent(m_hEvent);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	TaskTexture3DUnlock::TaskTexture3DUnlock( bool FreeOnTerminate ): TaskBase(FreeOnTerminate) {}
	void TaskTexture3DUnlock::SetData( PTexture3D ptxur3d, int level, HANDLE hEventHandle )
	{
		m_pTexture	= ptxur3d;
		m_iLevel	= level;
		m_hEvent	= hEventHandle;
		ResetEvent(m_hEvent);
	}

	void TaskTexture3DUnlock::Execute( void )
	{
		// verify that this task is in main thread
		if ((BYTE)m_flag != 0)
		{
			sx::sys::TaskManager::AddTask(this, 0);
			return;
		}

		m_pTexture->Unlock(m_iLevel);

		//  set signal
		SetEvent(m_hEvent);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	TaskTexture3DCleanup::TaskTexture3DCleanup( bool FreeOnTerminate ): TaskBase(FreeOnTerminate) {}
	void TaskTexture3DCleanup::SetData( PTexture3D ptxur3d, HANDLE hEventHandle )
	{
		m_pTexture	= ptxur3d;
		m_hEvent	= hEventHandle;
		ResetEvent(m_hEvent);
	}

	void TaskTexture3DCleanup::Execute( void )
	{
		// verify that this task is in main thread
		if ((BYTE)m_flag != 0)
		{
			sx::sys::TaskManager::AddTask(this, 0);
			return;
		}

		//  cleanup geometry
		m_pTexture->Cleanup();

		//  set signal
		SetEvent(m_hEvent);
	}
	//////////////////////////////////////////////////////////////////////////

} } // namespace sx { namespace d3d
