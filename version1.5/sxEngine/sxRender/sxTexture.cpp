#include "sxTexture.h"
#include "sxResource3D.h"
#include "sxTask3D.h"
#include "../sxCommon/sxCommon.h"


#define TX_THREADID					2
#define TX_MAX_LOCK_TIME			5000	// ms

enum  LODStatus { EMPTY, LOADING, LOADED };

typedef Map<UINT, sx::d3d::PTexture> sxMapTexture;
static 	sxMapTexture TextureMap;	//  hold the created texture objects

static const int	TextureFileVersion = 1;
static const char*	TextureTitleFile = "SeganX Texture File - sajad.b@gmail.com";
static bool			s_loadInThread = false;

//  will use to change shared variables in other threads
static CriticalSection CS;

/*
this is the header file of the texture and of course will be written after version and title ;)
*/
struct TextureHeaderFile 
{
	ResourceFileDetails	details;
	TextureDesc			desc;				//  texture description
	UINT				levelPos[16];		//  position of each level in the file
};

//---	FILE STRUCTURE	---
//	meta file
//		|	title & version
//	header file
//		|	description
//		|	LOD positions
//
//	LOD	2
//		|	size of level n	  |	  surface data
//		|	size of level n-1 |	  surface data
//		|	size of level n-2 |	  surface data
//		|	size of level n-3 |	  surface data
//		|	size of level ... |	  surface data
//		|	size of level 2	  |	  surface data
//
//	LOD	1
//		|	size of level 1	  |	  surface data
//
//	LOD	0
//		|	size of level 0	  |	  surface data
//-------------------------


UINT GetSurfaceSizeInByte(D3DFormat& format, UINT width, UINT height)
{
	switch (format)
	{
	case D3DFMT_R8G8B8:        return  width * height * 3;
	case D3DFMT_A8R8G8B8:      return  width * height * 4;
	case D3DFMT_X8R8G8B8:      return  width * height * 4;
	case D3DFMT_R5G6B5:        return  width * height * 2;
	case D3DFMT_X1R5G5B5:      return  width * height * 2;
	case D3DFMT_A1R5G5B5:      return  width * height * 2;
	case D3DFMT_A4R4G4B4:      return  width * height * 2;
	case D3DFMT_R3G3B2:        return  width * height * 1;
	case D3DFMT_A8:            return  width * height * 1;
	case D3DFMT_A8R3G3B2:      return  width * height * 2;
	case D3DFMT_X4R4G4B4:      return  width * height * 2;
	case D3DFMT_A2B10G10R10:   return  width * height * 4;
	case D3DFMT_A8B8G8R8:      return  width * height * 4;
	case D3DFMT_X8B8G8R8:      return  width * height * 4;
	case D3DFMT_G16R16:        return  width * height * 4;
	case D3DFMT_A2R10G10B10:   return  width * height * 4;
	case D3DFMT_A16B16G16R16:  return  width * height * 8;
	case D3DFMT_R16F:          return  width * height * 2;
	case D3DFMT_G16R16F:       return  width * height * 4;
	case D3DFMT_A16B16G16R16F: return  width * height * 8;
	case D3DFMT_R32F:          return  width * height * 4;
	case D3DFMT_G32R32F:       return  width * height * 8;
	case D3DFMT_A32B32G32R32F: return  width * height * 16;
	case D3DFMT_DXT5:          return  width * height * 1;
	case D3DFMT_DXT4:          return  width * height * 1;
	case D3DFMT_DXT3:          return  width * height * 1;
	case D3DFMT_DXT2:          return  width * height * 1;
	case D3DFMT_DXT1:
		{
			UINT res = (width * height) / 2;
			return res>7 ? res : 8;
		}
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//	DRIVED TEXTURE
//////////////////////////////////////////////////////////////////////////
class DerivedTexture : public sx::d3d::Texture
{

	SEGAN_STERILE_CLASS(DerivedTexture);

public:
	DerivedTexture( void ): m_Optn(0), m_RefCount(1), m_activeCount(0), m_LOD(0)
	{
		m_ID = sx::cmn::ID_Generate(10);
		m_CleanupTask.m_texture = this;

		for (int i=0; i<3; i++)
		{
			m_LODStatus[i] = EMPTY;
			m_LoaderTask[i].m_texture = this;
			m_LoaderTask[i].m_LOD = i;
		}
	}

	virtual ~DerivedTexture( void )
	{
		Cleanup();

		//  remove any tasks exist in task list
		CS.Enter();
		sx::sys::TaskManager::RemoveTask(&m_LoaderTask[0]);
		sx::sys::TaskManager::RemoveTask(&m_LoaderTask[1]);
		sx::sys::TaskManager::RemoveTask(&m_LoaderTask[2]);
		sx::sys::TaskManager::RemoveTask(&m_CleanupTask);
		CS.Leave();
	}

	FORCEINLINE PDirect3DBaseTexture GetD3DTexture( void )
	{
		return m_APITX.GetD3DTexture();
	}

	bool CreateTextureEx(D3DResourceType type, UINT width, UINT height, UINT levels, DWORD usage, D3DFormat format, D3DPool pool)
	{
		Cleanup();

		if ( m_APITX.CreateTextureEx(type, width, height, levels, usage, format, pool) )
		{
			m_LODStatus[0] = LOADED;
			m_LODStatus[1] = LOADED;
			m_LODStatus[2] = LOADED;
			return true;
		}

		return false;
	}

	bool CreateRenderTarget(D3DResourceType type, UINT width, UINT height, D3DFormat format)
	{
		Cleanup();

		if ( m_APITX.CreateRenderTarget(type, width, height, format) )
		{
			m_LODStatus[0] = LOADED;
			m_LODStatus[1] = LOADED;
			m_LODStatus[2] = LOADED;
			return true;
		}

		return false;
	}

	bool CreateTexture(D3DResourceType type, UINT width, D3DFormat format = D3DFMT_DXT5)
	{
		Cleanup();

		if ( m_APITX.CreateTexture(type, width, format) )
		{
			m_LODStatus[0] = LOADED;
			m_LODStatus[1] = LOADED;
			m_LODStatus[2] = LOADED;
			return true;
		}

		return false;
	}

	FORCEINLINE void SetAsRenderTarget(int index, int cubeFace)
	{
		m_APITX.SetAsRenderTarget(index, cubeFace);
	}

	FORCEINLINE void SetAsDepthStencil(void)
	{
		m_APITX.SetAsDepthStencil();
	}

	bool LoadFromImageFile(const WCHAR* FileName, D3DFormat format = D3DFMT_DXT5)
	{
		Cleanup();

		if ( m_APITX.LoadFromImageFile(FileName, format) )
		{
			m_LODStatus[0] = LOADED;
			m_LODStatus[1] = LOADED;
			m_LODStatus[2] = LOADED;
			return true;
		}

		return false;
	}

	const WCHAR* SaveToImageFile(const WCHAR* FileName, const WCHAR* format)
	{
		WCHAR* res = NULL;

		if ( m_LODStatus[0] != LOADED )
		{
			//  load texture to save
			PStream file = NULL;
			if ( sx::sys::FileManager::File_Open(m_Src, SEGAN_PACKAGENAME_TEXTURE, file) )
			{
				Load(*file, false);

				sx::sys::FileManager::File_Close(file);
			}

			//  save texture to file
			res = (WCHAR*)m_APITX.SaveToImageFile(FileName, format);

			//  clear loaded image
			Cleanup();
		}
		else
		{
			//  save texture to file
			res = (WCHAR*)m_APITX.SaveToImageFile(FileName, format);
		}

		return res;
	}

	FORCEINLINE bool GetDesc(TextureDesc& desc)
	{
		return m_APITX.GetDesc( desc );
	}

	FORCEINLINE bool Lock(UINT level, PD3DLockedRect pRectArray)
	{
		return m_APITX.Lock(level, pRectArray);
	}

	FORCEINLINE void Unlock(UINT level)
	{
		m_APITX.Unlock(level);
	}

	FORCEINLINE void SetLOD( int LOD )
	{
		sx_assert(LOD>=0);

		m_LOD = LOD;
	}

	FORCEINLINE int GetLOD( void )
	{
		return m_LOD;
	}

	FORCEINLINE void SetToDevice( UINT stage /*= 0*/ )
	{
		sx_assert(stage>=0);

		switch (m_LOD)
		{
		case 0:
			{
				if ( m_LODStatus[0] == LOADED )
					m_APITX.SetLOD(0);
				else if ( m_LODStatus[1] == LOADED )
					m_APITX.SetLOD(1);
				else
					m_APITX.SetLOD(2);
			}
			break;

		case 1:
			{
				if ( m_LODStatus[1] == LOADED )
					m_APITX.SetLOD(1);
				else
					m_APITX.SetLOD(2);
			}
			break;

		case 2:
			{
				m_APITX.SetLOD(2);
			}
			break;

		default : m_APITX.SetLOD(m_LOD);
		}

		m_APITX.SetToDevice(stage);
	}

	FORCEINLINE DWORD GetID( void )
	{
		return m_ID;
	}

	void SetSource( const WCHAR* srcAddress )
	{
		sx_callstack_push(DerivedTexture::SetSource(srcAddress=%s), srcAddress);

		UINT id = sx::cmn::GetCRC32(srcAddress);
		if (id == m_ID) return;

		sx::d3d::PTexture tx = NULL;
		if (TextureMap.Find(m_ID, tx) && tx==this)
		{
			TextureMap.Remove(m_ID);
			TextureMap.Insert(id, this);
		}

		m_Src = srcAddress;
		m_ID = id;
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

	void Activate( int LOD /*= 0*/ )
	{
		sx_callstack_push(DerivedTexture::Activate(LOD=%d), LOD);

		if ( !s_loadInThread )
		{
			if ( m_LODStatus[0] == EMPTY )
			{
				PStream file;
				if ( sx::sys::FileManager::File_Open( m_Src, SEGAN_PACKAGENAME_TEXTURE, file ) )
				{
					Load( *file, true );
					sx::sys::FileManager::File_Close( file );
				}
			}
			
			m_activeCount++;
			return;
		}

		switch ( LOD )
		{
		case 0:		m_activeCount++;	LoadByTaskManager(0);	break;
		case 1:		m_activeCount++;	LoadByTaskManager(1);	break;
		case 2:		m_activeCount++;	LoadByTaskManager(2);	break;
		}
	}

	void Deactivate( void )
	{
		if ( !m_activeCount ) return;

		m_activeCount--;

		if ( !m_activeCount )
		{
			//  verify that this texture use tasks in task manager
			if ( m_LoaderTask[0].Executing() || m_LoaderTask[1].Executing() || m_LoaderTask[2].Executing() )
			{
				//  clear any load task
				sx::sys::TaskManager::RemoveTask(&m_LoaderTask[0]);
				sx::sys::TaskManager::RemoveTask(&m_LoaderTask[1]);
				sx::sys::TaskManager::RemoveTask(&m_LoaderTask[2]);
				sx::sys::TaskManager::RemoveTask(&m_CleanupTask);

				//  send cleanup task to the task manager
				//sxLog::Log(L"send cleanup task to the task manager");
				sx::sys::TaskManager::AddTask(&m_CleanupTask, TX_THREADID);

				//  now the current texture has been cleaned up
				CS.Enter();
				m_LODStatus[0] = EMPTY;
				m_LODStatus[1] = EMPTY;
				m_LODStatus[2] = EMPTY;
				CS.Leave();
			}
			else if ( m_CleanupTask.Executing() )
			{
				sxLog::Log(L"texture cleanup is executing !!");
			}
			else
			{
				//sxLog::Log(L"start texture cleanup");
				Cleanup();
				//sxLog::Log(L"end texture cleanup");
			}
		}
	}

	void Cleanup( void )
	{
		sx_callstack_push(DerivedTexture::Cleanup());

		m_APITX.Cleanup();

		CS.Enter();
		m_LODStatus[0] = EMPTY;
		m_LODStatus[1] = EMPTY;
		m_LODStatus[2] = EMPTY;
		CS.Leave();
	}

	void Save( Stream& stream, PResourceFileDetails pDetails /*= NULL*/ )
	{
		if ( !m_APITX.GetD3DTexture() ) return;

		//  write the version and title
		stream.Write(&TextureFileVersion, sizeof(TextureFileVersion));
		stream.Write(TextureTitleFile, (int)strlen(TextureTitleFile));

		//  create a header file
		TextureHeaderFile thf;
		ZeroMemory(&thf, sizeof(TextureHeaderFile));
		if (pDetails)
			memcpy(&thf.details, pDetails, sizeof(ResourceFileDetails));
		else
		{
			memcpy(&thf.details.Name, m_Src.Text(), sx_min_i(m_Src.Length()*2, 128));
			memcpy(&thf.details.Author, L"SeganX Engine", 28);
			memcpy(&thf.details.Comment, L"www.seganx.com", 30);
		}
		m_APITX.GetDesc(thf.desc);

		//	write header file
		UINT curpos = stream.GetPos();
		stream.Write(&thf, sizeof(TextureHeaderFile));

		//  write each LOD
		for (int i=(int)thf.desc.Levels-1; i>=0; i--)
		{
			thf.levelPos[i] = stream.GetPos();
			SaveLevelToStream(i, stream);	
		}

		//  update header file
		stream.SetPos(curpos);
		stream.Write(&thf, sizeof(TextureHeaderFile));
		stream.Seek(ST_END);
	}

	void Load( Stream& stream, bool limitSize /*= true*/ )
	{
		sx_callstack_push(DerivedTexture::Load(stream, limitSize));

		//  at first read the version and title
		int fileVer = 0; char ch[1024];
		stream.Read(&fileVer, sizeof(fileVer));
		stream.Read(ch, (int)strlen(TextureTitleFile));

		if (fileVer == 1)
		{
			//  read texture header file
			TextureHeaderFile thf;
			stream.Read(&thf, sizeof(TextureHeaderFile));

			//  create needed buffers for texture
			m_APITX.CreateTexture(thf.desc.rType, thf.desc.Width, thf.desc.Format, limitSize);

			//  read each level from the file
			for (int i=(int)thf.desc.Levels-1; i>=0; i--)
			{
				TextureDesc desc;
				if (m_APITX.GetDesc(desc))
				{			
					int d = thf.desc.Levels - desc.Levels;
					stream.SetPos(thf.levelPos[i + d]);
				}
				
				LoadLevelFromStream(i, stream);
			}

			//  validate texture
			m_LODStatus[0] = LOADED;
			m_LODStatus[1] = LOADED;
			m_LODStatus[2] = LOADED;
		}
	}

	void Reload( void )
	{
		PStream file;
		if ( sx::sys::FileManager::File_Open( m_Src, SEGAN_PACKAGENAME_TEXTURE, file ) )
		{
			Load( *file, true );
			sx::sys::FileManager::File_Close( file );
		}
	}

	FORCEINLINE bool Activated( void )
	{
		return m_LODStatus[0] == LOADED && m_LODStatus[1] == LOADED && m_LODStatus[2] == LOADED;
	}

	bool SaveLevelToStream( UINT level, Stream& stream )
	{
		D3DLockedRect rect[6];
		ZeroMemory(rect, sizeof(D3DLockedRect)*6);

		TextureDesc tDesc;
		if ( !m_APITX.GetDesc(tDesc) ) return false;

		int W = int ( tDesc.Width / (pow(2.0f, (int)level)) );

		if (m_APITX.Lock(level, rect))
		{
			MemoryStream mem;

			for (int i=0; i<6; i++)
			{
				UINT n = rect[i].pBits ? GetSurfaceSizeInByte(tDesc.Format, W, W) : 0;
				mem.Write(&n, sizeof(UINT));

				if (n) mem.Write(rect[i].pBits, n);
			}
			m_APITX.Unlock(level);

			//  compress memory to the stream
			mem.Seek(ST_BEGIN);
			return sx::sys::ZCompressStream(mem, stream);
		}

		return false;
	}

	bool LoadLevelFromStream( UINT level, Stream& stream )
	{
		D3DLockedRect rect[6];
		ZeroMemory(rect, sizeof(D3DLockedRect)*6);

		TextureDesc tDesc;
		if (!m_APITX.GetDesc(tDesc)) return false;
		int W = int ( tDesc.Width / (pow(2.0f, (int)level)) );

		//  decompress stream to the memory
		MemoryStream mem;
		if (sx::sys::ZDecompressStream(stream, mem))
		{
			mem.Seek(ST_BEGIN);		

			if (m_APITX.Lock(level, rect))
			{
				for (int i=0; i<6; i++)
				{
					UINT n = rect[i].pBits ? GetSurfaceSizeInByte(tDesc.Format, W, W) : 0;
					
					UINT m = 0;
					mem.Read(&m, sizeof(UINT));
					if (n && n==m) 
						mem.Read(rect[i].pBits, n);
				}
				m_APITX.Unlock(level);
			}
		}

		return true;
	}

	void LoadByTaskManager( int LOD )
	{
		switch (LOD)
		{
		case 0:
			{
				CS.Enter();

				if ( m_LODStatus[0] == EMPTY )
				{
					m_LODStatus[0] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[0], TX_THREADID);
				}

				if ( m_LODStatus[1] == EMPTY )
				{
					m_LODStatus[1] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[1], TX_THREADID);
				}

				if ( m_LODStatus[2] == EMPTY )
				{
					m_LODStatus[2] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[2], TX_THREADID);
				}

				CS.Leave();
			}
			break;

		case 1:
			{
				CS.Enter();

				if ( m_LODStatus[1] == EMPTY )
				{
					m_LODStatus[1] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[1], TX_THREADID);
				}

				if ( m_LODStatus[2] == EMPTY )
				{
					m_LODStatus[2] = LOADING;
					sx::sys::TaskManager::AddTask(&m_LoaderTask[2], TX_THREADID);
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
					sx::sys::TaskManager::AddTask(&m_LoaderTask[2], TX_THREADID);
				}

				CS.Leave();
			}
			break;
		}
	}

	bool LoadLevel(int level, PStream file, TextureHeaderFile* pTHF)
	{
		bool result = true;

		//  create needed tasks and objects
		HANDLE lockEvent					= CreateEvent(NULL, true, false, NULL);
		sx::d3d::TaskTexture3DCreateLock	lockTask(false);
		sx::d3d::TaskTexture3DUnlock		unlockTask(false);
		TaskTextureData						lockData;
		lockData.createWidth				= pTHF->desc.Width;
		lockData.rType						= pTHF->desc.rType;
		lockData.rFotmat					= pTHF->desc.Format;

		//  send lock buffer command to the main task
		lockData.levelToLock	= level;
		lockTask.SetData(&m_APITX, &lockData, lockEvent);
		sx::sys::TaskManager::AddTask(&lockTask, 0);

		//  wait for receiving answer
		if (WaitForSingleObject(lockEvent, TX_MAX_LOCK_TIME) != WAIT_TIMEOUT)
		{
			//  find the correct file position
			TextureDesc desc;
			if (m_APITX.GetDesc(desc))
			{	
				if (desc.Levels != pTHF->desc.Levels)
				{
					int d = pTHF->desc.Levels - desc.Levels;
					for (int i=d; i<16; i++)
					{
						pTHF->levelPos[i-d] = pTHF->levelPos[i];
					}

					pTHF->desc.Width  = desc.Width;
					pTHF->desc.Levels = desc.Levels;
				}

				file->SetPos( pTHF->levelPos[level] );
			}

			//  decompress stream to the memory
			MemoryStream mem;
			if (sx::sys::ZDecompressStream(*file, mem))
			{
				mem.Seek(ST_BEGIN);

				//  read the buffers
				int W = int ( desc.Width / (pow(2.0f, (int)level)) );

				for (int i=0; i<6; i++)
				{
					UINT n = lockData.rect[i].pBits ? GetSurfaceSizeInByte(desc.Format, W, W) : 0;
					
					UINT m = 0;
					mem.Read(&m, sizeof(UINT));
					if (n && n==m)
					{
						if( m_APITX.GetD3DTexture() != NULL )
							mem.Read(lockData.rect[i].pBits, n);
						else
						{
							result = false;
							goto e_Exit;
						}
					}
				}
			}						
		}
		else
		{
			sx::sys::TaskManager::RemoveTask(&lockTask);
			sxLog::Log(L"waiting for lock buffers of '%s' has been timeout! so the object may not show", *m_Src);
			result = false;
			goto e_Exit;
		}//if (WaitForSingleObject(lockEvent, 1000

		//  send unlock buffer command to the main task
		unlockTask.SetData(&m_APITX, level, lockEvent);
		sx::sys::TaskManager::AddTask(&unlockTask, 0);

		//  wait for receiving answer
		if (WaitForSingleObject(lockEvent, TX_MAX_LOCK_TIME) == WAIT_TIMEOUT)
		{
			sx::sys::TaskManager::RemoveTask(&unlockTask);
			sxLog::Log(L"waiting for unlock buffers of '%s' has been timeout! so the object may not show", *m_Src);
			goto e_Exit;
		}

e_Exit:
		CloseHandle(lockEvent);
		return result;
	}

	void LoadInThread(int level)
	{
		SEGAN_CLAMP( level, 0, 2 );
		if  ( m_LODStatus[level] != LOADING ) return;

		PStream file;
		if ( sx::sys::FileManager::File_Open(m_Src, SEGAN_PACKAGENAME_TEXTURE, file) )
		{
			//  at first read the version and title
			int fileVer = 0; char ch[1024];
			file->Read(&fileVer, sizeof(fileVer));
			file->Read(ch, (int)strlen(TextureTitleFile));

			if (fileVer == 1)
			{
				//  read texture header file
				TextureHeaderFile thf;
				file->Read(&thf, sizeof(TextureHeaderFile));

				switch (level)
				{
				case 0:	
					{
						LoadLevel(0, file, &thf);
						CS.Enter();
						m_LODStatus[0] = LOADED;
						CS.Leave();
					}
					break;

				case 1:
					{
						LoadLevel(1, file, &thf);
						CS.Enter();
						m_LODStatus[1] = LOADED;
						CS.Leave();

						//  exception format used in GUIs
						if ( thf.desc.Format == D3DFMT_A8R8G8B8 )
						{
							if ( m_LODStatus[0] != LOADED )
							{
								LoadLevel(0, file, &thf);
								CS.Enter();
								m_LODStatus[0] = LOADED;
								CS.Leave();
							}
						}
					}
					break;

				case 2:
					{
						for (int i=2; i<(int)thf.desc.Levels; i++)
						{
							LoadLevel(i, file, &thf);
						}
						CS.Enter();
						m_LODStatus[2] = LOADED;
						CS.Leave();

						//  exception format used in GUIs
						if ( thf.desc.Format == D3DFMT_A8R8G8B8 )
						{
							if ( m_LODStatus[1] != LOADED )
							{
								LoadLevel(1, file, &thf);
								CS.Enter();
								m_LODStatus[1] = LOADED;
								CS.Leave();
							}

							if ( m_LODStatus[0] != LOADED )
							{
								LoadLevel(0, file, &thf);
								CS.Enter();
								m_LODStatus[0] = LOADED;
								CS.Leave();
							}
						}
					}
					break;

				default:	sxLog::Log(L"Invalid file format for '%s' on level %d", *m_Src, level);
				}
				

			}//if (fileVer == 1)

			sx::sys::FileManager::File_Close(file);
		}//if ( sx::sys::FileManager::File_Open
	}

	void CleanupInThread(void)
	{
		//  create needed tasks and objects
		sx::d3d::TaskTexture3DCleanup	cleanupTask(false);
		HANDLE cleanupEvent	= CreateEvent(NULL, true, false, NULL);

		//  send cleanup buffer command to the main task
		cleanupTask.SetData(&m_APITX, cleanupEvent);
		sx::sys::TaskManager::AddTask(&cleanupTask, 0);

		//  wait for receiving answer
		if (WaitForSingleObject(cleanupEvent, TX_MAX_LOCK_TIME) == WAIT_TIMEOUT)
		{
			sxLog::Log(L"waiting for cleanup buffers of '%s' has been timeout! so the object may not cleanup", *m_Src);
		}
		
		CloseHandle(cleanupEvent);
	}
public:
	String				m_Src;				//  Address of resource file
	DWORD						m_ID;				//  I just feel that it will be useful for fast search :)
	DWORD						m_Optn;				//  I just feel that it will be useful :)

	sx::d3d::Texture3D			m_APITX;			//  The API texture resource
	int 						m_activeCount;		//  number of references to API resource
	int							m_RefCount;			//  number of references to objects

	int							m_LOD;				//  hold the current LOD
	LODStatus					m_LODStatus[3];		//  status of each LOD
	DerivedTaskTextureLoader	m_LoaderTask[3];	//  will send to task manager to load data for 3 LOD
	DerivedTaskTextureCleanup	m_CleanupTask;		//  will send to task manager to cleanup data
};

//////////////////////////////////////////////////////////////////////////
//	TEXTURE LOADER TASK
//////////////////////////////////////////////////////////////////////////
DerivedTaskTextureLoader::DerivedTaskTextureLoader(): TaskBase(false), m_LOD(0), m_texture(NULL) {}
void DerivedTaskTextureLoader::Execute( void )
{
	if ( !m_texture ) return;
	if ( !sx::sys::FileManager::File_Exist( m_texture->GetSource(), SEGAN_PACKAGENAME_TEXTURE ) )
	{
		return;
	}

	switch (m_LOD)
	{
	case 0:
		{
			if ( m_texture->m_LODStatus[1] != LOADED )
				sx::sys::TaskManager::AddTask(this, TX_THREADID);
			else
				m_texture->LoadInThread(0);
		}
		break;

	case 1:
		{
			if ( m_texture->m_LODStatus[2] != LOADED )
				sx::sys::TaskManager::AddTask(this, TX_THREADID);
			else
				m_texture->LoadInThread(1);
		}
		break;

	case 2:
		{
			m_texture->LoadInThread(2);
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//	TEXTURE CLEANUP TASK
//////////////////////////////////////////////////////////////////////////
DerivedTaskTextureCleanup::DerivedTaskTextureCleanup(): TaskBase(false), m_texture(NULL) {}
void DerivedTaskTextureCleanup::Execute( void )
{
	if (m_texture)
		m_texture->CleanupInThread();
}

//////////////////////////////////////////////////////////////////////////
//	TEXTURE DESTROY TASK
//////////////////////////////////////////////////////////////////////////
DerivedTaskTextureDestroy::DerivedTaskTextureDestroy(): TaskBase(false), m_texture(NULL) {}
void DerivedTaskTextureDestroy::Execute( void )
{
	// verify that this task is in main thread
	if ( (BYTE)m_flag != 0 )
	{
		sx::sys::TaskManager::AddTask(this, 0);
		return;
	}

	if (m_texture)
	{
		//  check to see that this texture use tasks in task manager
		if (m_texture->m_LoaderTask[0].Executing() ||
			m_texture->m_LoaderTask[1].Executing() ||
			m_texture->m_LoaderTask[2].Executing() ||
			m_texture->m_CleanupTask.Executing())
		{
			//  post this task to the end of the queue until the other threads finished
			sx::sys::TaskManager::AddTask(this, 0);
		}
		else
		{
			sx_delete_and_null( m_texture );

			DerivedTaskTextureDestroy* me = this;
			sx_delete_and_null( me );
		}
	}
}


namespace sx { namespace d3d
{
	//////////////////////////////////////////////////////////////////////////
	//  TEXTURE INTERNAL MANAGER
	//////////////////////////////////////////////////////////////////////////
	bool Texture::Manager::Exist( OUT PTexture& pTxur, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);
		return TextureMap.Find(key, pTxur);
	}

	bool Texture::Manager::Get( OUT PTexture& pTxur, const WCHAR* src )
	{
		sx_callstack_push(Texture::Manager::Get(src=%s), src);

		if (!src) 
		{
			pTxur = NULL;
			return false;
		}

		UINT key = sx::cmn::GetCRC32(src);

		if (TextureMap.Find(key, pTxur))
		{
			DerivedTexture* g = (DerivedTexture*)pTxur;
			g->m_RefCount++;
		}
		else
		{
			if (Create(pTxur, src))
			{
				TextureMap.Insert(key, pTxur);
			}
			else return false;
		}

		return true;		
	}

	bool Texture::Manager::Create( OUT PTexture& pTxur, const WCHAR* src )
	{
		sx_callstack_push(Texture::Manager::Create(src=%s), src);

		pTxur = sx_new( DerivedTexture );
		if (pTxur)
		{
			pTxur->SetSource(src);
			return true;
		}
		else return false;
	}

	void Texture::Manager::AddRef( PTexture pTxur )
	{
		if (!pTxur) return;
		DerivedTexture* g = (DerivedTexture*)pTxur;
		g->m_RefCount++;
		g->m_activeCount++;
	}

	void Texture::Manager::Release( PTexture& pTxur )
	{
		if (!pTxur) return;
		DerivedTexture* g = (DerivedTexture*)pTxur;

		pTxur = NULL;
		g->m_RefCount--;
		if (g->m_RefCount>0) return;

		//  remove from texture map
		PTexture tx = NULL;
		if (TextureMap.Find(g->m_ID, tx) && tx==g)
			TextureMap.Remove(g->m_ID);

		//  remove any tasks exist in task list
		CS.Enter();
		sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[0]);
		sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[1]);
		sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[2]);
		sx::sys::TaskManager::RemoveTask(&g->m_CleanupTask);
		CS.Leave();

		//  check to see that this texture use tasks in task manager
		if (g->m_LoaderTask[0].Executing() ||
			g->m_LoaderTask[1].Executing() ||
			g->m_LoaderTask[2].Executing() ||
			g->m_CleanupTask.Executing())
		{
			DerivedTaskTextureDestroy* freeTask = sx_new( DerivedTaskTextureDestroy );
			freeTask->m_texture = g;

			//  send this task to the main thread
			sx::sys::TaskManager::AddTask(freeTask, 0);
		}
		else
		{
			sx_delete_and_null(g);
		}
	}

	void Texture::Manager::ClearAll( void )
	{
		if (TextureMap.IsEmpty()) return;

		String str = L"Releasing remains Texture(s) :\r\n\t\t\t";

		for (sxMapTexture::Iterator it = TextureMap.First(); !it.IsLast(); it++)
		{
			DerivedTexture* g = (DerivedTexture*)(*it);
			str << (g->GetSource()!=NULL ? g->GetSource() : L" ??? no name ???") << L"\r\n\t\t\t";
				//<< L" [ " << g->m_RefCount << L" refCount ]\r\n\t\t\t";
			
			//  remove any tasks exist in task list
			sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[0]);
			sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[1]);
			sx::sys::TaskManager::RemoveTask(&g->m_LoaderTask[2]);
			sx::sys::TaskManager::RemoveTask(&g->m_CleanupTask);

			//  check to see that this texture use tasks in task manager
			if (g->m_LoaderTask[0].Executing() ||
				g->m_LoaderTask[1].Executing() ||
				g->m_LoaderTask[2].Executing() ||
				g->m_CleanupTask.Executing())
			{
				DerivedTaskTextureDestroy* freeTask = sx_new( DerivedTaskTextureDestroy );
				freeTask->m_texture = g;

				//  send this task to the main thread
				sx::sys::TaskManager::AddTask(freeTask, 0);
			}
			else
			{
				sx_delete_and_null(g);
			}
		}
		TextureMap.Clear();

		sxLog::Log(str);
	}

	void Texture::Manager::SetLOD( UINT LOD )
	{
		Texture3D::SetLoadingLOD(LOD);

		if (TextureMap.IsEmpty()) return;
		for (sxMapTexture::Iterator it = TextureMap.First(); !it.IsLast(); it++)
		{
			DerivedTexture* g = (DerivedTexture*)(*it);
			
			if ( g->GetD3DTexture() )
			{
				TextureDesc desc;
				g->GetDesc(desc);
				if ( desc.Format != D3DFMT_A8R8G8B8 && !String::IsFullPath(g->m_Src) )
				{
					g->SetLOD(LOD);
				}
			}
		}
	}

	UINT Texture::Manager::GetLOD( void )
	{
		return Texture3D::GetLoadingLOD();
	}

	UINT Texture::Manager::GetMaxLevels( void )
	{
		return Texture3D::GetMaxLevels();
	}

	UINT Texture::Manager::GetMaxWidth( void )
	{
		return Texture3D::GetMaxWidth();
	}

	bool& Texture::Manager::LoadInThread( void )
	{
		return s_loadInThread;
	}

	Map<UINT, sx::d3d::PTexture>::Iterator& Texture::Manager::GetFirst( void )
	{
		static sxMapTexture::Iterator it = TextureMap.First();
		it = TextureMap.First();
		return it;
	}

	
} } // namespace sx { namespace d3d
