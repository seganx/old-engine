#include "sxGeometry.h"
#include "sxResource3D.h"
#include "sxTask3D.h"
#include "../sxCommon/sxCommon.h"


#define GM_THREADID					1
#define GM_MAX_LOCK_TIME			10000	// ms

typedef Map<UINT, sx::d3d::PGeometry> sxMapGeometry;
static 	sxMapGeometry GeometryMap;	//  hold the created geometry objects

static const int	GeometryFileVersion = 1;
static const char*	GeometryFileTitle	= "SeganX Geometry File - sajad.b@gmail.com";
static bool			s_geometryLoadInThread = false;

//  will use to change shared variables in other threads
static CriticalSection CS;

//	this is the header file of the geometry and of course will be written after version and title ;)
struct GeometryFileHeader
{
	ResourceFileDetails	details;
	GeometryDesc		desc;				//  geometry description
	UINT				levelPos[8];		//  position of each level in the file
};

//---	FILE STRUCTURE	---
//	meta file
//		|	title & version
//	header file
//		|	description
//		|	LOD positions
//
//	LOD	2
//		|	size of vb0	  |	  vb0 data
//		|	size of vb1	  |	  vb1 data
//		|	size of vb2	  |	  vb2 data
//		|	size of vb3	  |	  vb3 data
//		|	size of index |	  index data
//
//	LOD n
//		...
//-------------------------


//////////////////////////////////////////////////////////////////////////
//	DRIVED GEOMETRY
//////////////////////////////////////////////////////////////////////////
class DrivedGeometry : public sx::d3d::Geometry
{
	
	SEGAN_STERILE_CLASS(DrivedGeometry);

	enum  LODStatus { EMPTY, LOADING, LOADED };

public:
	DrivedGeometry( void ): m_Optn(0), m_RefCount(1), m_APIRefCount(0)
	{
		m_ID = sx::cmn::ID_Generate(10);
		m_CleanupTask.m_geometry = this;

		for (int i=0; i<3; i++)
		{
			m_LODStatus[i] = EMPTY;
			m_LoaderTask[i].m_geometry = this;
			m_LoaderTask[i].m_LOD = i;
		}
	}

	virtual ~DrivedGeometry( void )
	{
		Cleanup();
	}

	FORCEINLINE DWORD GetID( void )
	{
		return m_ID;
	}

	void SetSource( const WCHAR* srcAddress )
	{
		UINT id = sx::cmn::GetCRC32(srcAddress);
		if (id == m_ID) return;

		sx::d3d::PGeometry g = NULL;
		if (GeometryMap.Find(m_ID, g) && g==this)
		{
			GeometryMap.Remove(m_ID);
			GeometryMap.Insert(id, this);
		}

		m_Src = srcAddress;
		m_ID = id;

		//  load necessary data
		//  TEST: RISK: I should save and load geometry bounding volumes in a perfect way. 
		//	but currently I have to open file and read header file to solve the problem :(
		PStream file;
		if ( sx::sys::FileManager::File_Open(m_Src, SEGAN_PACKAGENAME_GEOMETRY, file) )
		{
			ZeroMemory(&m_Header, sizeof(GeometryFileHeader));

			//  at first, read the version and title
			int fileVer = 0; char ch[1024];
			file->Read(&fileVer, sizeof(fileVer));
			file->Read(ch, (int)strlen(GeometryFileTitle));

			if (fileVer == 1)	//  read geometry header file
				file->Read(&m_Header, sizeof(GeometryFileHeader));

			sx::sys::FileManager::File_Close(file);
		}
	}

	FORCEINLINE const WCHAR* GetSource( void )
	{
		return m_Src;
	}

	FORCEINLINE void SetOption( DWORD op )
	{
		m_Optn = op;
	}

	FORCEINLINE DWORD GetOption( void )
	{
		return m_Optn;
	}

	FORCEINLINE sx::d3d::PGeometry3D GetRes( int level )
	{
		return &m_APIGM[level];
	}

	void GenerateSubLOD( const float percent1, const float percent2, const WCHAR* userLog )
	{
		if( m_APIGM[0].VB_IsEmpty() ) return;
		sx::d3d::Geometry3D::Simplify(m_APIGM[0], m_APIGM[1], percent1, userLog );
		sx::d3d::Geometry3D::Simplify(m_APIGM[0], m_APIGM[2], percent2, userLog );
	}

	FORCEINLINE UINT GetVertexCount(int level)
	{
		switch (level)
		{
		case 0:
			return 
				( m_LODStatus[0] == LOADED ) ? m_APIGM[0].GetVertexCount() :
				( m_LODStatus[1] == LOADED ) ? m_APIGM[1].GetVertexCount() : 
				m_APIGM[2].GetVertexCount();

		case 1:	
			return 
				( m_LODStatus[1] == LOADED ) ? m_APIGM[1].GetVertexCount() :
				( m_LODStatus[0] == LOADED ) ? m_APIGM[0].GetVertexCount() : 
				m_APIGM[2].GetVertexCount();

		case 2:	
			return 
				( m_LODStatus[2] == LOADED ) ? m_APIGM[2].GetVertexCount() :
				( m_LODStatus[1] == LOADED ) ? m_APIGM[1].GetVertexCount() : 
				m_APIGM[0].GetVertexCount();
		}
		return 0;
	}

	FORCEINLINE UINT GetFaceCount(int level)
	{
		switch (level)
		{
		case 0:	
			return 
				( m_LODStatus[0] == LOADED ) ? m_APIGM[0].GetFaceCount() :
				( m_LODStatus[1] == LOADED ) ? m_APIGM[1].GetFaceCount() : 
				m_APIGM[2].GetFaceCount();

		case 1:	
			return 
				( m_LODStatus[1] == LOADED ) ? m_APIGM[1].GetFaceCount() :
				( m_LODStatus[0] == LOADED ) ? m_APIGM[0].GetFaceCount() : 
				m_APIGM[2].GetFaceCount();

		case 2:	
			return 
				( m_LODStatus[2] == LOADED ) ? m_APIGM[2].GetFaceCount() :
				( m_LODStatus[1] == LOADED ) ? m_APIGM[1].GetFaceCount() : 
				m_APIGM[2].GetFaceCount();
		}
		return 0;
	}

	FORCEINLINE AABox& Geometry::GetBoundingBox( void )
	{
		if ( m_Header.desc.Box == sx::math::AABOX_ZERO )
			return m_APIGM[0].BoundingBox();
		else
			return m_Header.desc.Box;
	}

	FORCEINLINE Sphere& Geometry::GetBoundingSphere( void )
	{
		if ( m_Header.desc.Sphere == sx::math::SPHERE_ZERO )
			return m_APIGM[0].IB_IsEmpty() ? (m_APIGM[1].IB_IsEmpty() ? m_APIGM[2].BoundingSphere() : m_APIGM[1].BoundingSphere()) : m_APIGM[0].BoundingSphere();
		else
			return m_Header.desc.Sphere;
	}

	FORCEINLINE int Validate(int level)
	{
		switch (level)
		{
		case 0:
			{
				if ( m_LODStatus[0] == LOADED && !m_APIGM[0].IB_IsEmpty() )
					return 0;
				else if ( m_LODStatus[1] == LOADED && !m_APIGM[1].IB_IsEmpty() )
					return 1;
				else if ( m_LODStatus[2] == LOADED && !m_APIGM[2].IB_IsEmpty() )
					return 2;
				else return -1;
			}

		case 1:
			{
				if ( m_LODStatus[1] == LOADED && !m_APIGM[1].IB_IsEmpty() )
					return 1;
				else if ( m_LODStatus[0] == LOADED && !m_APIGM[0].IB_IsEmpty() )
					return 0;
				else if ( m_LODStatus[2] == LOADED && !m_APIGM[2].IB_IsEmpty() )
					return 2;
				else return -1;
			}

		case 2:
			{
				if ( m_LODStatus[2] == LOADED && !m_APIGM[2].IB_IsEmpty() )
					return 2;
				else if ( m_LODStatus[1] == LOADED && !m_APIGM[1].IB_IsEmpty() )
					return 1;
				else if ( m_LODStatus[0] == LOADED && !m_APIGM[0].IB_IsEmpty() )
					return 0;
				else return -1;
			}
		}

		return -1;
	}

	FORCEINLINE int SetToDevice( int level )
	{
		switch (level)
		{
		case 0:
			{
				if ( m_LODStatus[0] == LOADED )
				{
					m_APIGM[0].SetToDevice();
					return 0;
				}
				else if ( m_LODStatus[1] == LOADED )
				{
					m_APIGM[1].SetToDevice();
					return 1;
				}
				else if ( m_LODStatus[2] == LOADED )
				{
					m_APIGM[2].SetToDevice();
					return 2;
				}
				else return -1;
			}

		case 1:
			{
				if ( m_LODStatus[1] == LOADED )
				{
					m_APIGM[1].SetToDevice();
					return 1;
				}
				else if ( m_LODStatus[0] == LOADED )
				{
					m_APIGM[0].SetToDevice();
					return 0;
				}
				else if ( m_LODStatus[2] == LOADED )
				{
					m_APIGM[2].SetToDevice();
					return 2;
				}
				else return -1;
			}

		case 2:
			{
				if ( m_LODStatus[2] == LOADED )
				{
					m_APIGM[2].SetToDevice();
					return 2;
				}
				else if ( m_LODStatus[1] == LOADED )
				{
					m_APIGM[1].SetToDevice();
					return 1;
				}
				else if ( m_LODStatus[0] == LOADED )
				{
					m_APIGM[0].SetToDevice();
					return 0;
				}
				else return -1;
			}
		}

		return -1;

	}

	FORCEINLINE int SetToDevice( int level, bool vb0, bool vb1, bool vb2, bool vb3, bool ib )
	{
		switch (level)
		{
		case 0:
			{
				if ( m_LODStatus[0] == LOADED )
				{
					m_APIGM[0].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 0;
				}
				else if ( m_LODStatus[1] == LOADED )
				{
					m_APIGM[1].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 1;
				}
				else if ( m_LODStatus[2] == LOADED )
				{
					m_APIGM[2].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 2;
				}
				else return -1;
			}

		case 1:
			{
				if ( m_LODStatus[1] == LOADED )
				{
					m_APIGM[1].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 1;
				}
				else if ( m_LODStatus[0] == LOADED )
				{
					m_APIGM[0].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 0;
				}
				else if ( m_LODStatus[2] == LOADED )
				{
					m_APIGM[2].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 2;
				}
				else return -1;
			}

		case 2:
			{
				if ( m_LODStatus[2] == LOADED )
				{
					m_APIGM[2].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 2;
				}
				else if ( m_LODStatus[1] == LOADED )
				{
					m_APIGM[1].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 1;
				}
				else if ( m_LODStatus[0] == LOADED )
				{
					m_APIGM[0].SetToDevice(vb0, vb1, vb2, vb3, ib);
					return 0;
				}
				else return -1;
			}
		}

		return -1;
	}

	void Activate( int LOD )
	{
		if ( !s_geometryLoadInThread )
		{
			if ( m_LODStatus[0] == EMPTY )
			{
				PStream file;
				if ( sx::sys::FileManager::File_Open( m_Src, SEGAN_PACKAGENAME_GEOMETRY, file ) )
				{
					Load( *file );
					sx::sys::FileManager::File_Close( file );
				}
			}

			m_APIRefCount++;
			return;
		}

		switch (LOD)
		{
		case 0:		m_APIRefCount++;	LoadByTaskManager(0);	break;
		case 1:		m_APIRefCount++;	LoadByTaskManager(1);	break;
		case 2:		m_APIRefCount++;	LoadByTaskManager(2);	break;
		}

	}

	void Deactivate( void )
	{
		if ( !m_APIRefCount ) return;

		m_APIRefCount--;

		if ( !m_APIRefCount )
		{
			//  verify that this geometry use tasks in task manager
			if ( m_LoaderTask[0].Executing() || m_LoaderTask[1].Executing() || m_LoaderTask[2].Executing() )
			{
				//  clear any load task
				sx::sys::TaskManager::RemoveTask(&m_LoaderTask[0]);
				sx::sys::TaskManager::RemoveTask(&m_LoaderTask[1]);
				sx::sys::TaskManager::RemoveTask(&m_LoaderTask[2]);
				sx::sys::TaskManager::RemoveTask(&m_CleanupTask);

				//  send cleanup task to the task manager
				sx::sys::TaskManager::AddTask(&m_CleanupTask, GM_THREADID);

				//  now the current geometry has been cleaned up
				CS.Enter();
				m_LODStatus[0] = EMPTY;
				m_LODStatus[1] = EMPTY;
				m_LODStatus[2] = EMPTY;
				CS.Leave();
			}
			else
			{
				Cleanup();
			}
		}
	}

	void Cleanup( void )
	{
		CS.Enter();
		m_APIGM[0].Cleanup();
		m_APIGM[1].Cleanup();
		m_APIGM[2].Cleanup();

		m_LODStatus[0] = EMPTY;
		m_LODStatus[1] = EMPTY;
		m_LODStatus[2] = EMPTY;
		
		CS.Leave();
	}

	void Save( Stream& stream, PResourceFileDetails pDetails /*= NULL*/ )
	{
		//  write the version and title
		stream.Write(&GeometryFileVersion, sizeof(GeometryFileVersion));
		stream.Write(GeometryFileTitle, (int)strlen(GeometryFileTitle));

		//  create a header file
		GeometryFileHeader ghf;
		ZeroMemory(&ghf, sizeof(GeometryFileHeader));

		if (pDetails)
			memcpy(&ghf.details, pDetails, sizeof(ResourceFileDetails));
		else
		{
			memcpy(&ghf.details.Name, m_Src.Text(), sx_min_i(m_Src.Length()*2, 128));
			memcpy(&ghf.details.Author, L"SeganX Engine", 28);
			memcpy(&ghf.details.Comment, L"www.seganx.com", 30);
		}

		ghf.desc.VertexCount[0]	= m_APIGM[0].GetVertexCount();
		ghf.desc.VertexCount[1]	= m_APIGM[1].GetVertexCount();
		ghf.desc.VertexCount[2]	= m_APIGM[2].GetVertexCount();
		ghf.desc.FaceCount[0]	= m_APIGM[0].GetFaceCount();
		ghf.desc.FaceCount[1]	= m_APIGM[1].GetFaceCount();
		ghf.desc.FaceCount[2]	= m_APIGM[2].GetFaceCount();

		//  verify that this geometry has animation buffer
		if ( m_APIGM[0].VB_IsEmpty(3) )
			SEGAN_SET_REM(ghf.desc.Option, SX_GEOMETRY_OPTION_ANIMATED);
		else
			SEGAN_SET_ADD(ghf.desc.Option, SX_GEOMETRY_OPTION_ANIMATED);

		//  validate geometry bounding volumes
		if (!m_APIGM[0].VB_IsEmpty())
		{
			ghf.desc.Box	= m_APIGM[0].BoundingBox();
			ghf.desc.Sphere	= m_APIGM[0].BoundingSphere();
		}
		else if (!m_APIGM[1].VB_IsEmpty())
		{
			ghf.desc.Box	= m_APIGM[1].BoundingBox();
			ghf.desc.Sphere	= m_APIGM[1].BoundingSphere();
		}
		else if (!m_APIGM[2].VB_IsEmpty())
		{
			ghf.desc.Box	= m_APIGM[2].BoundingBox();
			ghf.desc.Sphere	= m_APIGM[2].BoundingSphere();
		}

		//	write header file
		UINT curpos = stream.GetPos();
		stream.Write(&ghf, sizeof(GeometryFileHeader));

		//  write each LOD
		ghf.levelPos[2] = stream.GetPos();
		SaveLevelToStream(2, stream);	

		ghf.levelPos[1] = stream.GetPos();
		SaveLevelToStream(1, stream);	

		ghf.levelPos[0] = stream.GetPos();
		SaveLevelToStream(0, stream);	

		//  update header file
		stream.SetPos(curpos);
		stream.Write(&ghf, sizeof(GeometryFileHeader));
	}

	void Load( Stream& stream )
	{
		//  at first read the version and title
		int fileVer = 0; char ch[1024];
		stream.Read(&fileVer, sizeof(fileVer));
		stream.Read(ch, (int)strlen(GeometryFileTitle));

		if (fileVer == 1)
		{
			//  read geometry header file
			GeometryFileHeader ghf;
			stream.Read(&ghf, sizeof(GeometryFileHeader));

			//  create needed buffers for geometries
			m_APIGM[0].VB_Create( ghf.desc.VertexCount[0], (ghf.desc.Option & SX_GEOMETRY_OPTION_ANIMATED) );
			m_APIGM[1].VB_Create( ghf.desc.VertexCount[1], (ghf.desc.Option & SX_GEOMETRY_OPTION_ANIMATED) );
			m_APIGM[2].VB_Create( ghf.desc.VertexCount[2], (ghf.desc.Option & SX_GEOMETRY_OPTION_ANIMATED) );

			m_APIGM[0].IB_Create( ghf.desc.FaceCount[0] );
			m_APIGM[1].IB_Create( ghf.desc.FaceCount[1] );
			m_APIGM[2].IB_Create( ghf.desc.FaceCount[2] );

			//  read each level in the file
			LoadLevelFromStream(2, stream);
			LoadLevelFromStream(1, stream);
			LoadLevelFromStream(0, stream);

			//  validate geometry
			m_LODStatus[0] = LOADED;
			m_LODStatus[1] = LOADED;
			m_LODStatus[2] = LOADED;			
		}
	}

	bool SaveLevelToStream( int level, Stream& stream )
	{
		MemoryStream mem;

		//  write each vertex buffer of specified geometry
		for (int i=0; i<4; i++)
		{
			m_APIGM[level].VB_UnLock(i);	// be sure that geometry is unlocked

			D3DVertexBufferDesc vbdesc;
			UINT n = m_APIGM[level].VB_GetDesc(i, vbdesc) ? vbdesc.Size : 0;

			void* p;
			if (m_APIGM[level].VB_Lock( i, (PD3DVertex&)p ))
			{
				mem.Write(&n, sizeof(UINT));
				mem.Write(p, n);
				m_APIGM[level].VB_UnLock(i);
			}
			else
			{
				n=0;
				mem.Write(&n, sizeof(UINT));
			}
		}

		//  write index buffer to the stream
		m_APIGM[level].IB_UnLock();	// be sure that geometry is unlocked

		D3DIndexBufferDesc ibdesc;
		UINT n = m_APIGM[level].IB_GetDesc(ibdesc) ? ibdesc.Size : 0;

		void* p;
		if ( m_APIGM[level].IB_Lock( (PDWORD&)p ))
		{
			mem.Write(&n, sizeof(UINT));
			mem.Write(p, n);
			m_APIGM[level].IB_UnLock();
		}
		else
		{
			n=0;
			mem.Write(&n, sizeof(UINT));
		}

		//  compress memory to the stream
		mem.Seek(ST_BEGIN);
		return sx::sys::ZCompressStream(mem, stream);
	}

	bool LoadLevelFromStream( int level, Stream& stream )
	{
		SEGAN_CLAMP(level, 0, 2);

		//  decompress stream to the memory
		MemoryStream mem;
		sx::sys::ZDecompressStream(stream, mem);
		mem.Seek(ST_BEGIN);

		//  load each vertex buffer data from stream to the specified geometry
		for (int i=0; i<4; i++)
		{
			UINT n;
			mem.Read(&n, sizeof(UINT));

			//  check the file rectification
			UINT m = m_APIGM[level].GetVertexCount();
			if(i==0) m *= SEGAN_SIZE_VERTEX_0;
			if(i==1) m *= SEGAN_SIZE_VERTEX_1;
			if(i==2) m *= SEGAN_SIZE_VERTEX_2;
			if(i==3) m *= SEGAN_SIZE_VERTEX_3;

			if (n && n==m)
			{
				m_APIGM[level].VB_UnLock(i);	// be sure that geometry is unlocked

				void* p;
				if (m_APIGM[level].VB_Lock( i, (PD3DVertex&)p ))
				{
					mem.Read(p, n);
					m_APIGM[level].VB_UnLock(i);
				}
				else
				{
					sxLog::Log(L"I can't lock the vertex buffer [%d] of geometry level [%d] to load '%s' !", i, level, *m_Src);
					Cleanup();
					return false;
				}
			}
		}

		//  read index buffer from the stream
		UINT n;
		mem.Read(&n, sizeof(UINT));
		if (n && n==m_APIGM[level].GetFaceCount()*12)
		{
			m_APIGM[level].IB_UnLock(); // be sure that geometry is unlocked

			void* p;
			if (m_APIGM[level].IB_Lock( (PDWORD&)p ))
			{
				mem.Read(p, n);
				m_APIGM[level].IB_UnLock();
			}
			else
			{
				sxLog::Log(L"I can't lock the index buffer of geometry level [%d] to load '%s' !", level, *m_Src);
				Cleanup();
				return false;
			}
		}

		m_LODStatus[level] = LOADED;
		return true;
	}

	void LoadByTaskManager( int LOD )
	{
		switch (LOD)
		{
		case 0:
			{
				CS.Enter();

				if ( m_LODStatus[2] == EMPTY )
				{
					m_LODStatus[2] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[2], GM_THREADID);
				}

				if ( m_LODStatus[1] == EMPTY )
				{
					m_LODStatus[1] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[1], GM_THREADID);
				}

				if ( m_LODStatus[0] == EMPTY )
				{
					m_LODStatus[0] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[0], GM_THREADID);
				}


				CS.Leave();
			}
			break;

		case 1:
			{
				CS.Enter();

				if ( m_LODStatus[2] == EMPTY )
				{
					m_LODStatus[2] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[2], GM_THREADID);
				}

				if ( m_LODStatus[1] == EMPTY )
				{
					m_LODStatus[1] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[1], GM_THREADID);
				}

				CS.Leave();
			}
			break;

		case 2:
			{
				CS.Enter();

				if ( m_LODStatus[2] == EMPTY )
				{
					m_LODStatus[2] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[2], GM_THREADID);
				}

				CS.Leave();
			}
			break;
		}


	}

	void LoadInThread(int level)
	{
		HANDLE lockEvent = CreateEvent(NULL, true, false, NULL);

		PStream file;
		if ( sx::sys::FileManager::File_Open(m_Src, SEGAN_PACKAGENAME_GEOMETRY, file) )
		{
			//  at first read the version and title
			int fileVer = 0; char ch[1024];
			file->Read(&fileVer, sizeof(fileVer));
			file->Read(ch, (int)strlen(GeometryFileTitle));

			if (fileVer == 1)
			{
				//  read geometry header file
				GeometryFileHeader ghf;
				file->Read(&ghf, sizeof(GeometryFileHeader));

				CS.Enter();
				while ( level > -1 && ghf.desc.VertexCount[ level ] == 0 )
				{
					m_LODStatus[level] = EMPTY;
					level--;
					if ( m_LODStatus[level] == EMPTY )
						m_LODStatus[level] = LOADING;
				}
				CS.Leave();

				if ( m_LODStatus[level] == LOADING )
				{
					//  find the correct file position
					file->SetPos(ghf.levelPos[level]);

					//  create needed tasks and objects
					sx::d3d::TaskGeometry3DCreateLock	lockTask(false);
					sx::d3d::TaskGeometry3DUnlock		unlockTask(false);
					TaskGeometryData					lockData;
				
					lockData.numVrtx	= ghf.desc.VertexCount[level];
					lockData.numFaces	= ghf.desc.FaceCount[level];
					lockData.animated	= (ghf.desc.Option & SX_GEOMETRY_OPTION_ANIMATED);

					//  send lock buffer command to the main task
					lockTask.SetData(&m_APIGM[level], &lockData, lockEvent);
					sx::sys::TaskManager::AddTask(&lockTask, 0);

					//  wait for receiving answer
					if (WaitForSingleObject(lockEvent, GM_MAX_LOCK_TIME) != WAIT_TIMEOUT)
					{
						//  decompress stream to the memory
						MemoryStream mem;
						sx::sys::ZDecompressStream(*file, mem);
						mem.Seek(ST_BEGIN);

						// verify that the geometry is still exist
						if (m_APIGM[level].GetVertexCount()!=lockData.numVrtx)	goto e_Exit;

						//  read the buffers
						UINT n = 0;
						UINT m = lockData.numVrtx;

						mem.Read(&n, sizeof(UINT));
						if ( n && n==(m*SEGAN_SIZE_VERTEX_0) && lockData.vb0 )
							mem.Read(lockData.vb0, n);

						mem.Read(&n, sizeof(UINT));
						if ( n && n==(m*SEGAN_SIZE_VERTEX_1) && lockData.vb1 )
							mem.Read(lockData.vb1, n);

						mem.Read(&n, sizeof(UINT));
						if ( n && n==(m*SEGAN_SIZE_VERTEX_2) && lockData.vb2 )
							mem.Read(lockData.vb2, n);

						mem.Read(&n, sizeof(UINT));
						if ( n && n==(m*SEGAN_SIZE_VERTEX_3) && lockData.vb3 )
							mem.Read(lockData.vb3, n);

						m = lockData.numFaces * 3;
						mem.Read(&n, sizeof(UINT));
						if ( n && n==(m*sizeof(DWORD)) && lockData.ib0 )
							mem.Read(lockData.ib0, n);
					}
					else
					{
						sx::sys::TaskManager::RemoveTask(&lockTask);
						sxLog::Log(L"waiting for lock buffers of '%s' has been timeout! so the object may not show", *m_Src);
						goto e_Exit;
					}//if (WaitForSingleObject(lockEvent, 1000

					//  send unlock buffer command to the main task
					unlockTask.SetData(&m_APIGM[level], lockEvent);
					sx::sys::TaskManager::AddTask(&unlockTask, 0);

					//  wait for receiving answer
					if (WaitForSingleObject(lockEvent, 10000) == WAIT_TIMEOUT)
					{
						sx::sys::TaskManager::RemoveTask(&unlockTask);
						sxLog::Log(L"waiting for unlock buffers of '%s' has been timeout! so the object may not show", *m_Src);
						goto e_Exit;
					}

					//  set the flag to say that this level has been loaded
					CS.Enter();
					m_LODStatus[level] = LOADED;
					CS.Leave();

	#ifdef SEGAN_LOG_LEVEL3
					sxLog::Log(L"geometry %s validated for level %d", *m_Src, level);
	#endif

				}//	if ( m_LODStatus[level] == EMPTY )

			}//if (fileVer == 1)

e_Exit:
			sx::sys::FileManager::File_Close(file);
		}//if ( sx::sys::FileManager::File_Open

		CloseHandle(lockEvent);
	}

	void CleanupInThread(void)
	{
		//  create needed tasks and objects
		sx::d3d::TaskGeometry3DCleanup	cleanupTask(false);
		HANDLE cleanupEvent	= CreateEvent(NULL, true, false, NULL);

		//  traverse between each level
		for (int i=0; i<3; i++)
		{				
			//  send cleanup buffer command to the main task
			cleanupTask.SetData(&m_APIGM[i], cleanupEvent);
			sx::sys::TaskManager::AddTask(&cleanupTask, 0);

			//  wait for receiving answer
			if (WaitForSingleObject(cleanupEvent, GM_MAX_LOCK_TIME) == WAIT_TIMEOUT)
			{
				sxLog::Log(L"waiting for cleanup buffers of '%s' has been timeout! so the object may not cleanup", *m_Src);
			}
		}

		CloseHandle(cleanupEvent);

		CS.Enter();
		m_LODStatus[0] = EMPTY;
		m_LODStatus[1] = EMPTY;
		m_LODStatus[2] = EMPTY;
		CS.Leave();
	}

public:
	String				m_Src;				//  address of resource file
	DWORD						m_ID;				//  I just feel that it will be useful for fast search :)
	DWORD						m_Optn;				//  I just feel that it will be useful :)
	GeometryFileHeader			m_Header;			//  geometry header file describe the geometry information

	sx::d3d::Geometry3D			m_APIGM[3];			//  the API geometry resource
	int 						m_APIRefCount;		//  number of references to API resource
	int							m_RefCount;			//  number of references to this object

	LODStatus					m_LODStatus[3];		//  status of each LOD
	DrivedTaskGeometryLoader	m_LoaderTask[3];	//  will send to task manager to load data for 3 LOD
	DrivedTaskGeometryCleanup	m_CleanupTask;		//  will send to task manager to cleanup data
};

//////////////////////////////////////////////////////////////////////////
//	GEOMETRY LOADER TASK
//////////////////////////////////////////////////////////////////////////
DrivedTaskGeometryLoader::DrivedTaskGeometryLoader(): TaskBase(false), m_geometry(NULL) {}
void DrivedTaskGeometryLoader::Execute( void )
{
	if (m_geometry)
		m_geometry->LoadInThread(m_LOD);
}

//////////////////////////////////////////////////////////////////////////
//	GEOMETRY CLEANUP TASK
//////////////////////////////////////////////////////////////////////////
DrivedTaskGeometryCleanup::DrivedTaskGeometryCleanup(): TaskBase(false), m_geometry(NULL) {}
void DrivedTaskGeometryCleanup::Execute( void )
{
	if (m_geometry)
		m_geometry->CleanupInThread();
}

//////////////////////////////////////////////////////////////////////////
//	GEOMETRY DESTROY TASK
//////////////////////////////////////////////////////////////////////////
DrivedTaskGeometryDestroy::DrivedTaskGeometryDestroy(): TaskBase(false), m_geometry(NULL) {}
void DrivedTaskGeometryDestroy::Execute( void )
{
	// verify that this task is in main thread
	if ((BYTE)m_flag != 0)
	{
		sx::sys::TaskManager::AddTask(this, 0);
		return;
	}

	if (m_geometry)
	{
		//  check to see that this geometry use tasks in task manager
		if (m_geometry->m_LoaderTask[0].Executing() || 
			m_geometry->m_LoaderTask[1].Executing() || 
			m_geometry->m_LoaderTask[2].Executing() || 
			m_geometry->m_CleanupTask.Executing())
		{
			//  post this task to the end of the queue until the other threads finished
			sx::sys::TaskManager::AddTask(this, 0);
		}
		else
		{
			sx_delete_and_null(m_geometry);

			DrivedTaskGeometryDestroy* me = this;
			sx_delete_and_null(me);
		}
	}
}


namespace sx { namespace d3d
{
	//////////////////////////////////////////////////////////////////////////
	//  GEOMETRY INTERNAL MANAGER
	//////////////////////////////////////////////////////////////////////////
	bool Geometry::Manager::Exist( OUT PGeometry& pGmtry, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);
		return GeometryMap.Find(key, pGmtry);
	}

	bool Geometry::Manager::Get( OUT PGeometry& pGmtry, const WCHAR* src )
	{
		if (!src) return false;

		UINT key = sx::cmn::GetCRC32(src);
		
		if (GeometryMap.Find(key, pGmtry))
		{
			DrivedGeometry* g = (DrivedGeometry*)pGmtry;
			g->m_RefCount++;
		}
		else
		{
			if (Create(pGmtry, src))
			{
				GeometryMap.Insert(key, pGmtry);
			}
			else return false;
		}

		return true;		
	}

	bool Geometry::Manager::Create( OUT PGeometry& pGmtry, const WCHAR* src )
	{
		pGmtry = sx_new( DrivedGeometry );
		if (pGmtry)
		{
			pGmtry->SetSource(src);
			return true;
		}
		else return false;
	}

	void Geometry::Manager::Release( PGeometry& pGmtry )
	{
		if (!pGmtry) return;
		DrivedGeometry* g = (DrivedGeometry*)pGmtry;

		pGmtry = NULL;
		g->m_RefCount--;
		if (g->m_RefCount!=0) return;

		//  remove from geometry map
		PGeometry gm = NULL;
		if (GeometryMap.Find(g->m_ID, gm) && gm==g)
			GeometryMap.Remove(g->m_ID);

		//  remove any tasks exist in task list
		sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[0]);
		sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[1]);
		sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[2]);
		sx::sys::TaskManager::RemoveTask(&g->m_CleanupTask);

		//  check to see that this geometry use tasks in task manager
		if (g->m_LoaderTask[0].Executing() || 
			g->m_LoaderTask[1].Executing() || 
			g->m_LoaderTask[2].Executing() || 
			g->m_CleanupTask.Executing())
		{
			DrivedTaskGeometryDestroy* freeTask = sx_new( DrivedTaskGeometryDestroy );
			freeTask->m_geometry = g;

			//  send this task to the main thread
			sx::sys::TaskManager::AddTask(freeTask, 0);
		}
		else
		{
			sx_delete_and_null(g);
		}
	}

	void Geometry::Manager::ClearAll( void )
	{
		if (GeometryMap.IsEmpty()) return;
		sx_callstack();

		for (sxMapGeometry::Iterator it = GeometryMap.First(); !it.IsLast(); it++)
		{
			DrivedGeometry* g = (DrivedGeometry*)(*it);
	
			//  remove any tasks exist in task list
			sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[0]);
			sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[1]);
			sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[2]);
			sx::sys::TaskManager::RemoveTask(&g->m_CleanupTask);

			//  check to see that this geometry use tasks in task manager
			if (g->m_LoaderTask[0].Executing() ||
				g->m_LoaderTask[1].Executing() ||
				g->m_LoaderTask[2].Executing() ||
				g->m_CleanupTask.Executing())
			{
				DrivedTaskGeometryDestroy* freeTask = sx_new( DrivedTaskGeometryDestroy );
				freeTask->m_geometry = g;

				//  send this task to the main thread
				sx::sys::TaskManager::AddTask(freeTask, 0);
			}
			else
			{
				sx_delete_and_null(g);
			}
		}
		GeometryMap.Clear();
	}

	Map<UINT, sx::d3d::PGeometry>::Iterator& Geometry::Manager::GetFirst( void )
	{
		static sxMapGeometry::Iterator it = GeometryMap.First();
		it = GeometryMap.First();
		return it;
	}

	bool& Geometry::Manager::LoadInThread( void )
	{
		return s_geometryLoadInThread;
	}
} } // namespace sx { namespace d3d
